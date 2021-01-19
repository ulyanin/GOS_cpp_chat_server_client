#pragma once

#include <string>

namespace NChat {

bool StartsWith(std::string_view string, std::string_view sub) {
    return string.size() >= sub.size() && string.substr(0, sub.size()) == sub;
}

}