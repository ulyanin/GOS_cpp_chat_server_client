#include <chat_server/chat_message.h>

#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::tcp;

namespace NChat {

class TCycledEndpointIterator {

public:
    explicit TCycledEndpointIterator(tcp::resolver::iterator begin)
        : BeginIt_(begin)
    {
    }

    TCycledEndpointIterator& operator++() {
        CurrentIt_++;
        return *this;
    }

    tcp::resolver::iterator Current() const {
        return CurrentIt_;
    }

private:
    tcp::resolver::iterator BeginIt_;
    tcp::resolver::iterator CurrentIt_;
};

class TChatClient
{
public:
    TChatClient(boost::asio::io_service& io_service,
        tcp::resolver::iterator endpointIt)
        : IOService_(io_service)
        , Socket_(io_service)
    {
        tcp::endpoint endpoint = *endpointIt;
        std::cerr << "Socket_.async_connect begin" << std::endl;
        Socket_.async_connect(
            endpoint,
            boost::bind(&TChatClient::HandleConnect, this, boost::asio::placeholders::error, ++endpointIt));
    }

    void Write(const TChatMessage& msg) {
        std::cerr << "TChatClient::Write(msg = '" << msg.Body() << "')" << std::endl;
        IOService_.post(boost::bind(&TChatClient::DoWrite, this, msg));
    }

    void Close() {
        IOService_.post(boost::bind(&TChatClient::DoClose, this));
    }

private:

    void HandleConnect(const boost::system::error_code& error, tcp::resolver::iterator endpointIt) {
        std::cerr << "TChatClient::HandleConnect(error = " << (bool)(error) << ")" << std::endl;
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
            std::cerr << "endpointIt == tcp::resolver::iterator()" << std::endl;
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
        if (!error) {
            std::cout << ReadMsg_.Body() << "\n";
            boost::asio::async_read(Socket_,
                boost::asio::buffer(ReadMsg_.MutableHeader(), TChatMessage::HeaderLength),
                boost::bind(&TChatClient::HandleReadHeader, this, boost::asio::placeholders::error));
        } else {
            DoClose();
        }
    }

    void DoWrite(TChatMessage msg) {
        std::cerr << "TChatClient::DoWrite(msg = '" << msg.Body() << "')" << std::endl;
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
        std::cerr << "TChatClient::HandleWrite() " << "WriteMsgs_.size() = " << WriteMsgs_.size() << std::endl;
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
        std::cerr << "DoClose()" << std::endl;
        Socket_.close();
    }

private:
    boost::asio::io_service& IOService_;
    tcp::socket Socket_;
    TChatMessage ReadMsg_;
    TChatMessageQueue WriteMsgs_;
};

}