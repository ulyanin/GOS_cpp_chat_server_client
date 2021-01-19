#include "chat_client.h"
#include "lib.h"
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

        boost::asio::io_service ioService;

        tcp::resolver resolver(ioService);
        tcp::resolver::query query(host, port);
        tcp::resolver::iterator iterator = resolver.resolve(query);
        if (iterator == tcp::resolver::iterator()) {
            std::cerr << "Cannot resolve host=" << host << " port=" << port << std::endl;
            return 1;
        }

        NChat::TChatClient chatClient(ioService, iterator);

        boost::thread thread(boost::bind(&boost::asio::io_service::run, &ioService));

        std::string line;

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        while (!chatClient.Stopped() && std::getline(std::cin, line)) {
            if (chatClient.Stopped()) {
                std::cerr << "client has been stopped or was not launched, break" << std::endl;
                break;
            }
            if (NChat::StartsWith(line, "/shutdown")) {
                break;
            }
            if (!line.empty()) {
                NChat::TMessageProto msg;
                msg.set_content(line);
                msg.set_login(login);
                chatClient.Write(msg);
            }
        }

        if (chatClient.ConnectionError()) {
            std::cerr << "Connection error, launch server and retry" << std::endl;
        }

        ioService.stop();
        chatClient.Close();
        thread.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}