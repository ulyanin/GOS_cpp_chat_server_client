#pragma once

#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "chat_participant.h"
#include "chat_session.h"

#include <iostream>

using boost::asio::ip::tcp;


namespace NChat {

class TChatSession
    : public TChatParticipant
    , public boost::enable_shared_from_this<TChatSession>
{
public:

    TChatSession(boost::asio::io_service& io_service, TChatRoom* room)
        : Socket_(io_service)
        , Room_(room)
    {
        std::cerr << "TChatSession() " << static_cast<void*>(this) << std::endl;
    }

    virtual ~TChatSession() {
        std::cerr << "~TChatSession() " << static_cast<void*>(this) << std::endl;
    }

    tcp::socket& MutableSocket() {
        return Socket_;
    }

    void Start() {
        Room_->Join(shared_from_this());
        boost::asio::async_read(
            Socket_,
            boost::asio::buffer(ReadMsg_.MutableHeader(), TChatMessage::HeaderLength),
            boost::bind(&TChatSession::HandleReadHeader, shared_from_this(), boost::asio::placeholders::error));

    }

    void Deliver(const TChatMessage& msg) override {
        bool writeInProgress = !WriteMsgs_.empty();
        WriteMsgs_.push_back(msg);
        if (!writeInProgress) {
            boost::asio::async_write(
                Socket_,
                boost::asio::buffer(WriteMsgs_.front().Data()),
                boost::bind(&TChatSession::HandleWrite, shared_from_this(), boost::asio::placeholders::error));
        }
    }

    void HandleReadHeader(const boost::system::error_code& error) {
        if (!error && ReadMsg_.DecodeHeader()) {
            boost::asio::async_read(
                Socket_,
                boost::asio::buffer(ReadMsg_.MutableBody(), ReadMsg_.BodyLength()),
                boost::bind(&TChatSession::HandleReadBody, shared_from_this(), boost::asio::placeholders::error));
        } else {
            Room_->Leave(shared_from_this());
        }
    }

    void HandleReadBody(const boost::system::error_code& error) {
        if (!error) {
            std::cerr << "TChatServer::HandleReadBody, msg = '" << ReadMsg_.Body() << "'" << std::endl;
            Room_->Deliver(ReadMsg_);
            boost::asio::async_read(Socket_,
                boost::asio::buffer(ReadMsg_.MutableHeader(), TChatMessage::HeaderLength),
                boost::bind(&TChatSession::HandleReadHeader, shared_from_this(),
                    boost::asio::placeholders::error));
        } else {
            Room_->Leave(shared_from_this());
        }
    }

    void HandleWrite(const boost::system::error_code& error)
    {
        if (!error) {
            WriteMsgs_.pop_front();
            if (!WriteMsgs_.empty()) {
                boost::asio::async_write(Socket_,
                    boost::asio::buffer(WriteMsgs_.front().Data()),
                    boost::bind(&TChatSession::HandleWrite, shared_from_this(),
                        boost::asio::placeholders::error));
            }
        } else {
            Room_->Leave(shared_from_this());
        }
    }

private:
    tcp::socket Socket_;
    TChatRoom* Room_ = nullptr;
    TChatMessage ReadMsg_;
    TChatMessageQueue WriteMsgs_;
};

using TChatSessionPtr = boost::shared_ptr<TChatSession>;

} // namespace NChat