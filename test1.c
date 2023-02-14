#include <stdio.h>
#include <winsock2.h>
#include <string.h>

#pragma comment(lib, "ws2_32")

#define LISTEN_QUEUE    5
#define DIRECTORY_SIZE	50
#define FILENAME_SIZE	30
#define BUFFER_SIZE	    256

void errorHandling(char* message);
SOCKET clntConnector(SOCKET servSock, SOCKADDR_IN serv_addr);
SOCKET socketListener(SOCKADDR_IN serv_addr);
struct sockaddr_in socketAddress(char* ip, int port);
void fileReceiver(SOCKET clntSock, char path[], char filename[]);

int main() {
    int port = 9999;
    char* ip = "192.168.219.100";

    SOCKADDR_IN serv_addr = socketAddress(ip, port);
    SOCKET servSock = socketListener(serv_addr);
    SOCKET clntSock = clntConnector(servSock, serv_addr);

    // 클라이언트로 메시지 송신
    char test[] = "accepted";
    send(clntSock, test, sizeof(test), 0);

    char filePath[DIRECTORY_SIZE] = "D:\\projects\\file\\";
    char fileName[FILENAME_SIZE] = "copy.txt";

    // 클라이언트로부터 파일 수신, 저장
    fileReceiver(clntSock, filePath, fileName);

    // socket close
    closesocket(servSock);
    closesocket(clntSock);

    // Windows socket 반환
    WSACleanup();

    return 0;
}

// error 처리
void errorHandling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    printf("error number : %d\n", WSAGetLastError()); // error code
    exit(1); // stop
}

// client와 데이터 송수신용 소켓
SOCKET clntConnector(SOCKET servSock, SOCKADDR_IN serv_addr) {

    SOCKET clntSock;
    SOCKADDR_IN clnt_addr = { 0 };
    int clnt_addr_size = sizeof(clnt_addr);

    // accept될 시 clntSock 생성, 클라이언트와 통신
    clntSock = accept(servSock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    printf("Client IP : %s\taccept \n", inet_ntoa(clnt_addr.sin_addr));

    return clntSock;

}

// client의 connect 요청 대기 소켓
SOCKET socketListener(SOCKADDR_IN serv_addr) {

    SOCKET servSock;

    // winsock 초기화
    WSADATA wsadata;
    if (WSAStartup(0x0202, &wsadata) != 0) { // wsadata init, 성공 시 0 반환
        errorHandling("WSAStartup error");
    }

    // servSock SOCKET 생성
    servSock = socket(PF_INET, SOCK_STREAM, 0);
    if (servSock == INVALID_SOCKET) { // IPv4, TCP protocol
        errorHandling("socket error");
    }

    // servSock 주소 bind
    if (bind(servSock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        errorHandling("bind error");
    }

    // 클라이언트 요청 대기
    if (listen(servSock, LISTEN_QUEUE) == SOCKET_ERROR) {
        errorHandling("listen error");
    }

    return servSock;

}

// socket address
struct sockaddr_in socketAddress(char* ip, int port) {
    SOCKADDR_IN addr = { 0 };

    // servSock SOCKET 주소
    addr.sin_family = AF_INET; // protocol
    addr.sin_addr.s_addr = inet_addr(ip); // ip address
    // addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port); // port

    return addr;
}

// file수신, 저장
void fileReceiver(SOCKET clntSock, char path[], char filename[]) {
    printf("\nSave directory %s\n", path);
    char* fileDir = strcat(path, filename);

    int nbyte = BUFFER_SIZE;
    char buf[BUFFER_SIZE];
    FILE* file = NULL;
    size_t filesize, bufsize = 0;

    fopen_s(&file, fileDir, "wb");

    // file copy
    while (nbyte != 0) {
        nbyte = recv(clntSock, buf, BUFFER_SIZE, 0);
        fwrite(buf, 1, nbyte, file);
    }
    printf("%s save complete \n", filename);

    fclose(file);
}
