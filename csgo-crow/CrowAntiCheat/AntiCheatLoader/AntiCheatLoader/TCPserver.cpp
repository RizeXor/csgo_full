#include "stdafx.h"
#include "unity.h"
#include <time.h>
void TCPSERVER::SentPack(std::string pack)
{
	if (G::ClientSocket) 
		send(G::ClientSocket,pack.c_str(), pack.length(),0);
}
bool TCPSERVER::CreateTCPserver()
{
	WSADATA wsaData;
	int port = 5099;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return false;
	}

	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(port); //1024���ϵĶ˿ں�  
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	int retVal = bind(sockSrv, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));
	if (retVal == SOCKET_ERROR) {
		return false;
	}

	if (listen(sockSrv, 10) == SOCKET_ERROR) {
		return false;
	}

	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);
	static bool first = false;
	G::ClientSocket = accept(sockSrv, (SOCKADDR *)&addrClient, &len);
	if (G::ClientSocket == SOCKET_ERROR) {
		return false;
	}
	
	while (true)
	{
		char recvBuf[100];
		memset(recvBuf, 0, sizeof(recvBuf));
		if (recv(G::ClientSocket, recvBuf, sizeof(recvBuf), 0) == 0 || G::ClientSocket == INVALID_SOCKET)
		{
			first = false;
			closesocket(G::ClientSocket);
			G::ClientSocket = accept(sockSrv, (SOCKADDR *)&addrClient, &len);
			continue;
		}
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
			send(G::ClientSocket, buf, 32, 0);
			continue;
		}
		if (!first)
		{
			//�ڶ�����Ϣ������У���뵱����0x01337�����Ͽ�
			std::string recvKey = T::GetMd5Hash(Check_number + T::GetMd5Hash(Password));
			if (IPC.compare(recvKey) == 0)
				first = true;
			else
				closesocket(G::ClientSocket),
				G::ClientSocket = accept(sockSrv, (SOCKADDR *)&addrClient, &len);
			continue;
		}

		//������bug,ע��
		if (IPCnumber.compare(XorString("0x01338")) == 0)
			myCallBack->OnClientLogin(IPC.erase(0, 7));
		if(IPCnumber.compare(XorString("0x01339")) == 0)
			myCallBack->OnConnectCSGOServer(IPC.erase(0, 7));
	}
	closesocket(sockSrv);
	WSACleanup();
	return false;
}