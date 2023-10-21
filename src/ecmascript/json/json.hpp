#pragma once

#include <string_view>

struct JsonValue
{

    const bool is_null() const
    {
        return false;
    }
};

JsonValue parse_json(std::string_view source);