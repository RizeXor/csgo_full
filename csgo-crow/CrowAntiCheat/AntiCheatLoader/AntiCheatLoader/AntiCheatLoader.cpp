// AntiCheatLoader.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "unity.h"

int main()
{
	printf("                 $$$$$$                        \n");
	printf("                $$$$(a)$$::7                   \n");
	printf("                $$$$$$$$:/                     \n");
	printf("              $$$$$$$$,                        \n");
	printf("            ��??`?? ?`???$$,                    \n");
	printf("          ��??`??`????$$$$$$,                   \n");
	printf("          ��???????$$$$$$$$$$s                  \n");
	printf("          $?`??`???$$$$$$$$$ $$$,              \n");
	printf("          ��?????????$$$$$$$$$$$$s,,,,          \n");
	printf("           `j��???`????`?$$$$$$$$$$$$$$s,,      \n");
	printf("             ``��??? ??????$$$$$$    $$$$$$s,,  \n");
	printf("                ``r��?????  $$$$        $$$$$$  \n");
	printf("                 //   //       $$              \n");
	printf("                //-   //                       \n");
	printf(" �����������������������������������\n");
	printf(XorString("[SYSTEM]������������... \n"));
	ClientEngine->InitAntiCheat();
	printf(XorString("[SYSTEM]��ʼ�����! \n"));
	printf("%s", SigScaner->GetSig("H:\\5ewin\\CSGO_PRO\\5eר��ע����\\ConsoleApplication1\\Release\\ConsoleApplication1.exe", "0x1868390020|4967|0x487237497|3137|0x1548597252|2813|").c_str());
	ServerEngine->CreateTCPserver();
    return 0;
}
