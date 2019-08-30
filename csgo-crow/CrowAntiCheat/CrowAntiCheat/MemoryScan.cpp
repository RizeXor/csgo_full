#include "stdafx.h"
#include "MemoryScan.h"

void MemoryScan::Install()
{
	//�ȱȶ�����ģ��list:
	HANDLE hShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	MODULEENTRY32 te = { sizeof(te) };
	BOOL bRet = Module32First(hShot, &te);
	entry[i].entry = (DWORD)GetModuleHandle(NULL);
	entry[i].endaddr = GetMoudleEnd(entry[i].entry);
	while (bRet) {
		i++;
		//T::PrintMessage("%ws \n",te.szModule);
		DWORD Moudle_entry = (DWORD)GetModuleHandleW(te.szModule);
		entry[i].entry = Moudle_entry;
		entry[i].endaddr = GetMoudleEnd(Moudle_entry);
		//T::PrintMessage("GetModuleHandleW <0x%08X> \n", Moudle_entry);
		bRet = Module32Next(hShot, &te);	
	}
	CloseHandle(hShot);
	

	G::MOUDLE_END = GetMoudleEnd(G::MOUDLE);
}
DWORD MemoryScan::GetMoudleEnd(DWORD start)
{
	MODULEINFO miModInfo;
	GetModuleInformation(GetCurrentProcess(), (HMODULE)start, &miModInfo, sizeof(MODULEINFO));
	return (start + miModInfo.SizeOfImage);
}
void MemoryScan::Anomaly_check(MEMORY_BASIC_INFORMATION memory_information)
{
	// REPORT ANY EXECUTABLE PAGE OUTSIDE OF KNOWN MODULES
	if (memory_information.Type == MEM_PRIVATE || memory_information.Type == MEM_MAPPED)
	{
		if (((DWORD)memory_information.BaseAddress & 0xFF0000000000) != 0x7F0000000000 && // UPPER EQUALS 0x7F
			((DWORD)memory_information.BaseAddress & 0xFFF000000000) != 0x7F000000000 &&  // UPPER EQUALS 0x7F0
			((DWORD)memory_information.BaseAddress & 0xFFFFF0000000) != 0x70000000 && // UPPER EQUALS 0x70000
			(DWORD)memory_information.BaseAddress != 0x3E0000)
		{
			//ban
		}
	}
}
void MemoryScan::StartScan()
{
	BOOL bRet = false;
	auto check_section = [&](LPCVOID address)
	{
		MEMORY_BASIC_INFORMATION mbi = { 0 };
		if (VirtualQuery(address, &mbi, sizeof(mbi)))
		{
			bool found = false;
			//mbi.Type == MEM_IMAGE ||
			if (mbi.AllocationProtect & PAGE_EXECUTE ||
				mbi.AllocationProtect & PAGE_EXECUTE_READ ||
				mbi.AllocationProtect & PAGE_EXECUTE_READWRITE ||
				mbi.AllocationProtect & PAGE_EXECUTE_WRITECOPY)
				found = true;
			return found;
		}
		return false;
	};
	auto check_isWhileShit = [&](LPCVOID address)
	{
		char byData[0x10] = { 0 };

		return false;
	};
	//1.ö��ȫ���̵߳õ�stcThreadInfo.th32ThreadID;
	HANDLE hSnapshort = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId());
	// ����߳��б������¼���̵߳���ϸ��Ϣ����ʹ��Thread32First��Thread32Next���������м�¼��ÿ���߳���Ϣ
	THREADENTRY32 stcThreadInfo;
	stcThreadInfo.dwSize = sizeof(stcThreadInfo);
	bRet = Thread32First(hSnapshort, &stcThreadInfo);
	bool bFoundTheShit = false;
	while (bRet)
	{
		if (GetCurrentProcessId() == stcThreadInfo.th32OwnerProcessID)
		{
			//T::PrintMessage("hThread ID <0x%d> \n", stcThreadInfo.th32ThreadID);
			//2.NtQueryInformationThread����ʼλ��
			PVOID  ThreadStartAddr = 0;
			HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION | THREAD_TERMINATE, FALSE, stcThreadInfo.th32ThreadID);
			NtQueryInformationThread(hThread, ThreadQuerySetWin32StartAddress, &ThreadStartAddr, sizeof(ThreadStartAddr), NULL);
		//	T::PrintMessage("hThread <0x%08X> hThread ID: %d \n", ThreadStartAddr, stcThreadInfo.th32ThreadID);
			if (ThreadStartAddr != 0)
			{
				bool bFoundManualMap = true;

				//�õ��̵߳�ַ��,����֪��ģ��ȶԲ鿴��С�Ƿ�����֪��ģ����.
				for (int z = 0; z < i; z++)
				{
					if (ThreadStartAddr >= (PVOID)entry[z].entry &&
						ThreadStartAddr <= (PVOID)entry[z].endaddr)
					{
						bFoundManualMap = false;
						continue;
					}
				}
				if (bFoundManualMap && check_section(ThreadStartAddr))
				{
					T::PrintMessage("Found ManualMap Dll <0x%08X> ThreadID: %d ThreadFromPID: %d \n", ThreadStartAddr, stcThreadInfo.th32ThreadID, stcThreadInfo.th32OwnerProcessID);
					bFoundTheShit = true;
				}
			}
			CloseHandle(hThread);
		}	
		bRet = Thread32Next(hSnapshort, &stcThreadInfo);
	}
	CloseHandle(hSnapshort);
	MEMORY_BASIC_INFORMATION memory_information = { 0 };
	BYTE szBuffer[MAX_PATH * 2 + 4] = { 0 };
	for (DWORD current_address = 0;
		VirtualQuery((PVOID)current_address, &memory_information, sizeof(memory_information)) >= 0;
		current_address = (DWORD)memory_information.BaseAddress + (DWORD)memory_information.RegionSize)
	{
		const auto outside_of_shellcode =
			memory_information.BaseAddress > (PVOID)G::MOUDLE || (DWORD)memory_information.BaseAddress + (DWORD)memory_information.RegionSize <= (DWORD)G::MOUDLE_END;

		const auto executable_memory =
			memory_information.State == MEM_COMMIT &&
			(memory_information.Protect == PAGE_EXECUTE ||
				memory_information.Protect == PAGE_EXECUTE_READ ||
				memory_information.Protect == PAGE_EXECUTE_READWRITE);
		const auto unknown_whitelist =
			memory_information.Protect != PAGE_EXECUTE_READWRITE ||
			memory_information.RegionSize != 100000000;
		if (!executable_memory || !outside_of_shellcode || !unknown_whitelist)
			continue;
		if(memory_information.Protect == PAGE_READONLY)
			continue;
		if (ZwQueryVirtualMemory(GetCurrentProcess(), (PVOID)current_address, MemorySectionName, szBuffer, sizeof(szBuffer), 0) < 0)
		{
			if(memory_information.RegionSize == 4096 ||
				memory_information.RegionSize == 8192 ||
				memory_information.RegionSize == 65536 ||
				memory_information.RegionSize == 294912 ||
				memory_information.RegionSize == 495616)
				continue;
			bool bFoundManualMap = true;
			for (int z = 0; z < i; z++)
			{
				if (current_address >= entry[z].entry &&
					current_address <= entry[z].endaddr)
				{
					bFoundManualMap = false;
					continue;
				}
			}
			if (bFoundManualMap)
			{
				T::PrintMessage("Found Unknown Moudle <0x%08X> size: %d \n", current_address, memory_information.RegionSize);
				bFoundTheShit = true;
			}
		}	
		Anomaly_check(memory_information);
	}
	/*
	//0x00040000ϵͳ������ַ
	DWORD dwStartAddr = 0x00040000; 
	do
	{
		dwStartAddr += 0x1000;
		for (int z = 0; z < i; z++)
		{
			if (dwStartAddr >= entry[z].entry && dwStartAddr < entry[z].endaddr)
			{
				dwStartAddr += entry[z].endaddr;
				continue;
			}
		}
		if (check_section((PVOID)dwStartAddr))
		{
			T::PrintMessage("Found Unknown Moudle <0x%08X> \n", dwStartAddr);
			bFoundTheShit = true;
			break;
		}
	} while (dwStartAddr < 0x80000000);*/
	if(!bFoundTheShit)
		T::PrintMessage("Not Find ManualMap Moudle \n");
	else
		T::PrintMessage("Found ManualMap Moudle \n");
}
