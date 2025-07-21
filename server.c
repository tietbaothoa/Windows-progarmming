#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

typedef struct {
    SOCKET socket;
    int id;
    int active;
} Client;

Client clients[10];
CRITICAL_SECTION cs;

DWORD WINAPI handleClient(LPVOID lpParam) {
    Client* client = (Client*)lpParam;
    SOCKET clientSocket = client->socket;
    int id = client->id;

    char welcome[128];
    sprintf(welcome, "You are client%d\n", id + 1);
    send(clientSocket, welcome, strlen(welcome), 0);

    char buffer[1024];
    while (1) {
        ZeroMemory(buffer, sizeof(buffer));
        int receive = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (receive <= 0) {
            printf("Client%d disconnected.\n", id + 1);
            closesocket(clientSocket);
            EnterCriticalSection(&cs);
            clients[id].active = 0;
            LeaveCriticalSection(&cs);
            break;
        }
        buffer[receive] = '\0';
        printf("Client%d: %s\n", id + 1, buffer);
    }
    return 0;
}

DWORD WINAPI serverInputThread(LPVOID lpParam) {
    char input[1024];

    while (1) {
        fgets(input, sizeof(input), stdin);

        if (strncmp(input, "@client", 7) == 0) {
            int id = atoi(&input[7]) - 1;
            char* msg = strchr(input, ' ');
            if (msg && id >= 0 && id < 10 && clients[id].active) {
                send(clients[id].socket, msg + 1, strlen(msg + 1), 0);
            } else {
                printf("Invalid client ID or client not active.\n");
            }
        } else if (strncmp(input, "@all", 4) == 0) {
            char* msg = strchr(input, ' ');
            if (msg) {
                for (int i = 0; i < 10; i++) {
                    if (clients[i].active) {
                        send(clients[i].socket, msg + 1, strlen(msg + 1), 0);
                    }
                }
            }

        } else {
            printf("Usage:\n  @client1 message\n  @all message\n");
        }
    }
    return 0;
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Failed while bind\n");
        return 0;
    }
    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        printf("Failed while listen\n");
        return 0;
    }

    InitializeCriticalSection(&cs);
    printf("Waiting for connection.\n");

    CreateThread(NULL, 0, serverInputThread, NULL, 0, NULL);

    while (1) {
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

        EnterCriticalSection(&cs);
        int id = -1;
        for (int i = 0; i < 10; i++) {
            if (!clients[i].active) {
                id = i;
                clients[i].socket = clientSocket;
                clients[i].id = i;
                clients[i].active = 1;
                break;
            }
        }
        LeaveCriticalSection(&cs);

        if (id == -1) {
            printf("Too many clients.\n");
            closesocket(clientSocket);
        } else {
            printf("Client%d connected.\n", id + 1);
            CreateThread(NULL, 0, handleClient, &clients[id], 0, NULL);
        }
    }

    DeleteCriticalSection(&cs);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
