#pragma once

#include <string_view>
#include <variant>

struct JsonNull
{
    bool is_null() const
    {
        return true;
    }
};

struct JsonValue
{

    std::variant<JsonNull> _type;

    const bool is_null() const
    {
        return std::visit([](const auto &x) { return x.is_null(); }, _type);
    }
};

JsonValue parse_json(std::string_view source);