// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "unity.h"
/*
ɨ��vmt hook
����ֹ�ע��
*/
HookScaner* hkScanerEngine = new HookScaner;
MemoryScan* MMSCAN = new MemoryScan;
void AntiCheatThread()
{
	hkScanerEngine->InstallScanner();
	hkScanerEngine->CheckVMTHook();
	MMSCAN->StartScan();
}
void Cheat(HMODULE hDLL)
{
	DisableThreadLibraryCalls(hDLL);
	char bkjaa[] = { 'V','a','l','v','e','0','0','1','\0' };
	HWND Window;
	while (!(Window = FindWindowA(bkjaa, NULL))) Sleep(200);
	//Hooks::oldWindowProc = (WNDPROC)SetWindowLongPtr(G::Window, GWL_WNDPROC, (LONG_PTR)Hooks::WndProc);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)AntiCheatThread, NULL, NULL, NULL);
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Cheat(hModule);
		G::MOUDLE = (DWORD)hModule;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

