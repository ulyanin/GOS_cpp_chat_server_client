#pragma once

#include "chat_message.h"

#include <boost/shared_ptr.hpp>

namespace NChat {

class TChatParticipant {

public:
    virtual ~TChatParticipant() = default;

    virtual void Deliver(const TNetMessage& msg) = 0;
};

using TChatParticipantPtr = boost::shared_ptr<TChatParticipant>;

} // namespace NChat