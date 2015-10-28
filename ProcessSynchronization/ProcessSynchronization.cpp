// ProcessSynchronization.cpp : main project file.

#include "stdafx.h"

using namespace System;
using std::vector;

const char *m_programName = { "ProcessSyncronizationDerived.exe" }; // name of derived process program name
vector<HANDLE> m_eventsArray;


char* ConvertStringToChar(String ^ourString) // use to match "System::String ^" and "char *" types
{
	char *charArray;

	pin_ptr<const wchar_t> wch = PtrToStringChars(ourString);
	size_t convertedChars = 0;
	size_t  sizeInBytes = ((ourString->Length + 1) * 2);
	charArray = (char *)malloc(sizeInBytes);
	wcstombs_s(&convertedChars, charArray, sizeInBytes, wch, sizeInBytes);

	return charArray;
}


DWORD NormalizeProcessesOutput(LPVOID parameter)  // required for derived processes synchronization
{
	HANDLE m_Event = OpenEvent(EVENT_ALL_ACCESS, FALSE, "m_Event");

	while (!WaitForSingleObject(m_Event, 0))
	{
		for (int i = 0; i < m_eventsArray.size(); i++)
		{
			if (WaitForSingleObject(m_Event, 0))
				return 0;

			SetEvent(m_eventsArray[i]);
			WaitForSingleObject(m_eventsArray[i], INFINITE);
		}
	}

	return 0;
}


int main(array<System::String ^> ^args)
{
	SetConsoleTitle("ROCESSES SYNCHRONIZATION");
    Console::WriteLine("Press <+> to generate new string\nPress <-> to remove latest string\nPress <q> to quit the program\n");
	Console::SetWindowSize(80, 57); 

	char m_procNumber = 0;
	String ^m_addition, ^m_eventName;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	vector<HANDLE> m_piHandles;
	HANDLE isWorkedOff;
	DWORD m_threadID;
	HANDLE m_threadHANDLE = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)NormalizeProcessesOutput, NULL, 0, &m_threadID);
	HANDLE m_Event = CreateEvent(NULL, TRUE, TRUE, "m_Event");

	do
	{
		switch (getch())
		{
		case '+':
			m_eventName = String::Format("{0}{1}", "isWorkedOffProc", m_procNumber);
			isWorkedOff = CreateEvent(NULL, FALSE, FALSE, ConvertStringToChar(m_eventName));
			m_eventsArray.push_back(isWorkedOff);

			ZeroMemory(&si, sizeof(si));
			ZeroMemory(&pi, sizeof(pi));
			si.cb = sizeof(si);

			m_addition = String::Format("{0} {1} {2}", Convert::ToString(m_programName), m_procNumber, m_eventName);

			CreateProcess("C:\\Users\\SHUSTRICK\\Documents\\Visual Studio 2013\\Projects\\ProcessSynchronizationDerived\\Debug\\ProcessSynchronizationDerived.exe", // change directory in a appropriate way
				           ConvertStringToChar(m_addition), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

			m_piHandles.push_back(pi.hProcess);
			m_procNumber++;
			break;

		case '-':
			if (m_piHandles.empty())
				break;

			if (!m_eventsArray.empty())
			{
				ResetEvent(m_Event);
				WaitForSingleObject(m_threadHANDLE, INFINITE);	
				TerminateProcess(m_piHandles.back(), 0);
				CloseHandle(m_eventsArray.back());
				CloseHandle(m_piHandles.back());		
				
				m_eventsArray.pop_back();
			}

			m_piHandles.pop_back(); 
			m_procNumber--;
			printf("\nCurrent processes number: %d\n", m_procNumber);
			SetEvent(m_Event);
			m_threadHANDLE = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)NormalizeProcessesOutput, NULL, 0, &m_threadID);
			break;

		case 'q':
			Console::WriteLine("END ALL PROCESSES");
			Console::WriteLine("\n");

			ResetEvent(m_Event);
			WaitForSingleObject(m_threadHANDLE, INFINITE);

			while (!m_piHandles.empty())
			{
				TerminateProcess(m_piHandles.back(), 0);
				m_piHandles.pop_back();
			}
	
			while (!m_eventsArray.empty())
			{
				CloseHandle(m_eventsArray.back());
				m_eventsArray.pop_back();
			}

			CloseHandle(m_Event);
			return 0;

		default:
			break;
		}

	} while (1);
}
