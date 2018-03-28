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
#include <windows.h>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <string>

using namespace std;
string getOutpuDirectory(char *inputfile);

int main(int argc, char** argv)
{
	std::string fileName;
	std::ifstream input(argv[1], std::ios::binary);
	std::ofstream out;
	string directory = getOutpuDirectory(argv[1]);


	int i = 0;
	if (input) {
		// get length of file:
		input.seekg(0, input.end);
		int length = input.tellg();
		input.seekg(0, input.beg);

		unsigned char * buffer = new unsigned char[length];
		int sof = 0;
		int eof = 0;
		int files = 0;
		int depth = 0;
		bool copy = false;
		cout << "Reading " << length << " characters... " << endl;
		// read data as a block:
		input.read((char *)buffer, length);
		
		for (i = 0; i < length; i++)
		{
			//printf("%d\n", buffer[i]);
			if(buffer[i] == 0xFF && buffer[i+1] == 0xD8)
			{
				if(depth == 0)
				{
					fileName = directory + "/out" + to_string(files) + ".jpg";
					cout << "Creating file " << fileName << endl;					   
					out = ofstream(fileName, ios::binary);
				}
				copy = true;
				sof++;
				depth++;
			}
			if (buffer[i] == 0xFF && buffer[i + 1] == 0xD9)
			{
				eof++;
				depth--;
				if(depth == 0)
				{
					out << buffer[i];
					out << buffer[i + 1];
					copy = false;
					files++;
					cout << "closing file " << fileName << endl;
					out.close();
				}
			}
			if(copy)
			{
				out << buffer[i];
			}
		}
		input.close();

		printf("SOFs found: %d, EOFs found: %d\n", sof, eof);
		printf("found %d files\n", files);
		delete[] buffer;
	}
}

string getOutpuDirectory(char *inputfile)
{
	string input(inputfile);
	input = input.substr(0,input.find_last_of("."));
	CreateDirectory(input.c_str(), NULL);
	return input;
}
