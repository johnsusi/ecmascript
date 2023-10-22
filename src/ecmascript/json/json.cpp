#include "json.hpp"

JsonValue parse_json(std::string_view source)
{
    if (source == "null")
        return JsonValue{JsonNull{}};
    return {};
}