// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>

HANDLE hSemSend;
HANDLE hSemRecv;
HANDLE hSemTermination;

CHAR lpSemSendName[] = "SendControl";
CHAR lpSemRecvName[] = "ResiveControl";
CHAR lpSemTerminationName[] = "ExitControl";
CHAR lpFileShareName[] = "TEST";

HANDLE hFileMapping;
LPVOID lpFileMap;

int main()
{
	CHAR str[80];
	DWORD dwRetCode;
	printf("Mapped and shared file, Sem sync, client process\n \n\nEnter  <Exit> to terminate...\n");
	hSemSend = OpenSemaphore(SEMAPHORE_MODIFY_STATE, FALSE, lpSemSendName);
	hSemRecv = OpenSemaphore(SYNCHRONIZE, FALSE, lpSemRecvName); 
	if (hSemSend == NULL || hSemRecv == NULL)
	{
		fprintf(stdout, "OpenSem: Error %ld\n",	GetLastError());
		_getch();
		return 0;
	}
	
	hSemTermination = OpenSemaphore(SEMAPHORE_MODIFY_STATE, FALSE, lpSemTerminationName);
	if (hSemTermination == NULL)
	{
		fprintf(stdout, "OpenSem (Termination): Error %ld\n", GetLastError());
		_getch();
		return 0;
	}
	
	hFileMapping = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, lpFileShareName);
	if (hFileMapping == NULL)
	{
		fprintf(stdout, "OpenFileMapping: Error %ld\n",	GetLastError());
		_getch();
		return 0;
	}

	lpFileMap = MapViewOfFile(hFileMapping,	FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (lpFileMap == 0)
	{
		fprintf(stdout, "MapViewOfFile: Error %ld\n", GetLastError());
		_getch();
		return 0;
	}
	//-------------------------------------------------------------------------------------
	while (TRUE)
	{
		
		fgets(str, 80, stdin);
		str[strlen(str) - 1] = '\0';
		if (!strcmp(str, "exit") || !strcmp(str, "Exit") || !strcmp(str, "EXIT"))
			break;
		strcpy((char*)lpFileMap, str);
		ReleaseSemaphore(hSemSend, 1, NULL);
		dwRetCode = WaitForSingleObject(hSemRecv, INFINITE);
		if (dwRetCode == WAIT_OBJECT_0) puts(((LPSTR)lpFileMap));
		if (dwRetCode == WAIT_ABANDONED || dwRetCode == WAIT_FAILED)
		{
			printf("\nError waiting response! Error:%ld\n", GetLastError());
			//break;
		}
	}
	//---------------------------------------------------------------------------------------------
	// ����� ���������� ����� ����������� ��������
	// � ���������� ��������� ��� ������ �������� ������ �
	// ��������� ��������  
	//SetSem(hSemSend);
	ReleaseSemaphore(hSemTermination, 1, NULL);
	CloseHandle(hSemSend);
	CloseHandle(hSemRecv);
	CloseHandle(hSemTermination);
	UnmapViewOfFile(lpFileMap);
	CloseHandle(hFileMapping);

	return 0;
}
