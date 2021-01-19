#include "chat_client.h"

#include "message.pb.h"

int main(int argc, const char* argv[])
{
    try {
        if (argc != 4) {
            std::cerr << "Usage: chat_client <login> <server_host> <server_port>\n";
            return 1;
        }
        std::string login(argv[1]);
        std::string host(argv[2]);
        std::string port(argv[3]);

        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(host, port);
        tcp::resolver::iterator iterator = resolver.resolve(query);
        if (iterator == tcp::resolver::iterator()) {
            std::cerr << "Cannot resolve host=" << host << " port=" << port << std::endl;
            return 1;
        }

        NChat::TChatClient chatClient(io_service, iterator);

        boost::thread thread(boost::bind(&boost::asio::io_service::run, &io_service));

        std::string line;
        while (std::getline(std::cin, line)) {
            NChat::TMessageProto msg;
            msg.set_content(line);
            msg.set_login(login);
            chatClient.Write(msg);
        }

        chatClient.Close();
        thread.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}