#include <iostream>
#include <string>

class Bot {
public:
    Bot(const std::string& serverAddress, int port);
    void connectToServer();
    void joinChannel(const std::string& channel);
    void listenForCommands();
    void handleCommand(const std::string& command);
    void sendFile(const std::string& receiver, const std::string& filePath);
    void receiveFile(const std::string& sender, const std::string& filePath);
};

