//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++ (supporting code)
// |  |  |__   |  |  | | | |  version 3.11.3
// |_____|_____|_____|_|___|  https://github.com/nlohmann/json
//
// SPDX-FileCopyrightText: 2013-2023 Niels Lohmann <https://nlohmann.me>
// SPDX-License-Identifier: MIT

#include "doctest_compatibility.h"

#include <nlohmann/json.hpp>
using nlohmann::json;

// UDT - по ходу user defined types (потому что в udl - user-defined literals)

TEST_CASE("Serialize/deserialize classes, defined as erializable" * doctest::test_suite("udt"))
{
    SECTION("base")
    {
        CHECK(true);
        CHECK(true);
    }
}
