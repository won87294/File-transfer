#include <stdio.h>
#include <winsock2.h>
#include <string.h>

#pragma comment(lib, "ws2_32")

#define DIRECTORY_SIZE	50
#define FILENAME_SIZE	30
#define BUFFER_SIZE	256

void errorHandling(char* error_message);
SOCKET servConnector(SOCKADDR_IN sock_addr);
struct sockaddr_in socketAddress(char* ip, int port);
void fileSender(SOCKET sock, char path[], char file[]);

int main() {
	int port = 9999;
	char* ip = "192.168.219.100";

	SOCKADDR_IN sock_addr = socketAddress(ip, port);
	SOCKET sock = servConnector(sock_addr);

	char targetPath[DIRECTORY_SIZE] = "C:\\filetest\\";
	char targetFile[FILENAME_SIZE] = "";
	printf("file name > C:\\filetest\\");
	scanf("%s", targetFile);

	// 서버로 파일 송신
	fileSender(sock, targetPath, targetFile);

	// socket close
	closesocket(sock);

	// Windows socket 반환
	WSACleanup();

	return 0;
}

// error 처리
void errorHandling(char* error_message) {
	fputs(error_message, stderr);
	fputc('\n', stderr);
	printf("%d \n", WSAGetLastError());
	exit(1);
}

// server에 connect 요청
SOCKET servConnector(SOCKADDR_IN sock_addr) {
	SOCKET sock;

	char recv_message[10] = "";
	int recv_Len;

	WSADATA wsadata;
	if (WSAStartup(0x0202, &wsadata) != 0) { // wsadata init, 성공 시 0 반환
		errorHandling("WSAStartup error");
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		errorHandling("socket error");
	}

	// 서버 연결 요청
	if (connect(sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR) {
		errorHandling("connect error");
	}

	// 서버로부터 접속 승인 메시지 수신
	recv_Len = recv(sock, recv_message, sizeof(recv_message) - 1, 0);
	if (recv_Len <= 0) {
		errorHandling("receive error");
	}
	printf("connection %s \n", recv_message);

	return sock;
}

// socket address
struct sockaddr_in socketAddress(char* ip, int port) {
	SOCKADDR_IN addr = { 0 };

	// 접속할 서버의 ip주소, port번호
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);

	return addr;
}

// file 송신
void fileSender(SOCKET sock, char path[], char filename[]) {
	printf("\nfile directory %s\n", path);
	char* targetDir = strcat(path, filename);

	char buf[BUFFER_SIZE];
	FILE* file = NULL;
	size_t fsize, nsize = 0;

	fopen_s(&file, targetDir, "rb");
	if (file == NULL) {
		printf("file transfer fail");
		return 1;
	}

	// file size
	fseek(file, 0, SEEK_END);
	fsize = ftell(file);
	printf("file size : %d byte\n", (int*)fsize);
	printf("buf size : %d byte\n", BUFFER_SIZE);

	fseek(file, 0, SEEK_SET);

	while (nsize != fsize) {
		// 1byte * 256 count = 256byte => buf[256];
		int fpsize = fread(buf, 1, BUFFER_SIZE, file);
		nsize += fpsize;
		send(sock, buf, fpsize, 0);
	}
	printf("%s send complete \n", filename);

	fclose(file);
}
