#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#pragma warning (disable : 4996)

#define REMOTEIP   "255.255.255.255" //브로드 케스트 주소로 예약되어 있음
#define REMOTEPORT 9000
#define BUFSIZE    512
#define SURVEY "다음 중 2020년 최고의 선수는?\n\n 1.박용택\n2.오지환\n3.김현수\n4.이형종\n5. 이천용\n"

struct WINNER {
	char name[20];
	int point=0;
};
// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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

	strcpy(winner[0].name, "박용택");
	strcpy(winner[1].name, "오지환");
	strcpy(winner[2].name, "김현수");
	strcpy(winner[3].name, "이형종");
	strcpy(winner[4].name, "이천용");
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// 브로드캐스팅 활성화
	BOOL bEnable = TRUE; // 이 옵션을 켜겠다
	retval = setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
		(char*)& bEnable, sizeof(bEnable)); // 옵션 값과 그 사이즈
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// 소켓 주소 구조체 초기화
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr(REMOTEIP);
	remoteaddr.sin_port = htons(REMOTEPORT); // 각 호스트의 9000번 포트 번호로 한 번에 보내겠다

	// 데이터 통신에 사용할 변수
	int addrlen;
	char buf[BUFSIZE + 1];
	int len;

	// 브로드캐스트 데이터 보내기
	while (1) 
	{
		// 데이터 입력
		strcpy(buf, SURVEY);

		// 데이터 보내기
		retval = sendto(sock, buf, strlen(buf), 0,
			(SOCKADDR*)& remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			continue;
		}
		printf("[UDP] %d바이트를 보냈습니다.\n", retval);

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
			printf("현재 투표수 : %d\n",count);
		}

		printf("설문이 완료되었습니다\n");
		for (int i = 0; i < 5; i++)
		{
			printf("%s : %d\n", winner[i].name, winner[i].point);
		}
		break;
	}


	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}