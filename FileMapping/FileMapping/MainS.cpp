#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <conio.h>

HANDLE hSemSend;
HANDLE hSemRecv;
HANDLE hSemTermination;
HANDLE hSems[2];

CHAR lpSemSendName[] = "SendControl";
CHAR lpSemRecvName[] = "ResiveControl";
CHAR lpSemTerminationName[] = "ExitControl";
CHAR lpFileShareName[] = "TEST";
HANDLE hFileMapping;

LPVOID lpFileMap;
LONG res;
LPLONG lpres = &res;

HINSTANCE hLib;
int(*Test)(char*, int);

int main()
{
	DWORD dwRetCode;
	CHAR str[80];
	char filename[80];
	DWORD  cbWritten;
	DWORD   total = 0;

	char message[80] = { 0 };
	printf("Mapped and shared file, semaphore sync, server process\n");
	
	hSemSend = CreateSemaphore(NULL, 0, 1, lpSemSendName);
	hSemRecv = CreateSemaphore(NULL, 0, 1, lpSemRecvName); 
	if (hSemSend == NULL || hSemRecv == NULL)
	{
		fprintf(stdout, "CreateSemaphore: Error %ld\n", GetLastError());
		_getch();
		return 0;
	}
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		printf("\nServerApplication already started\n Press any key to exit...");
		_getch();
		return 0;
	}
	
	hSemTermination = CreateSemaphore(NULL, 0, 1, lpSemTerminationName);
	if (hSemTermination == NULL)
	{
		fprintf(stdout, "CreateSemaphore (Termination): Error %ld\n", GetLastError());
		_getch();
		return 0;
	}
	
	hFileMapping = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, 100, lpFileShareName);
	if (hFileMapping == NULL)
	{
		fprintf(stdout, "CreateFileMapping: Error %ld\n", GetLastError());
		_getch();
		return 0;
	}
	lpFileMap = MapViewOfFile(hFileMapping,
		FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (lpFileMap == 0)
	{
		fprintf(stdout, "MapViewOfFile: Error %ld\n", GetLastError());
		_getch();
		return 0;
	}

	hSems[0] = hSemTermination;
	hSems[1] = hSemSend;

	hLib = LoadLibrary("lab2.dll");
	if (hLib == NULL) {
		ExitProcess(-3);
	}
	Test = (int(*)(char*, int))GetProcAddress(hLib, "CWF");
	if (Test == NULL) {
		ExitProcess(-4);
	}
	//--------------------------------------------------------------------
	while (TRUE)
	{
		total = 0;
		
		dwRetCode = WaitForMultipleObjects(2, hSems, FALSE, INFINITE);
		if (dwRetCode == WAIT_ABANDONED_0 || dwRetCode == WAIT_ABANDONED_0 + 1 || dwRetCode == WAIT_OBJECT_0 ||	dwRetCode == WAIT_FAILED )// || dwRetCode == WAIT_OBJECT_0 + 1)
			break;
		else
		{
			puts(((LPSTR)lpFileMap));
			strcpy(str, ((LPSTR)lpFileMap));
			int i = 0;
			if ((str[(strlen(str) - 1)] >= 48) && (str[(strlen(str) - 1)] <= 57)) {
				while (*(str + i) != ' ' && *(str + i) != '\0') {
					filename[i] = *(str + i);
					i++;
				}
				filename[i] = '\0';
				char* num = (str + i);
				int l = atoi(num);
				total = (*Test)(filename, l);
				str[strlen(str) - 2] = '\0';
				switch (total) {
				case -1: sprintf(message, "Cann't find file\n", str, total); break;
				default: sprintf(message, "Server: %s, changes = %d\n", str, total); break;
				}
				
				WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message), &cbWritten, NULL);
				//sprintf(message, "%d", (int)total);
				strcpy(((LPSTR)lpFileMap), message);
			}
			else {
				sprintf(message, "Can't open %s!, invalid comand", str);
				WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message) + 1, &cbWritten, NULL);
				printf("\n");
				strcpy(((LPSTR)lpFileMap), message);
			}

			ReleaseSemaphore(hSemRecv, 1, lpres);
		}
	}
    //-------------------------------------------------------------------------------------------------

	CloseHandle(hSemSend);
	CloseHandle(hSemRecv);
	CloseHandle(hSemTermination);
	UnmapViewOfFile(lpFileMap);
	CloseHandle(hFileMapping);

	return 0;
}