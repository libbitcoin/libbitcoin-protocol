/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_PROTOCOL_CONFIG_SODIUM_HPP
#define LIBBITCOIN_PROTOCOL_CONFIG_SODIUM_HPP

#include <iostream>
#include <string>
#include <vector>
#include <bitcoin/system.hpp>
#include <bitcoin/protocol/define.hpp>

namespace libbitcoin {
namespace protocol {

/// Serialization helper for base58 sodium keys.
class BCP_API sodium
{
public:
    /// A list of base85 values.
    /// This must provide operator<< for ostream in order to be used as a
    /// boost::program_options default_value.
    typedef std::vector<sodium> list;

    sodium();
    sodium(const std::string& base85);
    sodium(const system::hash_digest& value);
    sodium(const sodium& other);

    /// True if the key is initialized.
    operator bool() const;

    /// Overload cast to internal type.
    operator const system::hash_digest&() const;

    /// Get the key as a base85 encoded (z85) string.
    std::string to_string() const;

    friend std::istream& operator>>(std::istream& input,
        sodium& argument);
    friend std::ostream& operator<<(std::ostream& output,
        const sodium& argument);

private:
    system::hash_digest value_;
};

} // namespace protocol
} // namespace libbitcoin

#endif
