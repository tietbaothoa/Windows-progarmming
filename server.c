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
        ZeroMemory(buffer, sizeof(buffer)); // xoa tin nhan cu duoc luu trong buffer
        int receive = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (receive <= 0) { // neu ham recv fail dong ket noi
            printf("Client%d disconnected.\n", id + 1);
            closesocket(clientSocket);
            EnterCriticalSection(&cs);
            clients[id].active = 0;
            LeaveCriticalSection(&cs);
            break;
        }
        buffer[receive] = '\0';
        printf("Client%d: %s\n", id + 1, buffer); // in ra tin nhan cua client voi clientid
    }
    return 0;
}

DWORD WINAPI serverInputThread(LPVOID lpParam) {
    char input[1024];

    while (1) {
        fgets(input, sizeof(input), stdin); // nhap tin nhan tu server
        if (strncmp(input, "@client", 7) == 0) { // xac dinh server dang muon giao tiep voi client nao vd: @client1
            int id = atoi(&input[7]) - 1;
            char* msg = strchr(input, ' ');
            if (msg && id >= 0 && id < 10 && clients[id].active) { // check xem client co dang hoat dong va nam trong khoang 1 den 10 khong
                send(clients[id].socket, msg + 1, strlen(msg + 1), 0); // gui tin nhan cho client
            } else {
                printf("Invalid client ID or client not active.\n");
            }
        } else if (strncmp(input, "@all", 4) == 0) {
            char* msg = strchr(input, ' ');
            if (msg) {
                for (int i = 0; i < 10; i++) {
                    if (clients[i].active) {
                        send(clients[i].socket, msg + 1, strlen(msg + 1), 0); // gui tin nhan den tat ca client
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
    // khoi dong winsock
    WSAStartup(MAKEWORD(2, 2), &wsa);
    //tao socket su dung IPv4, TCP
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    //cau hinh dia chi server
    serverAddr.sin_family = AF_INET;    // su dung IPv4
    serverAddr.sin_port = htons(5000);    // lang nghe tren port 5000
    serverAddr.sin_addr.s_addr = INADDR_ANY;     // lang nghe tren tat ca cac dia chi ip cuc bo

    //gan socket voi dia chi da cau hinh
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Failed while bind\n");
        return 0;
    }
    // bat dau lang nghe
    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        printf("Failed while listen\n");
        return 0;
    }
    // dong bo truy cap
    InitializeCriticalSection(&cs);
    printf("Waiting for connection.\n");

    // tao thread de nhap input của server
    CreateThread(NULL, 0, serverInputThread, NULL, 0, NULL);
    
    while (1) {
        //chap nhan ket noi tu client
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

        //bat dau doan code can dong bo 
        EnterCriticalSection(&cs);
        int id = -1;
        for (int i = 0; i < 10; i++) {
            if (!clients[i].active) {
                id = i;
                clients[i].socket = clientSocket; // gan socket client
                clients[i].id = i;    // stt client
                clients[i].active = 1;    // xac dinh client dang hoat dong
                break;
            }
        }
        //ket thuc doan code can dong bo
        LeaveCriticalSection(&cs);
        
        // neu so client > 10
        if (id == -1) {
            printf("Too many clients.\n");
            closesocket(clientSocket);
        } else {
            printf("Client%d connected.\n", id + 1);
            // tao thread quan ly client
            CreateThread(NULL, 0, handleClient, &clients[id], 0, NULL);
        }
    }

    DeleteCriticalSection(&cs);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
