#include <chat_server/chat_client.h>

int main(int argc, const char* argv[])
{
    try {
        if (argc != 3) {
            std::cerr << "Usage: chat_client <host> <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(argv[1], argv[2]);
        tcp::resolver::iterator iterator = resolver.resolve(query);
        if (iterator == tcp::resolver::iterator()) {
            std::cerr << "Cannot resolve host=" << argv[1] << " port=" << argv[2] << std::endl;
            return 1;
        }

        NChat::TChatClient chatClient(io_service, iterator);

        boost::thread thread(boost::bind(&boost::asio::io_service::run, &io_service));

        std::string line;
        while (std::getline(std::cin, line))
        {
            NChat::TChatMessage msg = NChat::TChatMessage::FromString(line);
            chatClient.Write(msg);
        }

        chatClient.Close();
        thread.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}