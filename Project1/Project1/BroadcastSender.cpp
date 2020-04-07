#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#pragma warning (disable : 4996)

#define REMOTEIP   "255.255.255.255" //��ε� �ɽ�Ʈ �ּҷ� ����Ǿ� ����
#define REMOTEPORT 9000
#define BUFSIZE    512
#define SURVEY "���� �� 2020�� �ְ��� ������?\n\n 1.�ڿ���\n2.����ȯ\n3.������\n4.������\n5. ��õ��\n"

struct WINNER {
	char name[20];
	int point=0;
};
// ���� �Լ� ���� ��� �� ����
void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int main(int argc, char* argv[])
{
	int retval;
	struct WINNER winner[5];

	strcpy(winner[0].name, "�ڿ���");
	strcpy(winner[1].name, "����ȯ");
	strcpy(winner[2].name, "������");
	strcpy(winner[3].name, "������");
	strcpy(winner[4].name, "��õ��");
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// ��ε�ĳ���� Ȱ��ȭ
	BOOL bEnable = TRUE; // �� �ɼ��� �Ѱڴ�
	retval = setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
		(char*)& bEnable, sizeof(bEnable)); // �ɼ� ���� �� ������
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// ���� �ּ� ����ü �ʱ�ȭ
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr(REMOTEIP);
	remoteaddr.sin_port = htons(REMOTEPORT); // �� ȣ��Ʈ�� 9000�� ��Ʈ ��ȣ�� �� ���� �����ڴ�

	// ������ ��ſ� ����� ����
	int addrlen;
	char buf[BUFSIZE + 1];
	int len;

	// ��ε�ĳ��Ʈ ������ ������
	while (1) 
	{
		// ������ �Է�
		strcpy(buf, SURVEY);

		// ������ ������
		retval = sendto(sock, buf, strlen(buf), 0,
			(SOCKADDR*)& remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			continue;
		}
		printf("[UDP] %d����Ʈ�� ���½��ϴ�.\n", retval);

		int count = 0;
		while (count < 5)
		{
			addrlen = sizeof(remoteaddr);
			retval = recvfrom(sock,(char*)&len, sizeof(int), 0,
				(SOCKADDR*)& remoteaddr, &addrlen);
			if (retval == SOCKET_ERROR) {
				err_display("recvfrom()");
				continue;
			}

			winner[len - 1].point++;
			count++;
			printf("���� ��ǥ�� : %d\n",count);
		}

		printf("������ �Ϸ�Ǿ����ϴ�\n");
		for (int i = 0; i < 5; i++)
		{
			printf("%s : %d\n", winner[i].name, winner[i].point);
		}
		break;
	}


	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}