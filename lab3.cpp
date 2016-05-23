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
// предварительное объявление функций

void GenerateNumbers(int* sumAddress)
{
	
	for (size_t i = 0; i < NUMBER_COUNT; i++)
	{
		EnterCriticalSection(&cs);
		// помещаю в очередь случайное число 0-99
		numbers.push(rand()%100);
		cout << "Generated number " << numbers.back() << endl;
		// жду две секунды (имитация долгой работы)
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
				//прочитал очередной элемент из очереди
				int current = numbers.front();
				cout << "Processing element " << current << endl;
				//прибавил к переменной, накапливающей сумму
				*sumAddress += current;
				//удалил его из очереди
				numbers.pop();
				//жду две секунды (имитация долгой работы)
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

// Точка входа
int _tmain(int argc, _TCHAR* argv[])
{	
	int sum;
	// сброс генератора случайных чисел (при необходимости).
	//srand(200);
	/*GenerateNumbers();
	ProcessNumbers(&sum);*/
	InitializeCriticalSection(&cs);
	hThreads[0]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)GeneratorThread,NULL,0,0);
	hThreads[1]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ConsumerThread,NULL,0,0);
	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);
	return 0;
}


// Подсказка о том, как выглядит функция - тело потока
/*DWORD WINAPI ThreadBody(LPVOID context)
{
	return 0;
}*/