#pragma once

#include "message.pb.h"

#include <iostream>

namespace NChat {

class TChatPrinter {
public:
    TChatPrinter() = default;

    void PrintMessage(const TMessageProto& messageProto) {
//        if (!Started_) {
//        }
        std::cout << Line() << std::endl;
        Started_ = true;
        std::cout << "| from: " << messageProto.login() << std::endl;
        std::cout << "| content:" << std::endl;
        std::cout << "| " << messageProto.content() << std::endl;
        std::cout << Line() << std::endl;
    }

private:

    std::string Line() const {
        size_t tmp = ScreenWidth - 2;
        return "+" + std::string(tmp, '-') + "+";
    }

private:
    bool Started_ = false;
    static constexpr size_t ScreenWidth = 80;

};

}
