/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/protocol/config/sodium.hpp>

#include <algorithm>
#include <sstream>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/define.hpp>

namespace libbitcoin {
namespace protocol {

using namespace bc::system;

sodium::sodium() NOEXCEPT
  : value_(null_hash)
{
}

sodium::sodium(const std::string& base85) THROWS
  : sodium()
{
    std::stringstream(base85) >> *this;
}

sodium::sodium(const hash_digest& value) NOEXCEPT
  : value_(value)
{
}

sodium::operator const hash_digest&() const NOEXCEPT
{
    return value_;
}

sodium::operator bool() const NOEXCEPT
{
    return value_ != null_hash;
}

std::string sodium::to_string() const NOEXCEPT
{
    std::stringstream value;

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    value << *this;
    return value.str();
    BC_POP_WARNING()
}

std::istream& operator>>(std::istream& input, sodium& argument) THROWS
{
    std::string base85;
    input >> base85;

    data_chunk out_value;
    if (!decode_base85(out_value, base85) || out_value.size() != hash_size)
    {
        throw istream_exception(base85);
    }

    std::copy_n(out_value.begin(), hash_size, argument.value_.begin());
    return input;
}

std::ostream& operator<<(std::ostream& output,
    const sodium& argument) THROWS
{
    std::string decoded;

    // Base85 requires four byte alignment (hash_digest is 32).
    if (!encode_base85(decoded, argument.value_))
    {
        throw istream_exception(decoded);
    }

    output << decoded;
    return output;
}

} // namespace protocol
} // namespace libbitcoin
