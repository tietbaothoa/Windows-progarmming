#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>

DWORD WINAPI recvThread(LPVOID lpParam) {
    SOCKET sockfd = *(SOCKET*)lpParam;
    char message_server[1024];

    while (1) {
        // xoa tin nhan cu
        memset(message_server, 0, sizeof(message_server));
        // nhan tin nhan tu server
        int read = recv(sockfd, message_server, sizeof(message_server), 0);
        // dong ket noi neu read failed
        if (read <= 0) {
            printf("Disconnected from server.\n");
            closesocket(sockfd);
            ExitThread(0);
        }
        printf("Server: %s", message_server);  
        fflush(stdout);
    }

    return 0;
}

int main() {
    WSADATA wsa;
    // khoi tao socket
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize WinSock\n");
        return 0;
    }

    struct sockaddr_in server_addr;
    // tao socket su dung IPv4 va TCP
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("FAILED WHILE set up on client");
        return 0;
    }
    // thiet lap dia chi server
    server_addr.sin_family = AF_INET; //su dung IPv4
    server_addr.sin_port = htons(5000); //lang nghe tren port 5000
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // lang nghe tren ip 
    // thiet lap ket noi den server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connection failed\n");
        return 0;
    }
    //tạo thread xu ly ham recvThread de nhan tin nhan ke ca khi client dang nhap tin nhan
    CreateThread(NULL, 0, recvThread, (LPVOID)&sockfd, 0, NULL);

    while (1) {
        char message_client[1024] = {0};
        // nhap tin nhan
        fgets(message_client, sizeof(message_client), stdin);
        message_client[strcspn(message_client, "\n")] = '\0';
        // gui tin nhan cho server
        int sent = send(sockfd, message_client, strlen(message_client), 0);
        if (sent <= 0) {
            printf("Failed while sending message.\n");
            break;
        }
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
