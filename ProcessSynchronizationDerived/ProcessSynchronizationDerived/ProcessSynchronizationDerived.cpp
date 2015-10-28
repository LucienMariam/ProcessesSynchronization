// ProcessSynchronizationDerived.cpp : main project file.

#include "stdafx.h"
#include <Vcclr.h>
#define JARENIY_PETUX_V_POPU_NE_KLYUNET true

using namespace System;

const int m_symbolOutputQuantity = 80;


char* ConvertStringToChar(String ^ourString)
{
	char *charArray;

	pin_ptr<const wchar_t> wch = PtrToStringChars(ourString);
	size_t convertedChars = 0;
	size_t  sizeInBytes = ((ourString->Length + 1) * 2);
	charArray = (char *)malloc(sizeInBytes);
	wcstombs_s(&convertedChars, charArray, sizeInBytes, wch, sizeInBytes);

	return charArray;
}


int main()
{
	array<System::String ^> ^argm = System::Environment::GetCommandLineArgs();
	HANDLE isMyTurn = OpenEvent(EVENT_ALL_ACCESS, FALSE, ConvertStringToChar(argm[2]));

	printf("\nCurrent processes number: %d\n", Convert::ToInt32(argm[1]) + 1);

	do
	{
		WaitForSingleObject(isMyTurn, INFINITE);	 
		Console::WriteLine("");

		for (int i = 0; i < m_symbolOutputQuantity; i++)   // print process output on the console
		{
			for (int j = 0; j < argm[1]->Length; j++)
			{
				printf("%c", argm[1][j]);
				i++;
			}

			printf(" ");	
		}
		
		Sleep(1000);
		SetEvent(isMyTurn);

	} while (JARENIY_PETUX_V_POPU_NE_KLYUNET);
}

