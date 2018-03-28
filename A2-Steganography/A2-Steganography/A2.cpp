/*
*	FILE		  : .cpp
*   PROJECT		  :	GAS - Assignment 1
*   PROGRAMMER	  : Wes Martin
*	FIRST VERSION : 12/2/18
*	DESCRIPTION	  : This file contains the
*/

// Pragmas
#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int is_jpg(unsigned char*);

int main(int argc, char* argv[])
{
	FILE* outptr = nullptr;

	//open card file
	FILE* inptr = fopen("control.img", "r");
	if (inptr == nullptr)
	{
		printf("File not found");
		return 1;
	}


	// reserve space for 512 byte blocks
	unsigned char* buffer = (unsigned char *)malloc(512 * sizeof(char));
	if (buffer == nullptr)
	{
		printf("Out of memory");
		return 2;
	}

	//reserve space for title
	char* title = (char *)malloc(8 * sizeof(char));
	if (buffer == nullptr)
	{
		printf("Out of memory");
		return 2;
	}

	int pic_number = 0;
	int bytesRead = 0;
	int totalBytesRead = 0;
	//repeat until end of card   
	//read 512 bytes from memory
	while ((bytesRead = fread(buffer, 1, 512, inptr)))
	{
		totalBytesRead += bytesRead;
		// start of first jpeg
		if (is_jpg(buffer) == 1 && pic_number == 0)
		{
			//create new jpeg
			sprintf(title, "%03d.jpg", pic_number);
			outptr = fopen(title, "w");

			pic_number++;
			//go to next read
		}

			// start of all other jpegs
		else if (is_jpg(buffer) == 1 && pic_number > 0)
		{
			//close old jpeg
			fclose(outptr);

			//create new jpg
			sprintf(title, "%03d.jpg", pic_number);
			outptr = fopen(title, "w");

			pic_number++;
			//go to next read            
		}

		// if a jpeg is currently open        
		if (pic_number > 0)
		{
			//add 512 bytes to open file
			fwrite(buffer, 1, 512, outptr);
		}
	}
	
	//close any remaning files
	fclose(outptr);
	fclose(inptr);
	free(buffer);
	free(title);
}


/**
* is_jpg(char*)
*
* Compares memory at pointer to standard JPEG signature
* Pass in pointer to 512 bytes of memory
* Returns 1 if it is the start of a jpeg
*
*/

int is_jpg(unsigned char* test)
{
	// check for FFD8FFE at start of chunk
	if (*test == 0xff &&
		*(test + 1) == 0xd8)
	{
		return 1;
	}
	return 0;
}
