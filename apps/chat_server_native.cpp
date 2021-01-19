#include "chat_server.h"

int main(int argc, const char* argv[]) {
    try {

        if (argc != 2) {
            std::cerr << "Usage: chat_server <port>\n";
            return 1;
        }
        int port = std::stoi(argv[1]);

        boost::asio::io_service ioService;

        tcp::endpoint endpoint(tcp::v4(), port);
        NChat::TChatServerPtr server(new NChat::TChatServer(&ioService, endpoint));

        ioService.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}