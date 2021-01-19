#include "chat_message.h"
#include "chat_printer.h"

#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::tcp;

namespace NChat {

//class TCycledEndpointIterator {
//
//public:
//    explicit TCycledEndpointIterator(tcp::resolver::iterator begin)
//        : BeginIt_(begin)
//    {
//    }
//
//    TCycledEndpointIterator& operator++() {
//        CurrentIt_++;
//        return *this;
//    }
//
//    tcp::resolver::iterator Current() const {
//        return CurrentIt_;
//    }
//
//private:
//    tcp::resolver::iterator BeginIt_;
//    tcp::resolver::iterator CurrentIt_;
//};

//class TPrettyPrinter {
//public:
//    TPrettyPrinter()
//private:
//    bool Started_ = false;
//};

class TChatClient
{
public:
    TChatClient(boost::asio::io_service& io_service,
        tcp::resolver::iterator endpointIt)
        : IOService_(io_service)
        , Socket_(io_service)
    {
        tcp::endpoint endpoint = *endpointIt;
        Socket_.async_connect(
            endpoint,
            boost::bind(&TChatClient::HandleConnect, this, boost::asio::placeholders::error, ++endpointIt));
    }

    void Write(const TMessageProto& msg) {
        TNetMessage serialized = TNetMessage::FromProto(msg);
        Write(serialized);
    }

    void Write(const TNetMessage& msg) {
        IOService_.post(boost::bind(&TChatClient::DoWrite, this, msg));
    }

    void Close() {
        IOService_.post(boost::bind(&TChatClient::DoClose, this));
    }

    bool ConnectionError() const {
        return ConnectionError_;
    }

    bool Stopped() const {
        return Stopped_;
    }

private:

    void HandleConnect(const boost::system::error_code& error, tcp::resolver::iterator endpointIt) {
        if (!error) {
            boost::asio::async_read(
                Socket_,
                boost::asio::buffer(ReadMsg_.MutableHeader(), TChatMessage::HeaderLength),
                boost::bind(&TChatClient::HandleReadHeader, this,
                    boost::asio::placeholders::error));
        } else if (endpointIt != tcp::resolver::iterator()) {
            Socket_.close();
            tcp::endpoint endpoint = *endpointIt;
            Socket_.async_connect(
                endpoint,
                boost::bind(&TChatClient::HandleConnect, this, boost::asio::placeholders::error, ++endpointIt));
        } else {
            ConnectionError_ = true;
            Stopped_ = true;
        }
    }

    void HandleReadHeader(const boost::system::error_code& error) {
        if (!error && ReadMsg_.DecodeHeader()) {
            boost::asio::async_read(Socket_,
                boost::asio::buffer(ReadMsg_.MutableBody(), ReadMsg_.BodyLength()),
                boost::bind(&TChatClient::HandleReadBody, this,
                    boost::asio::placeholders::error));
        } else {
            DoClose();
        }
    }

    void HandleReadBody(const boost::system::error_code& error) {
        TMessageProto proto;
        if (!error && !proto.ParseFromString(std::string(ReadMsg_.Body()))) {
            std::cerr << "ParseFromString failed" << std::endl;
        } else if (!error) {
            PrettyPrint(proto);
            boost::asio::async_read(Socket_,
                boost::asio::buffer(ReadMsg_.MutableHeader(), TChatMessage::HeaderLength),
                boost::bind(&TChatClient::HandleReadHeader, this, boost::asio::placeholders::error));
        } else {
            DoClose();
        }
    }

    void DoWrite(TNetMessage msg) {
        bool writeInProgress = !WriteMsgs_.empty();
        WriteMsgs_.push_back(msg);
        if (!writeInProgress) {
            boost::asio::async_write(
                Socket_,
                boost::asio::buffer(WriteMsgs_.front().Data()),
                boost::bind(&TChatClient::HandleWrite, this, boost::asio::placeholders::error));
        }
    }

    void HandleWrite(const boost::system::error_code& error) {
        if (!error) {
            WriteMsgs_.pop_front();
            if (!WriteMsgs_.empty()) {
                boost::asio::async_write(
                    Socket_,
                    boost::asio::buffer(WriteMsgs_.front().Data()),
                    boost::bind(&TChatClient::HandleWrite, this, boost::asio::placeholders::error));
            }
        } else {
            DoClose();
        }
    }

    void DoClose() {
        Socket_.close();
        Stopped_ = true;
    }

    void PrettyPrint(const TMessageProto& messageProto) {
        ChatPrinter_.PrintMessage(messageProto);
    }

private:
    bool Stopped_ = false;
    bool ConnectionError_ = false;
    boost::asio::io_service& IOService_;
    tcp::socket Socket_;
    TNetMessage ReadMsg_;
    TNetMessageQueue WriteMsgs_;
    TChatPrinter ChatPrinter_;
};

}