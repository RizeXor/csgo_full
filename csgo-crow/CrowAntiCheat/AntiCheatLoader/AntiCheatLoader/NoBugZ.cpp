#include "stdafx.h"
#include "NoBugZ.h"
#include "unity.h"
void NoBugZ::ClientBugReport()
{
	printf(XorString("[SYSTEM]�������쳣...����ϵ����Ա \n"));
	closesocket(G::ClientSocket);
	system(XorString("pause"));
	exit(0);
}
