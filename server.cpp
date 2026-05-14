#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <set>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include "protocol.h"

std::set<int> clients;
std::mutex clients_mutex;

#define PORT 12345
#define BUFFER_SIZE 1024
//这次我换一个文件改
/**
 * 将消息广播给所有客户端
 * @param msg 需要广播的消息
 */
void broadcastMessage(const Message &msg) {
    std::vector<uint8_t> serialized = msg.serializeMsg();

    std::lock_guard<std::mutex> lock(clients_mutex);
    for (int client_socket : clients) {
        send(client_socket, serialized.data(), serialized.size(), 0);
    }
}

/**
 * 处理客户端连接的线程函数
 * @param client_socket 客户端的套接字
 */
void handleClient(int client_socket) {
    try {
        while (true) {
            std::vector<uint8_t> buffer(BUFFER_SIZE);
            ssize_t bytes_received = recv(client_socket, buffer.data(), buffer.size(), 0);

            if (bytes_received <= 0) {
                throw std::runtime_error("Client disconnected");
            }

            buffer.resize(bytes_received);
            Message msg = Message::deserializeMsg(buffer);

            std::cout << "[Client " << client_socket << "]: " << msg.get_content() << std::endl;
            
            broadcastMessage(msg);
        }
    } catch (const std::exception &e) {
        std::cerr << "Error:" << e.what() << std::endl;
    }

    // 客户端断开连接时清理资源
    close(client_socket);
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.erase(client_socket);
    }
    std::cout << "Client disconnected: " << client_socket << std::endl;
}

int main() {
    int server_socket;
    struct sockaddr_in server_addr;
    
    // 创建套接字
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt failed");
        close(server_socket);
        return 1;
    }

    // 配置服务器地址
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 绑定套接字
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        return 1;
    }

    // 开始监听
    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        close(server_socket);
        return 1;
    }
    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }

        std::cout << "New client connected: " << client_socket << std::endl;

        // 将客户端添加到集合
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.insert(client_socket);
        }

        // 创建线程，对客户端进行处理
        std::thread(handleClient, client_socket).detach();
    }

    close(server_socket);
    return 0;
}
