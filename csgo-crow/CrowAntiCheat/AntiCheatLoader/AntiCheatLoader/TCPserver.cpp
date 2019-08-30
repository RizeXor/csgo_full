#include "stdafx.h"
#include "unity.h"
#include <time.h>
bool TCPSERVER::CreateTCPserver()
{
	WSADATA wsaData;
	int port = 5099;

//	char buf[] = "Server: hello, I am a server.....";

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
	//	printf("Failed to load Winsock");
		return false;
	}

	//�������ڼ������׽���  
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(port); //1024���ϵĶ˿ں�  
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	int retVal = bind(sockSrv, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));
	if (retVal == SOCKET_ERROR) {
	//	printf("Failed bind:%d\n", WSAGetLastError());
		return false;
	}

	if (listen(sockSrv, 10) == SOCKET_ERROR) {
	//	printf("Listen failed:%d", WSAGetLastError());
		return false;
	}

	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);
	SOCKET sockConn;
	static bool first = false;
	sockConn = accept(sockSrv, (SOCKADDR *)&addrClient, &len);
	if (sockConn == SOCKET_ERROR) {
		printf("Accept failed:%d", WSAGetLastError());
		return false;
	}
	
	while (true)
	{
		char recvBuf[100];
		memset(recvBuf, 0, sizeof(recvBuf));
		if (recv(sockConn, recvBuf, sizeof(recvBuf), 0) == 0 || sockConn == INVALID_SOCKET)
		{
			first = false;
			printf("Client Disconnect \n");
			closesocket(sockConn);
			sockConn = accept(sockSrv, (SOCKADDR *)&addrClient, &len);
			continue;
		}

		//printf("%s\n", recvBuf);
		std::string IPC = recvBuf;
		std::string IPCnumber = IPC.substr(0, 7);
		if (IPCnumber.compare(XorString("0x01337")) == 0)
		{
			//���Ƿ���md5(��ǰʱ��� + �����)
			//�Է�����md5(���ǵ���Ϣ + �ͻ�������һ���������У��)
			myCallBack->OnClientConnected(IPC.erase(0, 7));
			time_t myt = time(NULL);
			Check_number = std::to_string(myt) + XorString("Xor1337_TheDuck!");
			Check_number = T::GetMd5Hash(T::GetMd5Hash(Check_number));
			const char *buf = Check_number.c_str();
		//	printf("buf %s \n", buf);
			send(sockConn, buf, 32, 0);
			
			continue;
		}
		if (!first)
		{
			//�ڶ�����Ϣ������У���뵱����0x01337�����Ͽ�
			std::string recvKey = T::GetMd5Hash(Check_number + T::GetMd5Hash(Password));
		//	printf("recvKey %s \n", recvKey);
			if (IPC.compare(recvKey) == 0)
				first = true;
			else
				closesocket(sockConn),
				sockConn = accept(sockSrv, (SOCKADDR *)&addrClient, &len);
			continue;
		}

		//������bug,ע��
		if (IPCnumber.compare(XorString("0x01338")) == 0)
			myCallBack->OnClientLogin(IPC.erase(0, 7));
		if (IPCnumber.compare(XorString("0x01340")) == 0)
			myCallBack->OnBindSteamID(IPC.erase(0, 7));
		if(IPCnumber.compare(XorString("0x01339")) == 0)
			myCallBack->OnConnectCSGOServer(IPC.erase(0, 7));
	//	closesocket(sockConn);
	}

	closesocket(sockSrv);
	WSACleanup();
	return false;
}