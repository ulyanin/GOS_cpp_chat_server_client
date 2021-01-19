#include "chat_message.h"
#include "chat_room.h"
#include "chat_participant.h"
#include "chat_session.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include <algorithm>
#include <iostream>

namespace NChat {

class TChatServer {
public:
    TChatServer(boost::asio::io_service* ioService, const tcp::endpoint& endpoint)
        : IOService_(ioService)
        , Acceptor_(*IOService_, endpoint)
        , ChatRoom_()
    {
        TChatSessionPtr newSession(new TChatSession(*IOService_, &ChatRoom_));
        Acceptor_.async_accept(
            newSession->MutableSocket(),
            boost::bind(&TChatServer::HandleAccept, this, newSession, boost::asio::placeholders::error));
    }

    void HandleAccept(TChatSessionPtr session, const boost::system::error_code& error) {
        if (!error) {
            session->Start();
            TChatSessionPtr newSession(new TChatSession(*IOService_, &ChatRoom_));
            Acceptor_.async_accept(
                newSession->MutableSocket(),
                boost::bind(&TChatServer::HandleAccept, this, newSession, boost::asio::placeholders::error));
        }
    }

private:
    boost::asio::io_service* IOService_ = nullptr;
    tcp::acceptor Acceptor_;
    TChatRoom ChatRoom_;
};

using TChatServerPtr = boost::shared_ptr<TChatServer>;

} // namespace NChat