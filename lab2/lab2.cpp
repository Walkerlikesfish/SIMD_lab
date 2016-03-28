#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include<windows.h>

using namespace std;

double PCFreq = 0.0;
__int64 CounterStart = 0;

const int WIDTH = 1024;                      // define the size of image
const int HEIGHT = 1024;

char strInputFile[] = "test.raw";           // input file
char strOutputFile_max_c[] = "result_max_c.raw"; // output file
char strOutputFile_min_c[] = "result_min_c.raw";
char strOutputFile_diff_c[] = "result_diff_c.raw";//contour extraction

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

int main()
{
	FILE* pInput = NULL;
	FILE* pOutput_max_c = NULL;
	FILE* pOutput_min_c = NULL;
	FILE* pOutput_diff_c = NULL;
	
	unsigned char* buf_1 = new unsigned char[WIDTH*HEIGHT];
	unsigned char* buf_2 = new unsigned char[WIDTH*HEIGHT];
	unsigned char* buf_3 = new unsigned char[WIDTH*HEIGHT];
	unsigned char* buf_4 = new unsigned char[WIDTH*HEIGHT];
	
	
	memset(buf_1, 0, WIDTH*HEIGHT);
	memset(buf_2, 0, WIDTH*HEIGHT);
	memset(buf_3, 0, WIDTH*HEIGHT);
	memset(buf_4, 0, WIDTH*HEIGHT);

	double time = 0;
	
	errno_t errIn, errOut_max_c, errOut_min_c, errOut_asm,errOut_diff_c;
	errIn = fopen_s(&pInput, strInputFile, "rb");
	errOut_max_c = fopen_s(&pOutput_max_c, strOutputFile_max_c, "wb");
	errOut_min_c = fopen_s(&pOutput_min_c, strOutputFile_min_c, "wb");
	errOut_diff_c = fopen_s(&pOutput_diff_c, strOutputFile_diff_c, "wb");


	fread(buf_1, sizeof(unsigned char), WIDTH*HEIGHT, pInput);
	fclose(pInput);
	errIn = fopen_s(&pInput, strInputFile, "rb");
	fread(buf_2, sizeof(unsigned char), WIDTH*HEIGHT, pInput);

	int window;               //the size of window (3, 5, 7)

	printf("Please enter the size of window ( 3 , 5 , 7 ......) : ");
	scanf_s("%d", &window);
	while (window % 2==0 || window>1024)
	{
		printf("Error! Unavailable size.\n");
		printf("Please enter the size of window ( 3 , 5 , 7 ):");
		scanf_s("%d", &window);
	}
	printf("\n");

	unsigned char* buf_ex = new unsigned char[(WIDTH+window-1)*(HEIGHT+window-1)];

	StartCounter();
	/*-------------------------------------------------------------------------------------------*/
	//Max filter C version
	memset(buf_ex, 0, (WIDTH + window - 1)*(HEIGHT + window - 1));
	for (int i= 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			buf_ex[(j + (window / 2))*(WIDTH + window - 1) + (i + (window / 2))] = buf_1[j * 1024 + i];
		}
	}
	
	for (int i = 0; i < HEIGHT; i++ )
	{
		for (int j = 0; j < WIDTH; j++)
		{
			for (int k = 0; k < window*window; k++)
			{
				if (buf_1[i*1024+j] < buf_ex[(k / window+ i) * (WIDTH + window - 1) + k % window+ j])
					buf_1[i*1024+j] = buf_ex[(k / window + i) * (WIDTH + window - 1) + k % window + j];

			}
		}
	}

	fwrite(buf_1, sizeof(unsigned char), WIDTH*HEIGHT, pOutput_max_c);

	printf("For C version:\n");
	printf("\n");
	printf("Max filter has already be implemented.\n");
	printf("Result is saved in result_max_c.raw.\n");
	printf("\n");

	/*-------------------------------------------------------------------------------------------*/
	//Min Filter C version
	memset(buf_ex, 255, (WIDTH + window - 1)*(HEIGHT + window - 1));
		for (int i= 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			buf_ex[(j + (window / 2))*(WIDTH + window - 1) + (i + (window / 2))] = buf_2[j * 1024 + i];
		}
	}
	
	for (int i = 0; i < HEIGHT; i++ )
	{
		for (int j = 0; j < WIDTH; j++)
		{
			for (int k = 0; k < window*window; k++)
			{
				if (buf_2[i*1024+j] >buf_ex[(k / window+ i) * (WIDTH + window - 1) + k % window+ j])
					buf_2[i*1024+j] = buf_ex[(k / window + i) * (WIDTH + window - 1) + k % window + j];

			}
		}
	}

	fwrite(buf_2, sizeof(unsigned char), WIDTH*HEIGHT, pOutput_min_c);
	printf("Min filter has already be implemented.\n");
	printf("Result is saved in result_min_c.raw.\n");
	printf("\n");

	/*-------------------------------------------------------------------------------------------*/
	//contour extraction
	
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			buf_3[i * 1024 + j] = buf_1[i * 1024 + j] - buf_2[i * 1024 + j];
			//buf_3[i * 1024 + j] = 255 - buf_3[i * 1024 + j];
		}
	}

	fwrite(buf_3, sizeof(unsigned char), WIDTH*HEIGHT, pOutput_diff_c);
	printf("Contour extration figure has already be generated.\n");
	printf("Result is saved in result_diff_c.raw.\n");
	printf("\n");

	time = GetCounter();
	printf("runing time = %f ms\n", time);

	fclose(pInput);
	fclose(pOutput_max_c);
	fclose(pOutput_min_c);
	fclose(pOutput_diff_c);
	


	system("pause");
	return 0;
}