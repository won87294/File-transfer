#include <stdio.h>
#include <winsock2.h>
#include <errno.h>

#define BUFFER_SIZE	256

#pragma comment(lib, "ws2_32")

void errorHandling(char* error_message);

int main() {

	int port = 9999;
	char ip[] = "192.168.219.100";

	SOCKET server;
	SOCKADDR_IN server_addr = { 0 };
	WSADATA wsadata;

	char recv_message[30] = "";
	int recv_Len;

	char* target = "C:\\filetest\\testjpg.jpg";
	char buf[BUFFER_SIZE];
	FILE* file = NULL; // file stream
	size_t fsize, nsize = 0;

	if (WSAStartup(0x0202, &wsadata) != 0) { // wsadata init, ���� �� 0 ��ȯ
		errorHandling("WSAStartup error");
	}

	server = socket(PF_INET, SOCK_STREAM, 0);
	if (server == INVALID_SOCKET) {
		errorHandling("socket error");
	}

	// ������ ������ ip�ּ�, port��ȣ
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);

	// ���� ���� ��û
	if (connect(server, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		errorHandling("connect error");
	}

	// �����κ��� �޽��� ����
	recv_Len = recv(server, recv_message, sizeof(recv_message) - 1, 0);
	if (recv_Len == SOCKET_ERROR || recv_Len <= 0) {
		errorHandling("receive error");
	}
	printf("Message from server : %s \n", recv_message);

	fopen_s(&file, target, "rb");
	if (file == NULL) {
		printf("file transfer fail");
		return 1;
	}

	// file size
	fseek(file, 0, SEEK_END);
	fsize = ftell(file);

	fseek(file, 0, SEEK_SET);

	while (nsize != fsize) {
		// read from file to buf
		// 1byte * 256 count = 256byte => buf[256];
		int fpsize = fread(buf, 1, BUFFER_SIZE, file);
		nsize += fpsize;
		send(server, buf, fpsize, 0);
	}

	fclose(file);

	// socket close
	closesocket(server);

	// Windows socket ��ȯ
	WSACleanup();

	return 0;
}

void errorHandling(char* error_message) {
	fputs(error_message, stderr);
	fputc('\n', stderr);
	printf("%d \n", WSAGetLastError());
	exit(1);
}