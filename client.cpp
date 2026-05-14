#include <iostream>
#include <thread>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "protocol.h"

#define PORT 12345
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"

/**
 * 从服务器接收消息
 * @param server_socket 服务器的套接字值
 */
void receiveMessages(int server_socket) {
    try {
        while (true) {
            std::vector<uint8_t> buffer(BUFFER_SIZE);
            ssize_t bytes_received = recv(server_socket, buffer.data(), buffer.size(), 0);

            if (bytes_received <= 0) {
                throw std::runtime_error("Server disconnected");
            }

            buffer.resize(bytes_received);
            Message msg = Message::deserializeMsg(buffer);

            std::cout << "[" << msg.get_username() << "]: " << msg.get_content() << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // 创建套接字
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // 配置服务器地址
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid server address");
        close(client_socket);
        return 1;
    }

    // 连接到服务器
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection to server failed");
        close(client_socket);
        return 1;
    }
    std::cout << "Connected to server" << std::endl;

    // 设置用户名
    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);

    // 发送加入消息
    Message join_msg(MSG_JOIN, username);
    std::vector<uint8_t> join_serialized = join_msg.serializeMsg();
    send(client_socket, join_serialized.data(), join_serialized.size(), 0);

    // 创建线程接收服务器消息
    std::thread(receiveMessages, client_socket).detach();

    // 发送消息到服务器
    while (true) {
        std::string input;
        std::getline(std::cin, input);

        if (input == "/quit") {
            break;
        }

        // 构造消息并发送
        Message msg(MSG_TEXT, username, input);

        std::vector<uint8_t> serialized = msg.serializeMsg();
        send(client_socket, serialized.data(), serialized.size(), 0);
    }

    // 发送离开消息
    Message leave_msg(MSG_LEAVE, username);
    std::vector<uint8_t> leave_serialized = leave_msg.serializeMsg();
    send(client_socket, leave_serialized.data(), leave_serialized.size(), 0);

    // 关闭客户端套接字
    close(client_socket);
    return 0;
}