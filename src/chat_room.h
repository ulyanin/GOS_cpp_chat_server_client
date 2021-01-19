#pragma once

#include "chat_participant.h"
#include "chat_printer.h"
#include "chat_history.pb.h"

#include <boost/bind.hpp>

#include <set>
#include <deque>
#include <filesystem>
#include <fstream>

#include <google/protobuf/text_format.h>

namespace NChat {

class TChatRoom {
public:

    TChatRoom() {
        LoadHistory();
    }

    ~TChatRoom() {
        SaveHistory(true);
    }

    void Join(TChatParticipantPtr chatParticipant) {
        Participants_.insert(chatParticipant);
        std::for_each(
            History_.begin(),
            History_.end(),
            [chatParticipant](const TNetMessage& chatMessage) {
                chatParticipant->Deliver(chatMessage);
            });
    }

    void Leave(TChatParticipantPtr chatParticipantPtr) {
        Participants_.erase(chatParticipantPtr);
    }

    void Deliver(const TNetMessage& msg) {
        TMessageProto msgProto;
        if (!msgProto.ParseFromString(std::string(msg.Body()))) {
            std::cerr << "incompatible with TMessageProto message received" << std::endl;
        } else {
            ChatPrinter_.PrintMessage(msgProto);
        }
        History_.push_back(msg);
        while (History_.size() > MaxHistorySize) {
            History_.pop_front();
        }
        SaveHistory(false);

        for (const TChatParticipantPtr& participant : Participants_) {
            participant->Deliver(msg);
        }
    }

private:
    void LoadHistory() {
        if (!std::filesystem::exists(HistoryFile)) {
            return;
        }
        std::ifstream fileStream = std::ifstream(std::string(HistoryFile));
        std::string fileContent((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
        TChatHistory chatHistory;
        if (google::protobuf::TextFormat::ParseFromString(fileContent, &chatHistory)) {
            for (const TMessageProto& msg : chatHistory.history()) {
                ChatPrinter_.PrintMessage(msg);
                History_.push_back(TNetMessage::FromProto(msg));
            }
        }
    }

    void SaveHistory(bool verbose) {
        TChatHistory chatHistory;
        for (const TNetMessage& msg : History_) {
            TMessageProto msgProto;
            if (msgProto.ParseFromString(std::string(msg.Body()))) {
                *chatHistory.mutable_history()->Add() = std::move(msgProto);
            }
        }
        std::string buffer;
        google::protobuf::TextFormat::PrintToString(chatHistory, &buffer);
        {
            std::ofstream fileStream = std::ofstream(std::string(HistoryFile));
            fileStream << buffer << std::endl;
        }
        if (verbose) {
            std::cout << "history saved to " << HistoryFile << std::endl;
        }
    }

private:
    static constexpr size_t MaxHistorySize = 128;
    static constexpr std::string_view HistoryFile = "server_history.pb.txt";

    std::set<TChatParticipantPtr> Participants_;
    TNetMessageQueue History_;
    TChatPrinter ChatPrinter_;
};

} // namespace NChat