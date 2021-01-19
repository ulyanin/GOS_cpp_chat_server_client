#include <chat_server/chat_message.h>

#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::tcp;

namespace NChat {

class TChatClient
{
public:
    TChatClient(boost::asio::io_service& io_service,
        tcp::resolver::iterator endpointIt)
        : IOService_(io_service)
        , Socket_(io_service)
    {
        tcp::endpoint endpoint = *endpointIt;
        Socket_.async_connect(endpoint,
            boost::bind(&TChatClient::HandleConnect, this,
                boost::asio::placeholders::error, ++endpointIt));
    }

    void Write(const TChatMessage& msg) {
        IOService_.post(boost::bind(&TChatClient::DoWrite, this, msg));
    }

    void Close() {
        IOService_.post(boost::bind(&TChatClient::DoClose, this));
    }

private:

    void HandleConnect(const boost::system::error_code& error,
        tcp::resolver::iterator endpointIt)
    {
        if (!error) {
            boost::asio::async_read(
                Socket_,
                boost::asio::buffer(ReadMsg_.MutableHeader(), TChatMessage::HeaderLength),
                boost::bind(&TChatClient::HandleReadHeader, this,
                    boost::asio::placeholders::error));
        } else if (endpointIt != tcp::resolver::iterator()) {
            Socket_.close();
            tcp::endpoint endpoint = *endpointIt;
            Socket_.async_connect(endpoint,
                boost::bind(&TChatClient::HandleConnect, this,
                    boost::asio::placeholders::error, ++endpointIt));
        }
    }

    void HandleReadHeader(const boost::system::error_code& error)
    {
        if (!error && ReadMsg_.DecodeHeader())
        {
            boost::asio::async_read(Socket_,
                boost::asio::buffer(ReadMsg_.MutableBody(), ReadMsg_.BodyLength()),
                boost::bind(&TChatClient::HandleReadBody, this,
                    boost::asio::placeholders::error));
        }
        else
        {
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

    void DoWrite(TChatMessage msg)
    {
        bool write_in_progress = !WriteMsgs_.empty();
        WriteMsgs_.push_back(msg);
        if (!write_in_progress)
        {
            boost::asio::async_write(
                Socket_,
                boost::asio::buffer(WriteMsgs_.front().Data()),
                boost::bind(&TChatClient::handle_write, this, boost::asio::placeholders::error));
        }
    }

    void handle_write(const boost::system::error_code& error)
    {
        if (!error)
        {
            WriteMsgs_.pop_front();
            if (!WriteMsgs_.empty()) {
                boost::asio::async_write(
                    Socket_,
                    boost::asio::buffer(WriteMsgs_.front().Data()),
                    boost::bind(&TChatClient::handle_write, this, boost::asio::placeholders::error));
            }
        } else {
            DoClose();
        }
    }

    void DoClose()
    {
        Socket_.close();
    }

private:
    boost::asio::io_service& IOService_;
    tcp::socket Socket_;
    TChatMessage ReadMsg_;
    TChatMessageQueue WriteMsgs_;
};

}