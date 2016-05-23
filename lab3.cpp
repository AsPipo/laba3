// lab3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <queue>
#include <iostream>
#include <random>
#include <Windows.h>

CRITICAL_SECTION cs;
using namespace std;
HANDLE hThreads[2];

queue<int> numbers;
const int NUMBER_COUNT = 10;
volatile bool GeneratorCompleted = FALSE;
// ��������������� ���������� �������

void GenerateNumbers(int* sumAddress)
{
	
	for (size_t i = 0; i < NUMBER_COUNT; i++)
	{
		EnterCriticalSection(&cs);
		// ������� � ������� ��������� ����� 0-99
		numbers.push(rand()%100);
		cout << "Generated number " << numbers.back() << endl;
		// ��� ��� ������� (�������� ������ ������)
		LeaveCriticalSection(&cs);
		Sleep(500);
	}
	GeneratorCompleted = TRUE;
}

void ProcessNumbers(int* sumAddress)
{
	while (!GeneratorCompleted)
	{ 
		int batchSize = 3;
		for (int i = 0; i < batchSize; i++)
		{
			EnterCriticalSection(&cs);
			if (numbers.size() != 0)
			{
				//�������� ��������� ������� �� �������
				int current = numbers.front();
				cout << "Processing element " << current << endl;
				//�������� � ����������, ������������� �����
				*sumAddress += current;
				//������ ��� �� �������
				numbers.pop();
				//��� ��� ������� (�������� ������ ������)
				Sleep(500);
			}
			LeaveCriticalSection(&cs);
		}
		
	}
		
}

DWORD WINAPI GeneratorThread(LPVOID context)
{
	int sum;
	GenerateNumbers(&sum);
	return 0;
}

DWORD WINAPI ConsumerThread(LPVOID context)
{
	int sum;
	ProcessNumbers(&sum);
	return 0;
}

// ����� �����
int _tmain(int argc, _TCHAR* argv[])
{	
	int sum;
	// ����� ���������� ��������� ����� (��� �������������).
	//srand(200);
	/*GenerateNumbers();
	ProcessNumbers(&sum);*/
	InitializeCriticalSection(&cs);
	hThreads[0]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)GeneratorThread,NULL,0,0);
	hThreads[1]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ConsumerThread,NULL,0,0);
	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);
	return 0;
}


// ��������� � ���, ��� �������� ������� - ���� ������
/*DWORD WINAPI ThreadBody(LPVOID context)
{
	return 0;
}*/