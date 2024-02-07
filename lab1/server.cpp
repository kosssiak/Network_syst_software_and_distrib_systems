#include <iostream>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

const char* handle_echo(const char* data) {
    if (data != nullptr) {
        return data;
    } else {
        return "Нет данных для ECHO";
    }
}

const char* handle_time() {
    time_t raw_time;
    struct tm* time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);

    return asctime(time_info);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_size = sizeof(client_address);
    char buffer[BUFFER_SIZE];

    // Создание сокета
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Ошибка создания сокета");
        return -1;
    }

    // Настройка адреса сервера
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Привязка сокета к адресу и порту
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Ошибка привязки сокета к адресу и порту");
        close(server_socket);
        return -1;
    }

    // Прослушивание порта
    if (listen(server_socket, 5) == -1) {
        perror("Ошибка прослушивания порта");
        close(server_socket);
        return -1;
    }

    std::cout << "Сервер запущен на порту " << PORT << std::endl;

    while (true) {
        // Принятие входящего соединения
        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_size);
        if (client_socket == -1) {
            perror("Ошибка приема соединения");
            continue;
        }

        char client_address_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_address.sin_addr), client_address_str, INET_ADDRSTRLEN);

        std::cout << "Подключен клиент: " << client_address_str << std::endl;

        while (true) {
            // Чтение данных от клиента
            ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0) {
                break;
            }

            buffer[bytes_received] = '\0';  // Добавляем null-терминатор

            char* command = strtok(buffer, " \r\n");
            char* params = strtok(NULL, "\r\n");

            const char* response;

            if (strcmp(command, "ECHO") == 0) {
                response = handle_echo(params);
            } else if (strcmp(command, "TIME") == 0) {
                response = handle_time();
            } else if (strcmp(command, "CLOSE") == 0 || strcmp(command, "EXIT") == 0 || strcmp(command, "QUIT") == 0) {
                break;
            } else {
                response = "Неверная команда";
            }

            // Отправка ответа клиенту
            send(client_socket, response, strlen(response), 0);
        }

        close(client_socket);
        std::cout << "Соединение с клиентом закрыто" << std::endl;
    }

    close(server_socket);

    return 0;
}