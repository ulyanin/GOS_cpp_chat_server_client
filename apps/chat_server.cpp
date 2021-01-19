#include "chat_server.h"
#include "lib.h"

#include <boost/thread.hpp>

using namespace NChat;

int main(int argc, const char* argv[]) {
    try {

        if (argc != 3) {
            std::cerr << "Usage: chat_server <login> <port>\n";
            return 1;
        }
        std::string login = argv[1];
        int port = std::stoi(argv[2]);

        boost::asio::io_service ioService;

        tcp::endpoint endpoint(tcp::v4(), port);
        NChat::TChatServerPtr server(new NChat::TChatServer(&ioService, endpoint));

        boost::thread thread(boost::bind(&boost::asio::io_service::run, &ioService));

        std::string line;
        while (std::getline(std::cin, line)) {
            if (StartsWith(line, "/shutdown")) {
                break;
            } else {
                NChat::TMessageProto msg;
                msg.set_content(line);
                msg.set_login(login);
                server->WriteMessage(msg);
            }
        }
        ioService.stop();
        server.reset();
        thread.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}