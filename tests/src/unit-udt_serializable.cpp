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

#include <unordered_map>
#include <set>

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
    Serializable<std::array<int, 3>> array = {this, {"array"}};
    Serializable<std::unordered_map<std::string, int>> map = { this, {"map"} };
    Serializable<std::set<int>> set = { this, {"set"} };
    Serializable<std::string> str = {this, {"string"}};
    Serializable<std::vector<int>> vector = {this, {"vector"}};
};
}

TEST_CASE("Serialize/deserialize serializable class with containers members" * doctest::test_suite("udt_serializable"))
{
    std::string json = R"({"array":[1,2,3],"map":{"11":12,"13":14},"set":[4,5,6],"string":"str","vector":[7,8,9]})";
    std::string empty_json_input = R"({"array":[],"map":{},"set":[],"string":"","vector":[]})";
    std::string empty_json_result = R"({"array":[0,0,0],"map":{},"set":[],"string":"","vector":[]})";

    SECTION("deserialize")
    {
        auto obj = Serialization::deserialize<TypeWithContainers>(json);

        CHECK(*obj.str == "str");

        CHECK(obj.array->at(0) == 1);
        CHECK(obj.array->at(1) == 2);
        CHECK(obj.array->at(2) == 3);

        CHECK(obj.map->size() == 2);
        CHECK((*obj.map)["11"] == 12);
        CHECK((*obj.map)["13"] == 14);

        CHECK(obj.set->find(4) != obj.set->end());
        CHECK(obj.set->find(5) != obj.set->end());
        CHECK(obj.set->find(6) != obj.set->end());

        CHECK(obj.vector->size() == 3);
        CHECK(obj.vector->at(0) == 7);
        CHECK(obj.vector->at(1) == 8);
        CHECK(obj.vector->at(2) == 9);
    }
    SECTION("deserialize empty")
    {
        auto obj = Serialization::deserialize<TypeWithContainers>(empty_json_input);

        CHECK(*obj.str == "");
        CHECK(obj.array->at(0) == 0);
        CHECK(obj.array->at(1) == 0);
        CHECK(obj.array->at(2) == 0);
        CHECK(obj.map->size() == 0);
        CHECK(obj.set->size() == 0);
        CHECK(obj.vector->size() == 0);
    }
    SECTION("serialize")
    {
        TypeWithContainers obj;

        obj.str = "str";

        obj.array->at(0) = 1;
        obj.array->at(1) = 2;
        obj.array->at(2) = 3;

        (*obj.map)["11"] = 12;
        (*obj.map)["13"] = 14;

        obj.set->insert(4);
        obj.set->insert(5);
        obj.set->insert(6);

        obj.vector->resize(3);
        obj.vector->at(0) = 7;
        obj.vector->at(1) = 8;
        obj.vector->at(2) = 9;

        auto res = Serialization::serialize<TypeWithContainers>(obj);
        CHECK(res == json);
    }
    SECTION("serialize empty")
    {
        TypeWithContainers obj;

        auto res = Serialization::serialize<TypeWithContainers>(obj);

        CHECK(res == empty_json_result);
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

namespace {
// TODO: тест на unique_ptr тоже
struct RecursiveType : Serialization::JsonNlohmann::Serializable<RecursiveType>
{
    Serializable<std::shared_ptr<RecursiveType>> recursive = { this, { "recursive" } };
    Serializable<int> x = { this, { "x" } };
};
}

TEST_CASE("Serialize/deserialize serializable classes which contain itself recursively" * doctest::test_suite("udt_serializable"))
{
    std::string json = R"({"recursive":{"recursive":{"x":3},"x":2},"x":1})";

    SECTION("deserialize")
    {
        auto res = Serialization::deserialize<RecursiveType>(json);
        CHECK(*res.x == 0x01);

        auto& recursive1 = *(*res.recursive);
        CHECK(*recursive1.x == 0x02);

        auto& recursive2 = *(*recursive1.recursive);
        CHECK(*recursive2.x == 0x03);

        CHECK(recursive2.recursive->get() == nullptr);
    }
    SECTION("serialize")
    {
        RecursiveType obj;

        obj.x = 0x01;
        obj.recursive = std::make_shared<RecursiveType>();

        auto& recursive1 = *(*obj.recursive);
        recursive1.x = 0x02;
        recursive1.recursive = std::make_shared<RecursiveType>();

        auto& recursive2 = *(*recursive1.recursive);
        recursive2.x = 0x03;

        auto res = Serialization::serialize<RecursiveType>(obj);

        CHECK(res == json);
    }
}

namespace
{
struct Payload : public Serialization::JsonNlohmann::Serializable<Payload>
{};

struct Payload1 : public Payload
{
    Serializable<std::string> str = { this, { "str" } };
};

struct Payload2 : public Payload
{
    Serializable<int> i = { this, { "int" } };
};

struct Polymorphic : public Serialization::JsonNlohmann::Serializable<Polymorphic>
{
    Serializable<int> type = { this, { "type" } };
    std::shared_ptr<Payload> payload;

    void deserialize(Serialization::ISerializedType& value) override
    {
        SerializableClass::deserialize(value);

        if (*type == 1)
        {
            Serialization::JsonNlohmann::SerializedType wrapper;
            wrapper.json = *payloadRaw;
            Payload1 p;
            p.deserialize(wrapper);
            payload = std::make_shared<Payload1>(std::move(p));
        }
        else if (*type == 2)
        {
            Serialization::JsonNlohmann::SerializedType wrapper;
            wrapper.json = *payloadRaw;
            Payload2 p;
            p.deserialize(wrapper);
            payload = std::make_shared<Payload2>(std::move(p));
        }
        else
        {
            throw std::runtime_error("Incorrect type of Polymorphic type");
        }

        payloadRaw = {};
    }

    void serialize(Serialization::ISerializedType& value) const override
    {
        Serialization::JsonNlohmann::SerializedType data;

        if (*type == 1)
        {
            auto& typed = static_cast<Payload1&>(*payload);
            typed.serialize(data);
        }
        else if (*type == 2)
        {
            auto& typed = static_cast<Payload2&>(*payload);
            typed.serialize(data);
        }
        else
        {
            throw std::runtime_error("Incorrect type of Polymorphic type");
        }

        payloadRaw = data.json;

        SerializableClass::serialize(value);

        payloadRaw = {};
    }

  private:
    mutable Serializable<nlohmann::json> payloadRaw = { this, { "payload" } };
};
} // namespace

// Это в примеры перенести надо, а не в тесты

TEST_CASE("Serialize/deserialize polymorphic serializable class" * doctest::test_suite("udt_serializable"))
{
    std::string json_1 = R"({"payload":{"str":"data"},"type":1})";
    std::string json_2 = R"({"payload":{"int":555},"type":2})";

    SECTION("deserialize subtype 1")
    {
        auto res1 = Serialization::deserialize<Polymorphic>(json_1);
        auto& p1 = static_cast<Payload1&>(*res1.payload);

        CHECK(*res1.type == 1);
        CHECK(*p1.str == "data");
    }

    SECTION("deserialize subtype 2")
    {
        auto res2 = Serialization::deserialize<Polymorphic>(json_2);
        auto& p2 = static_cast<Payload2&>(*res2.payload);

        CHECK(*res2.type == 2);
        CHECK(*p2.i == 555);
    }
    SECTION("serialize subtype 1")
    {
        Polymorphic p;

        p.type = 1;
        p.payload = std::make_shared<Payload1>();

        auto& p1 = static_cast<Payload1&>(*p.payload);
        p1.str = "data";

        auto res1 = Serialization::serialize(p);

        CHECK(res1 == json_1);
    }
    SECTION("serialize subtype 2")
    {
        Polymorphic p;

        p.type = 2;
        p.payload = std::make_shared<Payload2>();

        auto& p2 = static_cast<Payload2&>(*p.payload);
        p2.i = 555;

        auto res2 = Serialization::serialize(p);

        CHECK(res2 == json_2);
    }
}
