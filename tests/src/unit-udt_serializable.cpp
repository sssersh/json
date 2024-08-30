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
// TODO: тест на имя поля в UTF (в examples тоже наверное добавить)
// TODO: snake case?

#include "nlohmann/serializable.h"
#include "nlohmann/serializer.h"

struct BaseTypes : Serialization::JsonNlohmann::Serializable<BaseTypes>
{
    Serializable<bool> b = { this, { "bool" } };

    Serializable<float> f = { this, { "float" } };
    Serializable<double> d = { this, { "double" } };

    Serializable<int32_t> i32 = { this, { "int32_t" } };
    Serializable<int64_t> i64 = { this, { "int64_t" } };

    Serializable<uint32_t> u32 = { this, { "uint32_t" } };
    Serializable<uint64_t> u64 = { this, { "uint64_t" } };
};

TEST_CASE("Serialize/deserialize serializable class with base types members" * doctest::test_suite("udt_serializable"))
{
    std::string json = R"({"bool":true,"double":2.0,"float":-1.0,"int32_t":-32,"int64_t":-64,"uint32_t":32,"uint64_t":64})";

    SECTION("serialize")
    {
        BaseTypes obj;

        obj.b = true;
        obj.d = 2.0;
        obj.f = -1.0;
        obj.i32 = -32;
        obj.i64 = -64;
        obj.u32 = 32;
        obj.u64 = 64;

        auto res = Serialization::serialize(obj);

        CHECK(res == json);
    }
    SECTION("deserialize")
    {
        auto obj = Serialization::deserialize<BaseTypes>(json);

        CHECK(*obj.b == true);
        CHECK(*obj.d == 2.0);
        CHECK(*obj.f == -1.0);

        CHECK(*obj.i32 == -32);
        CHECK(*obj.i64 == -64);
        CHECK(*obj.u32 == 32);
        CHECK(*obj.u64 == 64);
    }
}

namespace
{
struct TypeWithContainers : Serialization::JsonNlohmann::Serializable<TypeWithContainers>
{
    Serializable<std::string> str = {this, {"string"}};
    Serializable<std::vector<int>> vector = {this, {"vector"}};

    // TODO (s.dobychin@vk.team): implement other containers
    // Serializable<std::array<int, 3>> array = { this, { "array_name" } };
    // Serializable<std::set<std::string>> set = { this, "set_name" };
};
}

TEST_CASE("Serialize/deserialize serializable class with containers members" * doctest::test_suite("udt_serializable"))
{
    std::string json = R"({"string":"str","vector":[1,2,3]})";
    std::string emptyJson = R"({"string":"","vector":[]})";

    SECTION("deserialize")
    {
        auto obj = Serialization::deserialize<TypeWithContainers>(json);

        CHECK(*obj.str == "str");

        CHECK(obj.vector->size() == 3);
        CHECK(obj.vector->at(0) == 1);
        CHECK(obj.vector->at(1) == 2);
        CHECK(obj.vector->at(2) == 3);
    }
    SECTION("deserialize empty")
    {
        auto obj = Serialization::deserialize<TypeWithContainers>(emptyJson);

        CHECK(*obj.str == "");
        CHECK(obj.vector->size() == 0);
    }
    SECTION("serialize")
    {
        TypeWithContainers obj;

        obj.str = "str";

        obj.vector->resize(3);
        obj.vector->at(0) = 1;
        obj.vector->at(1) = 2;
        obj.vector->at(2) = 3;

        auto res = Serialization::serialize<TypeWithContainers>(obj);
        CHECK(res == json);
    }
    SECTION("serialize empty")
    {
        TypeWithContainers obj;

        auto res = Serialization::serialize<TypeWithContainers>(obj);

        CHECK(res == emptyJson);
    }
}

namespace
{
struct MostNested : public Serialization::JsonNlohmann::Serializable<MostNested>
{
    Serializable<float> v = {this, {"nameMostNested"}};
};

struct Nested : Serialization::JsonNlohmann::Serializable<Nested>
{
    Serializable<MostNested> m = {this, {"nameM"}};
};

struct Top : Serialization::JsonNlohmann::Serializable<Top>
{
    Serializable<Nested> n = {this, {"nameN"}};
};
}

TEST_CASE("Serialize/deserialize serializable class with nested serializable class" * doctest::test_suite("udt_serializable"))
{
    std::string json = R"({"nameN":{"nameM":{"nameMostNested":1.0}}})";

    SECTION("serialize")
    {
        Top obj;
        obj.n->m->v = 1.0;

        auto res = Serialization::serialize<Top>(obj);

        CHECK(json == res);
    }
    SECTION("deserialize")
    {
        auto obj = Serialization::deserialize<Top>(json);

        CHECK(*obj.n->m->v == 1.0);
    }
}

namespace {
struct NotOptional : Serialization::JsonNlohmann::Serializable<NotOptional>
{
    Serializable<int> a = {this, {"name"}};
};

struct Optional : Serialization::JsonNlohmann::Serializable<Optional>
{
    Serializable<std::optional<int>> a = {this, {"name"}};
};
}

TEST_CASE("Serialize/deserialize serializable class with optional member" * doctest::test_suite("udt_serializable"))
{
    std::string json = R"({"name":1})";
    std::string empty_json = R"({})";

    SECTION("deserialize not existed optional")
    {
        auto obj = Serialization::deserialize<NotOptional>(R"({"name":1})");
        CHECK(*obj.a == 1);
    }
    SECTION("deserialize not existed not optional")
    {
        CHECK_THROWS_AS(Serialization::deserialize<NotOptional>(empty_json), std::runtime_error&);
    }
    SECTION("deserialize existed optional")
    {
        auto obj = Serialization::deserialize<Optional>(json);
        CHECK(obj.a->has_value());
        CHECK(*obj.a == 1);
    }
    SECTION("deserialize not existed optional")
    {
        auto obj = Serialization::deserialize<Optional>(empty_json);
        CHECK(!obj.a->has_value());
    }
    SECTION("serialize existed not optional")
    {
        NotOptional obj;
        obj.a = 1;

        auto json = Serialization::serialize<NotOptional>(obj);
        CHECK(json == json);
    }
    SECTION("serialize existed optional")
    {
        Optional obj;
        obj.a = 1;

        auto json = Serialization::serialize<Optional>(obj);
        CHECK(json == json);
    }
    SECTION("serialize not existed optional")
    {
        Optional obj;
        auto json = Serialization::serialize<Optional>(obj);
        CHECK(json == empty_json);
    }
}

namespace
{

enum Enum
{
    False,
    True
};

NLOHMANN_JSON_SERIALIZE_ENUM(Enum,
                             {
                                 { False, "False" },
                                 { True, "True" }
                             })

struct TypeWithEnum : Serialization::JsonNlohmann::Serializable<TypeWithEnum>
{
    Serializable<Enum> n = { this, { "enum" } };
};

}

TEST_CASE("Serialize/deserialize serializable class with enum" * doctest::test_suite("udt_serializable"))
{
    std::string json_true = R"({"enum":"True"})";
    std::string json_false = R"({"enum":"False"})";

    SECTION("deserialize")
    {
        {
            auto res = Serialization::deserialize<TypeWithEnum>(json_true);
            CHECK(*res.n == Enum::True);
        }

        {
            auto res = Serialization::deserialize<TypeWithEnum>(json_false);
            CHECK(*res.n == Enum::False);
        }
    }
    SECTION("serialize")
    {
        TypeWithEnum obj;

        {
            obj.n = Enum::True;
            auto res = Serialization::serialize<TypeWithEnum>(obj);

            CHECK(res == json_true);
        }

        {
            obj.n = Enum::False;
            auto res = Serialization::serialize<TypeWithEnum>(obj);

            CHECK(res == json_false);
        }
    }
}

namespace {
struct MostBase : Serialization::JsonNlohmann::Serializable<MostBase>
{
    Serializable<int> mostBase = {this, {"most_base"}};
};

struct Base : MostBase
{
    Serializable<int> base = {this, {"base"}};
};

struct Derived : Base
{
    Serializable<int> derived = {this, {"derived"}};
};
}

TEST_CASE("Serialize/deserialize serializable classes inherited from other serializable class" * doctest::test_suite("udt_serializable"))
{
    std::string json = R"({"base":2,"derived":3,"most_base":1})";

    SECTION("deserialize")
    {
        auto res = Serialization::deserialize<Derived>(json);

        CHECK(*res.derived == 0x03);
        CHECK(*res.base == 0x02);
        CHECK(*res.mostBase == 0x01);
    }
    SECTION("serialize")
    {
        Derived obj;

        obj.derived = 0x03;
        obj.base = 0x02;
        obj.mostBase = 0x01;

        auto res = Serialization::serialize(obj);

        CHECK(res == json);
    }
}
