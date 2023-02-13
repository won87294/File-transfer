#include <stdio.h>
#include <winsock2.h>

#define BUFFER_SIZE	256

#define PACKET_SIZE 1024

#pragma comment(lib, "ws2_32")

// error 처리
void errorHandling(char* message);

int main() {

    int port = 9999;
    char ip[] = "192.168.219.100";

    SOCKET servSock, clntSock;
    SOCKADDR_IN serv_addr, clnt_addr = { 0 }; // socket 주소 구조체 초기화 (서버, 클라이언트)
    WSADATA wsadata;

    char test[] = "connection success";

    char buf[BUFFER_SIZE];
    int nbyte = BUFFER_SIZE;
    size_t filesize, bufsize = 0;
    FILE* file = NULL;

    // winsock 초기화
    if (WSAStartup(0x0202, &wsadata) != 0) { // wsadata init, 성공 시 0 반환
        errorHandling("WSAStartup error");
    }

    // servSock SOCKET 생성
    servSock = socket(PF_INET, SOCK_STREAM, 0);
    if (servSock == INVALID_SOCKET) { // IPv4, TCP protocol
        errorHandling("socket error");
    }

    // servSock SOCKET 주소
    serv_addr.sin_family = AF_INET; // protocol
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // ip address
    serv_addr.sin_port = htons(port); // port

    // servSock 주소 bind
    if (bind(servSock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        errorHandling("bind error");
    }

    // 클라이언트 요청 대기
    if (listen(servSock, 5) == SOCKET_ERROR) {
        errorHandling("listen error");
    }

    // accept될 시 clntSock 생성, 클라이언트와 통신
    int clnt_addr_size = sizeof(clnt_addr);
    clntSock = accept(servSock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    printf("connect complete");

    // 클라이언트로 메시지 송신
    send(clntSock, test, sizeof(test), 0);

    // 생성할 파일 위치, 이름
    fopen_s(&file, "D:\\projects\\file\\success.jpg", "wb");

    while (nbyte != 0) {
        nbyte = recv(clntSock, buf, BUFFER_SIZE, 0);
        fwrite(buf, sizeof(char), nbyte, file);
    }

    fclose(file);

    // socket close
    closesocket(servSock);
    closesocket(clntSock);

    // Windows socket 반환
    WSACleanup();

    return 0;
}

void errorHandling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    printf("error number : %d\n", WSAGetLastError()); // error code
    exit(1); // stop
}