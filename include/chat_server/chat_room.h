#pragma once

#include "chat_participant.h"

#include <boost/bind.hpp>

#include <set>
#include <deque>

namespace NChat {

class TChatRoom {
public:
    TChatRoom() = default;

    void Join(TChatParticipantPtr chatParticipant) {
        Participants_.insert(chatParticipant);
        std::for_each(
            History_.begin(),
            History_.end(),
            [chatParticipant](const TChatMessage& chatMessage) {
                chatParticipant->Deliver(chatMessage);
            });
    }

    void Leave(TChatParticipantPtr chatParticipantPtr) {
        Participants_.erase(chatParticipantPtr);
    }

    void Deliver(const TChatMessage& msg) {
        History_.push_back(msg);
        while (History_.size() > MaxHistorySize) {
            History_.pop_front();
        }

        for (const TChatParticipantPtr& participant : Participants_) {
            participant->Deliver(msg);
        }
    }

private:
    static constexpr size_t MaxHistorySize = 128;
    std::set<TChatParticipantPtr> Participants_;
    TChatMessageQueue History_;
    TChatMessageQueue Queue_;
};

} // namespace NChat