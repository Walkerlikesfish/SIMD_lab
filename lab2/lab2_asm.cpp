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
char strOutputFile_max_asm[] = "result_max_asm.raw"; // output file
char strOutputFile_min_asm[] = "result_min_asm.raw";
char strOutputFile_diff[] = "result_diff.raw";//contour extraction

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
	FILE* pOutput_max_asm = NULL;
	FILE* pOutput_min_asm = NULL;
	FILE* pOutput_diff = NULL;

	unsigned char* buf_1 = new unsigned char[WIDTH*HEIGHT];
	unsigned char* buf_2 = new unsigned char[WIDTH*HEIGHT];
	unsigned char* buf_3 = new unsigned char[WIDTH*HEIGHT];

	memset(buf_1, 0, WIDTH*HEIGHT);// for max filter output
	memset(buf_2, 0, WIDTH*HEIGHT);// for min filter output
	memset(buf_3, 0, WIDTH*HEIGHT);// for diff output
	
	double time = 0;

	errno_t errIn, errOut_max_asm, errOut_min_asm, errOut_diff;
	errIn = fopen_s(&pInput, strInputFile, "rb");
	errOut_max_asm = fopen_s(&pOutput_max_asm, strOutputFile_max_asm, "wb");
	errOut_min_asm = fopen_s(&pOutput_min_asm, strOutputFile_min_asm, "wb");
	errOut_diff = fopen_s(&pOutput_diff, strOutputFile_diff, "wb");
	
	fread(buf_1, sizeof(unsigned char), WIDTH*HEIGHT, pInput);
	fclose(pInput);
	errIn = fopen_s(&pInput, strInputFile, "rb");
	fread(buf_2, sizeof(unsigned char), WIDTH*HEIGHT, pInput);


	int window;               //the size of window (3, 5, 7)

	printf("Please enter the size of window ( only 3, 5, 7 are available ) : ");
	scanf_s("%d", &window);
	while (window%2==0||window>8)
	{
		printf("Error! Unavailable size.\n");
		printf("Please enter the size of window ( only 3, 5, 7 are available ) :");
		scanf_s("%d", &window);
	}
	printf("\n");
	
	StartCounter();

	/*-----------------------------------------------------------------------------*/
	//Max  filter

	unsigned char* buf_ex = new unsigned char[(WIDTH + window - 1)*(HEIGHT + window - 1)];
	
	memset(buf_ex, 0, (WIDTH + window - 1)*(HEIGHT + window - 1));
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			buf_ex[(j + (window / 2))*(WIDTH + window - 1) + (i + (window / 2))] = buf_1[j * 1024 + i];
		}
	}
	unsigned char * ptrin = NULL;
	unsigned char * ptrout = NULL;
	int ii;


	if (window == 3)
	{
		for (int i = 0; i < HEIGHT; i++)
		{
			ii = 73;//ii*14=1022
			ptrin = buf_ex + (i*(WIDTH + window - 1));
			ptrout = buf_1 + (i*WIDTH);
			_asm
			{
			start1:
				mov    esi , ptrin;
				mov    ecx , ii;
				mov    edi , ptrout;
				
			l1:
				movdqu xmm0, [esi];  // l1st line
				movdqu xmm1, [esi + 1026];  // 2nd line
				movdqu xmm2, [esi + 2052];  // 3rd line
				pmaxub xmm0, xmm1;  // compare
				pmaxub xmm0, xmm2; 
				movdqu xmm6, xmm0;  // copy
				movdqu xmm7, xmm0;
				psrldq xmm6, 1;  // shift
				psrldq xmm7, 2;
				pmaxub xmm6, xmm7;  // colon max
				pmaxub xmm6, xmm0;
				movdqu[edi], xmm6;  // move result
				add  edi, 14;  // new result pointer
				add  esi, 14;  // new src pointer
				sub  ecx, 1; 
				jnz  l1;  // end

			}
		}
	}
	else if (window == 5)
	{
		for (int i = 0; i < HEIGHT; i++)
		{
			ii = 85;//ii*12=1020
			ptrin = buf_ex + (i*(WIDTH + window - 1));
			ptrout = buf_1 + (i*WIDTH);
			_asm
			{
			start2:
				mov    esi, ptrin;
				mov    ecx, ii;
				mov    edi, ptrout;

			l2:
				movdqu xmm0, [esi];  // l1st line
				movdqu xmm1, [esi + 1028];  // 2nd line
				movdqu xmm2, [esi + 2056];  // 3rd line
				movdqu xmm3, [esi + 3084];  // 4th line
				movdqu xmm4, [esi + 4112];  // 5th line

				pmaxub xmm0, xmm1;  // compare
				pmaxub xmm0, xmm2;
				pmaxub xmm0, xmm3;
				pmaxub xmm0, xmm4;
				movdqu xmm1, xmm0;  // copy
				movdqu xmm2, xmm0;
				movdqu xmm3, xmm0;
				movdqu xmm4, xmm0;
				psrldq xmm1, 1;  // shift
				psrldq xmm2, 2;
				psrldq xmm3, 3;
				psrldq xmm4, 4;
				pmaxub xmm0, xmm1; // colon max  
				pmaxub xmm0, xmm2;
				pmaxub xmm0, xmm3;
				pmaxub xmm0, xmm4;
				movdqu[edi], xmm0;  // move result
				add  edi, 12;  // new result pointer
				add  esi, 12;  // new src pointer
				sub  ecx, 1;
				jnz  l2;  // end

			}
		}
	}
	else if (window == 7)
	{
		for (int i = 0; i < HEIGHT; i++)
		{
			ii = 102;//ii*10=1020
			ptrin = buf_ex + (i*(WIDTH + window - 1));
			ptrout = buf_1 + (i*WIDTH);
			_asm
			{
			start3:
				mov    esi, ptrin;
				mov    ecx, ii;
				mov    edi, ptrout;

			l3:
				movdqu xmm0, [esi];  // l1st line
				movdqu xmm1, [esi + 1030];  // 2nd line
				movdqu xmm2, [esi + 2060];  // 3rd line
				movdqu xmm3, [esi + 3090];  // 4th line
				movdqu xmm4, [esi + 4120];  // 5th line
				movdqu xmm5, [esi + 5150];  // 6th line
				movdqu xmm6, [esi + 6180];  // 7th line


				pmaxub xmm0, xmm1;  // compare
				pmaxub xmm0, xmm2;
				pmaxub xmm0, xmm3;
				pmaxub xmm0, xmm4;
				pmaxub xmm0, xmm5;
				pmaxub xmm0, xmm6;
				movdqu xmm1, xmm0;  // copy
				movdqu xmm2, xmm0;
				movdqu xmm3, xmm0;
				movdqu xmm4, xmm0;
				movdqu xmm5, xmm0;
				movdqu xmm6, xmm0;
				psrldq xmm1, 1;  // shift
				psrldq xmm2, 2;
				psrldq xmm3, 3;
				psrldq xmm4, 4;
				psrldq xmm5, 5;
				psrldq xmm6, 6;
				pmaxub xmm0, xmm1; // colon max  
				pmaxub xmm0, xmm2;
				pmaxub xmm0, xmm3;
				pmaxub xmm0, xmm4;
				pmaxub xmm0, xmm5;
				pmaxub xmm0, xmm6;
				movdqu[edi], xmm0;  // move result
				add  edi, 10;  // new result pointer
				add  esi, 10;  // new src pointer
				sub  ecx, 1;
				jnz  l3;  // end

			}
		}
	}

	fwrite(buf_1, sizeof(unsigned char), WIDTH*HEIGHT, pOutput_max_asm);
	printf("For ASM version:\n");
	printf("\n");
	printf("Max filter has already be implemented.\n");
	printf("Result is saved in result_max_asm.raw.\n");
	printf("\n");

	/*-----------------------------------------------------------------------------*/
	//Min filter
	memset(buf_ex, 255, (WIDTH + window - 1)*(HEIGHT + window - 1));
	
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			buf_ex[(j + (window / 2))*(WIDTH + window - 1) + (i + (window / 2))] = buf_2[j * 1024 + i];
		}
	}

	if (window == 3)
	{
		for (int i = 0; i < HEIGHT; i++)
		{
			ii = 73;//ii*14=1022
			ptrin = buf_ex + (i*(WIDTH + window - 1));
			ptrout = buf_2 + (i*WIDTH);
			
			_asm
			{
			start4:
				mov    esi, ptrin;
				mov    ecx, ii;
				mov    edi, ptrout;

			l4:
				movdqu xmm0, [esi];  // l1st line
				movdqu xmm1, [esi + 1026];  // 2nd line
				movdqu xmm2, [esi + 2052];  // 3rd line
				pminub xmm0, xmm1;  // compare
				pminub xmm0, xmm2;
				movdqu xmm6, xmm0;  // copy
				movdqu xmm7, xmm0;
				psrldq xmm6, 1;  // shift
				psrldq xmm7, 2;
				pminub xmm6, xmm7;  // colon max
				pminub xmm6, xmm0;
				movdqu[edi], xmm6;  // move result
				add  edi, 14;  // new result pointer
				add  esi, 14;  // new src pointer
				sub  ecx, 1;
				jnz  l4;  // end

			}
		}
	}
	else if (window == 5)
	{
		for (int i = 0; i < HEIGHT; i++)
		{
			ii = 85;//ii*12=1020
			ptrin = buf_ex + (i*(WIDTH + window - 1));
			ptrout = buf_2 + (i*WIDTH);
			_asm
			{
			start5:
				mov    esi, ptrin;
				mov    ecx, ii;
				mov    edi, ptrout;

			l5:
				movdqu xmm0, [esi];  // l1st line
				movdqu xmm1, [esi + 1028];  // 2nd line
				movdqu xmm2, [esi + 2056];  // 3rd line
				movdqu xmm3, [esi + 3084];  // 4th line
				movdqu xmm4, [esi + 4112];  // 5th line

				pminub xmm0, xmm1;  // compare
				pminub xmm0, xmm2;
				pminub xmm0, xmm3;
				pminub xmm0, xmm4;
				movdqu xmm1, xmm0;  // copy
				movdqu xmm2, xmm0;
				movdqu xmm3, xmm0;
				movdqu xmm4, xmm0;
				psrldq xmm1, 1;  // shift
				psrldq xmm2, 2;
				psrldq xmm3, 3;
				psrldq xmm4, 4;
				pminub xmm0, xmm1; // colon max  
				pminub xmm0, xmm2;
				pminub xmm0, xmm3;
				pminub xmm0, xmm4;
				movdqu[edi], xmm0;  // move result
				add  edi, 12;  // new result pointer
				add  esi, 12;  // new src pointer
				sub  ecx, 1;
				jnz  l5;  // end

			}
		}
	}
	else//(window == 7)
	{
		for (int i = 0; i < HEIGHT; i++)
		{
			ii = 102;//ii*10=1020
			ptrin = buf_ex + (i*(WIDTH + window - 1));
			ptrout = buf_2 + (i*WIDTH);
			_asm
			{
			start6:
				mov    esi, ptrin;
				mov    ecx, ii;
				mov    edi, ptrout;

			l6:
				movdqu xmm0, [esi];  // l1st line
				movdqu xmm1, [esi + 1030];  // 2nd line
				movdqu xmm2, [esi + 2060];  // 3rd line
				movdqu xmm3, [esi + 3090];  // 4th line
				movdqu xmm4, [esi + 4120];  // 5th line
				movdqu xmm5, [esi + 5150];  // 6th line
				movdqu xmm6, [esi + 6180];  // 7th line


				pminub xmm0, xmm1;  // compare
				pminub xmm0, xmm2;
				pminub xmm0, xmm3;
				pminub xmm0, xmm4;
				pminub xmm0, xmm5;
				pminub xmm0, xmm6;
				movdqu xmm1, xmm0;  // copy
				movdqu xmm2, xmm0;
				movdqu xmm3, xmm0;
				movdqu xmm4, xmm0;
				movdqu xmm5, xmm0;
				movdqu xmm6, xmm0;
				psrldq xmm1, 1;  // shift
				psrldq xmm2, 2;
				psrldq xmm3, 3;
				psrldq xmm4, 4;
				psrldq xmm5, 5;
				psrldq xmm6, 6;
				pminub xmm0, xmm1; // colon max  
				pminub xmm0, xmm2;
				pminub xmm0, xmm3;
				pminub xmm0, xmm4;
				pminub xmm0, xmm5;
				pminub xmm0, xmm6;
				movdqu[edi], xmm0;  // move result
				add  edi, 10;  // new result pointer
				add  esi, 10;  // new src pointer
				sub  ecx, 1;
				jnz  l6;  // end

			}
		}
	}

	fwrite(buf_2, sizeof(unsigned char), WIDTH*HEIGHT, pOutput_min_asm);
	printf("Min filter has already be implemented.\n");
	printf("Result is saved in result_min_asm.raw.\n");
	printf("\n");


	/*-------------------------------------------------------------------------------------------*/
	//contour extraction

	ii = WIDTH*HEIGHT / 16;
	unsigned char * ptrin1 = buf_1;
	unsigned char * ptrin2 = buf_2;
	
	_asm
	{
		mov ecx, ii;
		mov edi, ptrin1;
		mov esi, ptrin2;
		
	l7:
		movapd xmm0, [edi];
		movapd xmm1, [esi];
		psubusb xmm0, xmm1;
		movapd[edi], xmm0;
		add edi, 16;
		add esi, 16;
		sub ecx, 1;
		jnz l7;
	}
	/*for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			buf_3[i * 1024 + j] = buf_1[i * 1024 + j] - buf_2[i * 1024 + j];
			buf_3[i * 1024 + j] = 255 - buf_3[i * 1024 + j];
		}
	}*/

	fwrite(buf_1, sizeof(unsigned char), WIDTH*HEIGHT, pOutput_diff);
	printf("Contour extration figure has already be generated.\n");
	printf("Result is saved in result_diff.raw.\n");
	printf("\n");

	time = GetCounter();
	printf("runing time = %f ms\n",time);

	fclose(pInput);
	fclose(pOutput_max_asm);
	fclose(pOutput_min_asm);
	fclose(pOutput_diff);
	
	system("pause");
	return 0;
}
