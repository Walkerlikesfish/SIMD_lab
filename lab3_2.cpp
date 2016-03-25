
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <process.h>

const int NUM = 2;
const int WIDTH = 1024;                      // define the size of image
const int HEIGHT = 1024/NUM;                   //divide image into 2 subimages                      

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
	int * num = (int*) pointer;

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (ptr1[i * 1024 + j+(*num)*1024*1024/NUM]>127)
			{
				ptr1[i * 1024 + j+ (*num) * 1024*1024/NUM] = 255;
			}
			else
			{
				ptr1[i * 1024 + j+ (*num) * 1024 * 1024 / NUM] = 0;
			}
		}
	}
	printf("DONE£¡£¡£¡\n");
}

void lab2( void * pointer )               //Max filter(3*3)
{
	int * num = (int*)pointer;

	unsigned char* buf_ex = new unsigned char[(WIDTH + 2)*(HEIGHT + 2)];
	memset(buf_ex, 0, (WIDTH + 2)*(HEIGHT+ 2));
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			 buf_ex[(j + 1)*(WIDTH +2) + (i + 1)] = ptr2[j * 1024 + i + (*num) * 1024 * 1024 / NUM];
		}
	}

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			for (int k = 0; k < 9; k++)
			{
				if (ptr2[i * 1024 + j + (*num) * 1024 * 1024 / NUM] < buf_ex[(k / 3 + i) * (WIDTH +2) + k % 3 + j])
					ptr2[i * 1024 + j + (*num) * 1024 * 1024 / NUM] = buf_ex[(k / 3 + i) * (WIDTH +2) + k % 3 + j];
			}
		}
	}
	printf("DONE£¡£¡£¡\n");
}


int main()
{
	FILE* pInput = NULL;
	FILE* pOutput_1 = NULL;
	FILE* pOutput_2 = NULL;
	unsigned char* buf_1 = new unsigned char[WIDTH*HEIGHT * NUM]; //for lab1
	unsigned char* buf_2 = new unsigned char[WIDTH*HEIGHT * NUM];//for lab2
	memset(buf_1, 0, WIDTH*HEIGHT * NUM );
	memset(buf_2, 0, WIDTH*HEIGHT * NUM );

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

	_beginthread(lab1, 0, &T1);
	Sleep(100);
	_beginthread(lab1, 0, &T2);
	Sleep(100);
	/*_beginthread(lab1, 0, &T3);
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

	_beginthread(lab2, 0, &T1);
	Sleep(100);
	_beginthread(lab2, 0, &T2);
	Sleep(100);
	/*_beginthread(lab2, 0, &T3);
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

	CloseHandle(mut);
	
	fwrite(buf_1, sizeof(unsigned char), WIDTH*HEIGHT * NUM, pOutput_1);
	fwrite(buf_2, sizeof(unsigned char), WIDTH*HEIGHT * NUM, pOutput_2);
	printf("write!\n");

	

	system("pause");

	return 0;
}

