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
#include <windows.h>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <string>

using namespace std;
string getOutpuDirectory(string inputfile);
std::string string_to_hex(const std::string& input);
bool getImages();
bool encodeImage();
bool decodeImage();
string validateFile(string fileType, bool checkExists = true);

int main(int argc, char** argv)
{
	bool cont = true;
	while(cont)
	{
		cout << "Please select task by entering numerical choice: " << endl;
		cout << " --> For file recovery,   press 1" << endl;
		cout << " --> To encode an image,  press 2" << endl;
		cout << " --> To decode an image,  press 3" << endl;
		cout << " --> To exit the program, press q" << endl;
		switch (std::cin.get())
		{
		case '1':
			getImages();
			break;

		case '2':
			encodeImage();
			break;

		case '3':
			decodeImage();
			break;

		case 'q':
			cont = false;
			break;

		default:
			cout << "error, invalid choice" << endl;
			break;

		}
	}

}

bool encodeImage()
{
	string file = validateFile(".jpg");
	fstream stream(file, ios::binary);
	stream.seekg(0, stream.end);
	int length = stream.tellg();
	stream.seekg(0, stream.beg);
	stream.seekg(0, length / 2);
	string message;
	int k = 0;
	do
	{
		cout << "Please enter the message to encode" << endl;
		getline(cin, message);
	} while (message.length() <= 0);
	unsigned char * cMessage = (unsigned char *)message.c_str();

	int what_bit_i_am_testing = 0;
	for (int i = 0; i < message.length(); i++)
	{
		while (what_bit_i_am_testing < 8) 
		{
			char temp = stream.get();
			temp = temp ^ message[i];

			what_bit_i_am_testing++;
			message[i] = message[i] >> 1;
			k++;
		}
		what_bit_i_am_testing = 0;
	}

	return true;
}

bool decodeImage()
{
	string file = validateFile(".jpg");

	return true;
}


bool getImages()
{
	string file = validateFile(".img");


	std::string fileName;
	std::ifstream input(file, std::ios::binary);
	std::ofstream out;
	string directory = getOutpuDirectory(file);


	int i = 0;
	int k = 0;
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

		for (i = 0; i < length / 512; i++)
		{

			input.read((char *)buffer, 512);

			if (buffer[0] == 0xFF && buffer[1] == 0xD8 || buffer[0] == 0xE5)
			{
				copy = true;
				printf("found start of file \n");
				while (copy)
				{
					if (k == 512)
					{
						input.read((char *)buffer, 512);
						k = 0;
					}
					if (buffer[k] == 0xFF && buffer[k + 1] == 0xD8)
					{
						if (depth == 0)
						{
							printf("Starting at byte %d, sector %.2f\n", i, i / 512.0);
							fileName = directory + "/out" + to_string(files) + ".jpg";
							cout << "Creating file " << fileName << endl;
							out = ofstream(fileName, ios::binary);
						}

						sof++;
						depth++;
					}

					if (buffer[k] == 0xFF && buffer[k + 1] == 0xD9)
					{
						eof++;
						depth--;
						printf("found end of file\n");
						if (depth == 0)
						{
							out << buffer[k];
							out << buffer[k + 1];
							copy = false;
							files++;
							cout << "closing file " << fileName << endl;
							out.close();
						}
					}
					if (copy)
					{
						out << buffer[k];
					}
					k++;
				}


			}
			k = 0;

		}
		input.close();

		printf("SOFs found: %d, EOFs found: %d\n", sof, eof);
		printf("found %d files\n", files);
		delete[] buffer;
	}
	return true;
}

string getOutpuDirectory(string input)
{
	input = input.substr(0, input.find_last_of("."));
	CreateDirectory(input.c_str(), NULL);
	return input;
}


std::string string_to_hex(const std::string& input)
{
	static const char* const lut = "0123456789ABCDEF";
	int len = input.length();

	std::string output;
	output.reserve(2 * len);
	for (int i = 0; i < len; ++i)
	{
		const unsigned char c = input[i];
		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);
	}
	return output;
}


string validateFile(string fileType, bool checkExists)
{
	bool valid = false;
	string file;
	while (!valid)
	{
		do
		{
			cout << "Please enter name of valid " << fileType << " file" << endl;
			getline(cin, file);
		} while (file.find(fileType) == string::npos);

		if(checkExists)
		{
			ifstream temp(file.c_str());
			valid = temp.good();
		}
	}
	return file;
}