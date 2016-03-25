#include "stdafx.h"
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <process.h>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
using namespace std;
//------------------------------------------------------------------

double PCFreq = 0.0;
__int64 CounterStart = 0;

void StartCounter()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		cout << "QueryPerformanceFrequency failed!\n";
	PCFreq = double(li.QuadPart) / 1000.0;
	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}

double GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - CounterStart) / PCFreq;
}

const int NUM = 8;
const int WIDTH = 1024;                      // define the size of image
const int HEIGHT = 1024 / NUM;                   //divide image into 2 subimages                      

char strInputFile[] = "test.raw";           // input file
char strOutputFile_1[] = "result_1.raw";    // output file
char strOutputFile_2[] = "result_2.raw";

unsigned char * ptr1;
unsigned char * ptr2;

int T1 = 0;
int T2 = 1;
int T3 = 2;
int T4 = 3;
int T5 = 4;
int T6 = 5;
int T7 = 6;
int T8 = 7;


HANDLE mut;

void lab1(void * pointer)         //Threshold
{
	int * num = (int*)pointer;

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (ptr1[i * 1024 + j + (*num) * 1024 * 1024 / NUM]>127)
			{
				ptr1[i * 1024 + j + (*num) * 1024 * 1024 / NUM] = 255;
			}
			else
			{
				ptr1[i * 1024 + j + (*num) * 1024 * 1024 / NUM] = 0;
			}
		}
	}
	printf("part %d threshold is DONE£¡£¡£¡\n", *num + 1);
}

void lab2(void * pointer)               //Max filter(3*3)
{
	int * num = (int*)pointer;

	unsigned char* buf_ex = new unsigned char[(WIDTH + 2)*(HEIGHT + 2)];
	memset(buf_ex, 0, (WIDTH + 2)*(HEIGHT + 2));
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			buf_ex[(j + 1)*(WIDTH + 2) + (i + 1)] = ptr2[j * 1024 + i + (*num) * 1024 * 1024 / NUM];
		}
	}

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			for (int k = 0; k < 9; k++)
			{
				if (ptr2[i * 1024 + j + (*num) * 1024 * 1024 / NUM] < buf_ex[(k / 3 + i) * (WIDTH + 2) + k % 3 + j])
					ptr2[i * 1024 + j + (*num) * 1024 * 1024 / NUM] = buf_ex[(k / 3 + i) * (WIDTH + 2) + k % 3 + j];
			}
		}
	}
	printf("part %d max filter is DONE£¡£¡£¡\n", *num + 1);
}


int main()
{
	FILE* pInput = NULL;
	FILE* pOutput_1 = NULL;
	FILE* pOutput_2 = NULL;
	unsigned char* buf_1 = new unsigned char[WIDTH*HEIGHT * NUM]; //for lab1
	unsigned char* buf_2 = new unsigned char[WIDTH*HEIGHT * NUM];//for lab2
	memset(buf_1, 0, WIDTH*HEIGHT * NUM);
	memset(buf_2, 0, WIDTH*HEIGHT * NUM);

	double time = 0;

	int num = 0;

	errno_t errIn, errOut_1, errOut_2;
	errIn = fopen_s(&pInput, strInputFile, "rb");
	errOut_1 = fopen_s(&pOutput_1, strOutputFile_1, "wb");
	errOut_2 = fopen_s(&pOutput_2, strOutputFile_2, "wb");
	fread(buf_1, sizeof(unsigned char), WIDTH*HEIGHT*NUM, pInput);
	for (int i = 0; i < WIDTH*HEIGHT * NUM; i++)
	{
		buf_2[i] = buf_1[i];
	}
	ptr1 = buf_1;
	ptr2 = buf_2;

	StartCounter();

	mut = CreateMutex(NULL, FALSE, NULL);

	/*for (int i = 0; i < NUM - 1; i++)
	{
	num = i;
	_beginthread(lab1, 0, &num);
	Sleep(10);
	}

	for (int i = 0; i < NUM - 1; i++)
	{
	num = i;
	_beginthread(lab2, 0, &num);
	Sleep(10);
	}*/

	//can use pthread_create and pthread_join to fix it!!


	/*_beginthread(lab1, 0, &T1);
	Sleep(100);
	_beginthread(lab1, 0, &T2);
	Sleep(100);
	_beginthread(lab1, 0, &T3);
	Sleep(10);
	_beginthread(lab1, 0, &T4);
	Sleep(10);
	_beginthread(lab1, 0, &T5);
	Sleep(10);
	_beginthread(lab1, 0, &T6);
	Sleep(10);
	_beginthread(lab1, 0, &T7);
	Sleep(10);
	_beginthread(lab1, 0, &T8);
	Sleep(10);*/

	/*_beginthread(lab2, 0, &T1);
	Sleep(100);
	_beginthread(lab2, 0, &T2);
	Sleep(100);
	_beginthread(lab2, 0, &T3);
	Sleep(10);
	_beginthread(lab2, 0, &T4);
	Sleep(10);
	_beginthread(lab2, 0, &T5);
	Sleep(10);
	_beginthread(lab2, 0, &T6);
	Sleep(10);
	_beginthread(lab2, 0, &T7);
	Sleep(10);
	_beginthread(lab2, 0, &T8);
	Sleep(10);*/

	std::thread t1(lab1, &T1);
	std::thread t2(lab1, &T2);
	std::thread t3(lab1, &T3);
	std::thread t4(lab1, &T4);
	std::thread t5(lab1, &T5);
	std::thread t6(lab1, &T6);
	std::thread t7(lab1, &T7);
	std::thread t8(lab1, &T8);

	std::thread t9(lab2, &T1);
	std::thread t10(lab2, &T2);
	std::thread t11(lab2, &T3);
	std::thread t12(lab2, &T4);
	std::thread t13(lab2, &T5);
	std::thread t14(lab2, &T6);
	std::thread t15(lab2, &T7);
	std::thread t16(lab2, &T8);

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	t7.join();
	t8.join();

	t9.join();
	t10.join();
	t11.join();
	t12.join();
	t13.join();
	t14.join();
	t15.join();
	t16.join();

	CloseHandle(mut);

	time = GetCounter();
	printf("runing time = %f ms\n", time);

	fwrite(buf_1, sizeof(unsigned char), WIDTH*HEIGHT * NUM, pOutput_1);
	fwrite(buf_2, sizeof(unsigned char), WIDTH*HEIGHT * NUM, pOutput_2);




	system("pause");

	return 0;
}