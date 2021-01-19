#pragma once

#include <boost/asio/buffer.hpp>

#include <string>
#include <string_view>
#include <cassert>
#include <deque>

namespace NChat {

class TChatMessage {
public:
    static constexpr size_t HeaderLength = 4;
    static constexpr size_t MaxBodyLength = 4096;

public:
    TChatMessage() = default;

    static TChatMessage FromString(const std::string& message) {
        TChatMessage msg;
        msg.BodyLength_ = message.length();
        std::copy(message.begin(), message.end(), msg.MutableBody());
        msg.EncodeHeader();
        return msg;
    }

    char* MutableHeader() {
        return Data_;
    }

    char* MutableBody() {
        return Data_ + HeaderLength;
    }

    std::string_view Data() const {
        return std::string_view(Data_, HeaderLength + BodyLength_);
    }

    std::string_view Body() const {
        return std::string_view(Data_ + HeaderLength, BodyLength_);
    }

    size_t BodyLength() const {
        return BodyLength_;
    }

    bool DecodeHeader() {
        BodyLength_ = std::stoll(std::string(Data_, HeaderLength));
         if (BodyLength_ > MaxBodyLength) {
             BodyLength_ = 0;
         }
         return BodyLength_ != 0;
    }

    void EncodeHeader() {
        std::string header = std::to_string(BodyLength_);
        memcpy(Data_, header.data(), HeaderLength);
    }

private:
    char Data_[HeaderLength + MaxBodyLength] = "";
    size_t BodyLength_ = 0;
};

using TChatMessageQueue = std::deque<TChatMessage>;

} // namespace NChat