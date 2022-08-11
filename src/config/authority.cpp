/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/protocol/config/authority.hpp>

#include <algorithm>
#include <exception>
#include <sstream>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/boost.hpp>
#include <bitcoin/protocol/network.hpp>

namespace libbitcoin {
namespace protocol {

static const ip_address null_ip_address
{
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }
};

// host:    [2001:db8::2] or  2001:db8::2  or 1.2.240.1
// returns: [2001:db8::2] or [2001:db8::2] or 1.2.240.1
static std::string to_host_name(const std::string& host) noexcept
{
    if (host.find(":") == std::string::npos || host.find("[") == 0)
        return host;

    return (boost::format("[%1%]") % host).str();
}

// host: [2001:db8::2] or 2001:db8::2 or 1.2.240.1
static std::string to_text(const std::string& host, uint16_t port) noexcept
{
    std::stringstream authority;
    authority << to_host_name(host);
    if (!is_zero(port))
        authority << ":" << port;

    return authority.str();
}

static std::string to_ipv6(const std::string& ipv4_address) noexcept
{
    return std::string("::ffff:") + ipv4_address;
}

static ipv6 to_ipv6(const ipv4& ipv4_address) noexcept
{
    boost::system::error_code ignore;

    // Create an IPv6 mapped IPv4 address via serialization.
    const auto ipv6 = to_ipv6(ipv4_address.to_string());
    return ipv6::from_string(ipv6, ignore);
}

static ipv6 to_ipv6(const boost::asio::ip::address& ip_address) noexcept
{
    if (ip_address.is_v6())
        return ip_address.to_v6();

    BC_ASSERT_MSG(ip_address.is_v4(),
        "The address must be either IPv4 or IPv6.");

    return to_ipv6(ip_address.to_v4());
}

static std::string to_ipv4_hostname(
    const boost::asio::ip::address& ip_address) noexcept
{
    // C++11: use std::regex.
    // std::regex requires gcc 4.9, so we are using boost::regex for now.
    static const boost::regex regular("^::ffff:([0-9\\.]+)$");

    const auto address = ip_address.to_string();
    boost::sregex_iterator it(address.begin(), address.end(), regular), end;
    if (it == end)
        return "";

    const auto& match = *it;
    return match[1];
}

static std::string to_ipv6_hostname(
    const boost::asio::ip::address& ip_address) noexcept
{
    // IPv6 URLs use a bracketed IPv6 address, see rfc2732.
    return (boost::format("[%1%]") % to_ipv6(ip_address)).str();
}

authority::authority() noexcept
  : ip_(null_ip_address), port_(0)
{
}

// authority: [2001:db8::2]:port or 1.2.240.1:port
authority::authority(const std::string& authority) noexcept(false)
{
    std::stringstream(authority) >> *this;
}

// host: [2001:db8::2] or 2001:db8::2 or 1.2.240.1
authority::authority(const std::string& host, uint16_t port) noexcept (false)
{
    std::stringstream(to_text(host, port)) >> *this;
}

authority::operator bool() const noexcept
{
    return port_ != 0;
}

const ipv6& authority::ip() const noexcept
{
    return ip_;
}

uint16_t authority::port() const noexcept
{
    return port_;
}

std::string authority::to_hostname() const noexcept
{
    auto ipv4_hostname = to_ipv4_hostname(ip_);
    return ipv4_hostname.empty() ? to_ipv6_hostname(ip_) : ipv4_hostname;
}

std::string authority::to_string() const noexcept
{
    std::stringstream value;
    value << *this;
    return value.str();
}

std::istream& operator>>(std::istream& input,
    authority& argument) noexcept(false)
{
    std::string value;
    input >> value;

    // C++11: use std::regex.
    // std::regex requires gcc 4.9, so we are using boost::regex for now.
    static const boost::regex regular(
        "^(([0-9\\.]+)|\\[([0-9a-f:\\.]+)])(:([0-9]{1,5}))?$");

    boost::sregex_iterator it(value.begin(), value.end(), regular), end;
    if (it == end)
        throw istream_exception(value);

    const auto& match = *it;
    std::string port(match[5]);
    std::string ip_address(match[3]);
    if (ip_address.empty())
        ip_address = to_ipv6(match[2]);

    try
    {
        argument.ip_ = ipv6::from_string(ip_address);
        argument.port_ = port.empty() ? 0 : boost::lexical_cast<uint16_t>(port);
    }
    catch (const std::exception&)
    {
        throw istream_exception(value);
    }

    return input;
}

std::ostream& operator<<(std::ostream& output,
    const authority& argument) noexcept
{
    output << to_text(argument.to_hostname(), argument.port());
    return output;
}

} // namespace protocol
} // namespace libbitcoin
