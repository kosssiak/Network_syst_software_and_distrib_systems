#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];

    // Создание сокета
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Ошибка создания сокета" << std::endl;
        return -1;
    }

    // Настройка адреса сервера
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) == -1) {
        std::cerr << "Ошибка преобразования адреса" << std::endl;
        close(client_socket);
        return -1;
    }

    // Подключение к серверу
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Ошибка подключения к серверу" << std::endl;
        close(client_socket);
        return -1;
    }

    std::cout << "Подключение к серверу прошло успешно" << std::endl;

    // Отправка и получение команд
    while (true) {
        std::cout << "Введите команду: ";
        std::string command;
        std::getline(std::cin, command);

        if (command == "EXIT" || command == "QUIT" || command == "CLOSE") {
            break;
        }

        // Отправка команды
        send(client_socket, command.c_str(), command.length(), 0);

        // Получение и вывод ответа
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            std::cerr << "Ошибка приема ответа" << std::endl;
            close(client_socket);
            return -1;
        }

        buffer[bytes_received] = '\0';
        std::cout << "Ответ от сервера: " << buffer << std::endl;
    }

    // Закрытие соединения
    close(client_socket);

    return 0;
}
