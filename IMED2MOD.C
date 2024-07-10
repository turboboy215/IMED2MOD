/*Climax/Images Software (IMEDGBoy GB/GBC module, IMEDGear GG/SMS module) to MOD converter*/
/*By Will Trowbridge*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define bankSize 16384

FILE* imed, *mod;
long offset;
long headerOffset;
int i, j;
char outfile[1000000];
long patList;
long patDir;
long chan12Data;
long chan3Data;
long waveTable;
long chan4Data;
int numInst;
int systemID;
unsigned static char* imedData;
unsigned static char* modData;
long imedLength;
long modLength;

char* tempPnt;
char OutFileBase[0x100];

/*Header IDs*/
const char gbID[8] = { 'I', 'M', 'E', 'D', 'G', 'B', 'o', 'y'};
const char ggID[8] = { 'I', 'M', 'E', 'D', 'G', 'e', 'a', 'r' };

/*Note frequency table*/
int noteVals[60] = { 856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480, 453,
428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226,
214, 202, 190, 180, 170, 160, 151, 143, 135, 127, 120, 113,
107, 101,  95,  90,  85,  80,  76,  71,  67,  64,  60,  57,
53, 50, 47, 45, 42, 40, 37, 35, 33, 31, 30, 28 };

/*Our sample*/
const unsigned char sineWave[64] = { 0x00, 0x00, 0x00, 0x03, 0x06, 0x0A, 0x0E, 0x11, 0x15, 0x17, 0x1A, 0x1D, 0x20, 0x23, 0x27, 0x2A, 0x2C, 0x30, 0x33, 0x35, 0x38, 0x3B, 0x3E, 0x41, 0x43, 0x46, 0x4A, 0x4C, 0x4F, 0x52, 0x55, 0x58, 0x59, 0x59, 0x54, 0x4F, 0x4B, 0x48, 0x44, 0x41, 0x3E, 0x3B, 0x39, 0x37, 0x35, 0x33, 0x30, 0x2E, 0x2C, 0x29, 0x26, 0x22, 0x1F, 0x1B, 0x18, 0x12, 0x0B, 0x05, 0xFE, 0xF8, 0xF2, 0xEC, 0xE7, 0xE4};

/*Function prototypes*/
unsigned short ReadLE16(unsigned char* Data);
static void Write8B(unsigned char* buffer, unsigned int value);
static void WriteBE32(unsigned char* buffer, unsigned long value);
static void WriteBE24(unsigned char* buffer, unsigned long value);
static void WriteBE16(unsigned char* buffer, unsigned int value);
static void WriteLE16(unsigned char* buffer, unsigned int value);
static void WriteLE24(unsigned char* buffer, unsigned long value);
static void WriteLE32(unsigned char* buffer, unsigned long value);
void imed2mod(long list, long dir);

/*Convert little-endian pointer to big-endian*/
unsigned short ReadLE16(unsigned char* Data)
{
	return (Data[0] << 0) | (Data[1] << 8);
}

static void Write8B(unsigned char* buffer, unsigned int value)
{
	buffer[0x00] = value;
}

static void WriteBE32(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0xFF000000) >> 24;
	buffer[0x01] = (value & 0x00FF0000) >> 16;
	buffer[0x02] = (value & 0x0000FF00) >> 8;
	buffer[0x03] = (value & 0x000000FF) >> 0;

	return;
}

static void WriteBE24(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0xFF0000) >> 16;
	buffer[0x01] = (value & 0x00FF00) >> 8;
	buffer[0x02] = (value & 0x0000FF) >> 0;

	return;
}

static void WriteBE16(unsigned char* buffer, unsigned int value)
{
	buffer[0x00] = (value & 0xFF00) >> 8;
	buffer[0x01] = (value & 0x00FF) >> 0;

	return;
}

static void WriteLE16(unsigned char* buffer, unsigned int value)
{
	buffer[0x00] = (value & 0x00FF) >> 0;
	buffer[0x01] = (value & 0xFF00) >> 8;

	return;
}

static void WriteLE24(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0x0000FF) >> 0;
	buffer[0x01] = (value & 0x00FF00) >> 8;
	buffer[0x02] = (value & 0xFF0000) >> 16;

	return;
}

static void WriteLE32(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0x000000FF) >> 0;
	buffer[0x01] = (value & 0x0000FF00) >> 8;
	buffer[0x02] = (value & 0x00FF0000) >> 16;
	buffer[0x03] = (value & 0xFF000000) >> 24;

	return;
}

int main(int args, char* argv[])
{
	printf("Climax/Images Software IMED module (GB/GBC/GG/SMS) to MOD converter\n");
	if (args != 2)
	{
		printf("Usage: IMED2MOD <module>\n");
		return -1;
	}
	else
	{
		if ((imed = fopen(argv[1], "rb")) == NULL)
		{
			printf("ERROR: Unable to open file %s!\n", argv[1]);
			exit(1);
		}
		else
		{

			/*Copy filename from argument - based on code by ValleyBell*/
			strcpy(OutFileBase, argv[1]);
			tempPnt = strrchr(OutFileBase, '.');
			if (tempPnt == NULL)
			{
				tempPnt = OutFileBase + strlen(OutFileBase);
			}
			*tempPnt = 0;

			fseek(imed, 0, SEEK_END);
			imedLength = ftell(imed);
			fseek(imed, 0, SEEK_SET);
			imedData = (unsigned char*)malloc(imedLength);
			fread(imedData, 1, imedLength, imed);
			fclose(imed);
			if (!memcmp(&imedData[0], gbID, 8))
			{
				printf("Detected format: Game Boy\n");
				systemID = 1;
				patList = ReadLE16(&imedData[8]);
				printf("Pattern list offset: 0x%04X\n", patList);
				patDir = ReadLE16(&imedData[10]);
				printf("Pattern directory offset: 0x%04X\n", patDir);
				chan12Data = ReadLE16(&imedData[12]);
				printf("Channel 1 & 2 patch data: 0x%04X\n", chan12Data);
				chan3Data = ReadLE16(&imedData[14]);
				printf("Channel 3 patch data: 0x%04X\n", chan3Data);
				waveTable = ReadLE16(&imedData[16]);
				printf("Waveform table: 0x%04X\n", waveTable);
				chan4Data = ReadLE16(&imedData[18]);
				printf("Channel 4 patch data: 0x%04X\n", chan4Data);
				imed2mod(patList, patDir);

			}
			else if (!memcmp(&imedData[0], ggID, 8))
			{
				printf("Detected format: Game Gear\n");
				systemID = 2;
				patList = ReadLE16(&imedData[8]);
				printf("Pattern list offset: 0x%04X\n", patList);
				patDir = ReadLE16(&imedData[10]);
				printf("Pattern directory offset: 0x%04X\n", patDir);
				chan12Data = ReadLE16(&imedData[12]);
				printf("Patch data: 0x%04X\n", chan12Data);
				numInst = imedData[14];
				printf("Number of instruments: %i\n", numInst);
				imed2mod(patList, patDir);
			}
			else
			{
				printf("ERROR: Invalid or unsupported module format!\n");
				exit(-1);
			}
		}
		printf("The operation was successfully completed!\n");
	}
}

void imed2mod(long list, long dir)
{
	unsigned char mask[2];
	int curPat = 0;
	long curPtr;
	long nextPtr;
	unsigned char command[4];
	long imedPos = 0;
	long dataPos = 0;
	long modPos = 0;
	long tempPos = 0;
	int channels = 4;
	int curNote = 0;
	int totalPos = 0;
	int highestPos = 0;
	int curRow = 0;
	int i = 0;
	int tempVal;
	int maskArray[16];
	int curPattern[16];

	modLength = 0x100000;
	modData = ((unsigned char*)malloc(modLength));

	for (i = 0; i < modLength; i++)
	{
		modData[i] = 0;
	}
	sprintf(outfile, "%s_converted.mod", OutFileBase);
	if ((mod = fopen(outfile, "wb")) == NULL)
	{
		printf("ERROR: Unable to write to file %s_converted.mod!\n", OutFileBase);
		exit(2);
	}
	else
	{

		/*Write MOD header*/
		modPos = 0;
		sprintf((char*)&modData[modPos], "IMED2MOD conversion");
		modPos += 20;

		/*Write sample/instrument parameters*/
		for (i = 0; i < 31; i++)
		{
			sprintf((char*)&modData[modPos], "sinewave");
			modPos += 22;
			/*Sample length*/
			WriteBE16(&modData[modPos], 32);
			modPos += 2;
			/*Fine tune*/
			Write8B(&modData[modPos], 0);
			modPos++;
			/*Sample volume*/
			Write8B(&modData[modPos], 64);
			modPos++;
			/*Loop point start*/
			WriteBE16(&modData[modPos], 0);
			modPos += 2;
			/*Loop point end*/
			WriteBE16(&modData[modPos], 32);
			modPos += 2;
		}

		/*Get ready for the pattern table...*/
		imedPos = list;
		/*Number of song positions - fill out soon*/
		Write8B(&modData[modPos], 0);
		tempPos = modPos;
		modPos++;
		/*Set this byte to 127*/
		Write8B(&modData[modPos], 127);
		modPos++;
		/*Fill in the pattern table*/
		while (imedData[imedPos] != 0xFF)
		{
			Write8B(&modData[modPos], imedData[imedPos]);
			if (imedData[imedPos] > highestPos)
			{
				highestPos = imedData[imedPos];
			}
			totalPos++;
			modPos++;
			imedPos++;
		}
		/*Finally fill in the value for total number of positions*/
		Write8B(&modData[tempPos], totalPos);
		/*Skip to end of pattern data area*/
		modPos += 128 - totalPos;
		/*Put in the initials M.K.*/
		sprintf((char*)&modData[modPos], "M.K.");
		modPos += 4;

		/*Now for the actual pattern data...*/
		curPtr = dir;
		for (curPat = 0; curPat < highestPos+1; curPat++)
		{
			curPtr = dir + ReadLE16(&imedData[dir + (curPat * 2)]);
			nextPtr = dir + ReadLE16(&imedData[dir + (curPat * 2)] + 2);
			imedPos = curPtr;
			
			while (imedPos < nextPtr)
			{
				/*Get the current channel mask*/
				mask[0] = imedData[imedPos];
				mask[1] = imedData[imedPos + 1];

				/*Convert the mask values to an array*/
				maskArray[0] = mask[0] >> 7 & 1;
				maskArray[1] = mask[0] >> 6 & 1;
				maskArray[2] = mask[0] >> 5 & 1;
				maskArray[3] = mask[0] >> 4 & 1;
				maskArray[4] = mask[0] >> 3 & 1;
				maskArray[5] = mask[0] >> 2 & 1;
				maskArray[6] = mask[0] >> 1 & 1;
				maskArray[7] = mask[0] & 1;
				maskArray[8] = mask[1] >> 7 & 1;
				maskArray[9] = mask[1] >> 6 & 1;
				maskArray[10] = mask[1] >> 5 & 1;
				maskArray[11] = mask[1] >> 4 & 1;
				maskArray[12] = mask[1] >> 3 & 1;
				maskArray[13] = mask[1] >> 2 & 1;
				maskArray[14] = mask[1] >> 1 & 1;
				maskArray[15] = mask[1] & 1;
				imedPos += 2;
				for (i = 0; i < 16; i++)
				{
					if (maskArray[i] == 1)
					{
						curPattern[i] = imedData[imedPos];
						imedPos++;
					}
					else if (maskArray[i] == 0)
					{
						curPattern[i] = 0;
					}
				}

				/*Transfer the current IMED module data to MOD*/

				/*Channel 1 note*/
				if (curPattern[0] != 0)
				{
					curNote = noteVals[curPattern[0] - 1];
					WriteBE16(&modData[modPos], curNote);
				}
				else if (curPattern[0] == 0)
				{
					Write8B(&modData[modPos], 0);
				}
				modPos += 2;
				/*Channel 1 instrument*/
				if (curPattern[1] != 0)
				{
					Write8B(&modData[modPos], (curPattern[1]) << 4);
				}
				else if (curPattern[1] == 0)
				{
					Write8B(&modData[modPos], 0);
				}
				modPos++;
				/*Channel 1 effect*/
				if (curPattern[2] != 0)
				{
					tempVal = modData[modPos - 1];
					tempVal = tempVal | curPattern[2];
					Write8B(&modData[modPos - 1], tempVal);
				}
				else if (curPattern[2] == 0)
				{
					;
				}
				/*Channel 1 effect parameters*/
				if (curPattern[3] != 0)
				{
					Write8B(&modData[modPos], curPattern[3]);
				}
				else if (curPattern[3] == 0)
				{
					Write8B(&modData[modPos], 0);
				}
				modPos++;

				/*Channel 2 note*/
				if (curPattern[4] != 0)
				{
					curNote = noteVals[curPattern[4] - 1];
					WriteBE16(&modData[modPos], curNote);
				}
				else if (curPattern[4] == 0)
				{
					Write8B(&modData[modPos], 0);
				}
				modPos += 2;
				/*Channel 2 instrument*/
				if (curPattern[5] != 0)
				{
					Write8B(&modData[modPos], (curPattern[5]) << 4);
				}
				else if (curPattern[5] == 0)
				{
					Write8B(&modData[modPos], 0);
				}
				modPos++;
				/*Channel 2 effect*/
				if (curPattern[6] != 0)
				{
					tempVal = modData[modPos - 1];
					tempVal = tempVal | curPattern[6];
					Write8B(&modData[modPos - 1], tempVal);
				}
				else if (curPattern[6] == 0)
				{
					;
				}
				/*Channel 2 effect parameters*/
				if (curPattern[7] != 0)
				{
					Write8B(&modData[modPos], curPattern[7]);
				}
				else if (curPattern[7] == 0)
				{
					Write8B(&modData[modPos], 0);
				}
				modPos++;

				/*Channel 3 note*/
				if (curPattern[8] != 0)
				{
					curNote = noteVals[curPattern[8] - 13];
					WriteBE16(&modData[modPos], curNote);
				}
				else if (curPattern[8] == 0)
				{
					Write8B(&modData[modPos], 0);
				}
				modPos += 2;
				/*Channel 3 instrument*/
				if (curPattern[9] != 0)
				{
					Write8B(&modData[modPos], (curPattern[9]) << 4);
				}
				else if (curPattern[9] == 0)
				{
					Write8B(&modData[modPos], 0);
				}
				modPos++;
				/*Channel 3 effect*/
				if (curPattern[10] != 0)
				{
					tempVal = modData[modPos - 1];
					tempVal = tempVal | curPattern[10];
					Write8B(&modData[modPos - 1], tempVal);
				}
				else if (curPattern[10] == 0)
				{
					;
				}
				/*Channel 3 effect parameters*/
				if (curPattern[11] != 0)
				{
					Write8B(&modData[modPos], curPattern[11]);
				}
				else if (curPattern[11] == 0)
				{
					Write8B(&modData[modPos], 0);
				}
				modPos++;

				/*Channel 4 note*/
				if (curPattern[12] != 0)
				{
					if ((curPattern[12] - 1) >= 48)
					{
						curPattern[12] -= 24;
					}
					curNote = noteVals[curPattern[12] - 1];
					WriteBE16(&modData[modPos], curNote);
				}
				else if (curPattern[12] == 0)
				{
					Write8B(&modData[modPos], 0);
				}
				modPos += 2;
				/*Channel 4 instrument*/
				if (curPattern[13] != 0)
				{
					Write8B(&modData[modPos], (curPattern[13]) << 4);
				}
				else if (curPattern[13] == 0)
				{
					Write8B(&modData[modPos], 0);
				}
				modPos++;
				/*Channel 4 effect*/
				if (curPattern[14] != 0)
				{
					tempVal = modData[modPos - 1];
					tempVal = tempVal | curPattern[14];
					Write8B(&modData[modPos - 1], tempVal);
				}
				else if (curPattern[14] == 0)
				{
					;
				}
				/*Channel 4 effect parameters*/
				if (curPattern[15] != 0)
				{
					Write8B(&modData[modPos], curPattern[15]);
				}
				else if (curPattern[15] == 0)
				{
					Write8B(&modData[modPos], 0);
				}
				modPos++;
			}
		}


		for (i = 0; i < 31; i++)
		{
			int j;
			for (j = 0; j < 64; j++)
			{
				Write8B(&modData[modPos + j], sineWave[j]);
			}
			modPos += 64;
		}
		fwrite(modData, modPos, 1, mod);
		fclose(mod);
	}
}