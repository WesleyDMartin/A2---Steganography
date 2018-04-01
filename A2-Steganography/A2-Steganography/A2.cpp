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
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <vector>
#pragma comment (lib,"Gdiplus.lib")

using namespace Gdiplus;
using namespace std;
string getOutpuDirectory(string inputfile);
std::string string_to_hex(const std::string& input);
bool getImages();
bool encodeImage();
bool decodeImage();
string validateFile(string fileType, bool checkExists = true);
void splitYCrCb(char in, int *r, int *g, int *b);
char createYCrCb(int r, int g, int b);
int test(string file);
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
void getThumbnail(int *files, string directory, ifstream &input, vector<string> &allFiles);

int main(int argc, char** argv)
{
	bool cont = true;
	while (cont)
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
	string file = validateFile(".bmp");
	fstream stream(file, std::ios::binary | std::fstream::in | std::fstream::out);
	if (stream)
	{
		stream.seekg(0, stream.end);
		int length = stream.tellg();
		stream.seekg(0, stream.beg);
		stream.seekg(length / 2, stream.beg);
		string message;
		int k = 0;
		do
		{
			cout << "Please enter the message to encode" << endl;
			getline(cin, message);
		} while (message.length() <= 0);
		message += "\r\r\r\r\r";
		int r, g, b = 0;
		int rindex, windex = 0;
		int what_bit_i_am_testing = 0;
		char  input[1];
		stream.seekp(-1, ios_base::cur);
		for (int i = 0; i < message.length(); i++)
		{
			while (what_bit_i_am_testing < 8)
			{
				rindex = stream.tellg();
				windex = stream.tellp();
				stream.seekp(1, ios_base::cur);
				stream.read((char *)input, 1);
				stream.seekp(-1, ios_base::cur);
				rindex = stream.tellg();
				windex = stream.tellp();
				stream.seekp(-1, ios_base::cur);
				rindex = stream.tellg();
				windex = stream.tellp();
				input[0] = (input[0] & 0xFE) | (message[i] & 0x01);
				stream.write(input, 1);
				rindex = stream.tellg();
				windex = stream.tellp();
				//stream.seekp(1, ios_base::cur);
				what_bit_i_am_testing++;
				message[i] = message[i] >> 1;
				k++;
			}
			what_bit_i_am_testing = 0;
		}
		stream.close();
	}
	return true;
}

bool decodeImage()
{
	string file = validateFile(".bmp");
	fstream stream(file, std::ios::binary | std::fstream::in);
	if (stream)
	{
		stream.seekg(0, stream.end);
		int length = stream.tellg();
		stream.seekg(0, stream.beg);
		stream.seekg(length / 2, stream.beg);

		string message;
		int k = 0;
		int what_bit_i_am_testing = 0;
		unsigned char input[1] = "";
		unsigned char byte = 0;
		stream.seekp(-1, ios_base::cur);
		while (stream.peek() != EOF)
		{
			while (what_bit_i_am_testing < 8)
			{

				byte = byte >> 1;
				int test = stream.tellp();
				input[0] = stream.get();

				byte = byte | ((input[0] & 0x01) * 128);
				what_bit_i_am_testing++;
				k++;
			}
			//stream.seekp(2, ios_base::cur);
			what_bit_i_am_testing = 0;
			if (message.find("\r\r\r\r\r") != string::npos)
			{
				message = message.erase(message.length() - 5);
				break;
			}
			message += byte;
			byte = 0;

		}

		if (stream.peek() == EOF)
		{
			cout << "Sorry, no message found";
		}
		else
		{
			cout << "Message is " << message << endl;
		}
	}
	return true;
}


bool getImages()
{
	string file = validateFile(".img");
	vector<string> allFiles;

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
							//printf("Starting at byte %d, sector %.2f\n", i, i / 512.0);
							fileName = directory + "/out" + to_string(files) + ".jpg";
							allFiles.push_back(directory + "/out" + to_string(files));
							cout << "Creating file " << fileName << endl;
							out = ofstream(fileName, ios::binary);
						}
						else
						{
							files++;
							int test = input.tellg();
							getThumbnail(&files, directory, input, allFiles);
							test = input.tellg();
						}
						sof++;
						depth++;
					}

					if (buffer[k] == 0xFF && buffer[k + 1] == 0xD9)
					{
						eof++;
						depth--;
						if (depth == 0)
						{
							out << buffer[k];
							out << buffer[k + 1];
							copy = false;
							cout << "closing file " << fileName << endl;
							out.close();
							files++;
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

		for (int i = 0; i < allFiles.size(); i++)
		{
			test(allFiles[i]);
		}
	}
	return true;
}

string getOutpuDirectory(string input)
{
	input = input.substr(0, input.find_last_of("."));
	CreateDirectory(input.c_str(), NULL);
	return input;
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

		if (checkExists)
		{
			fstream stream(file, std::ios::binary | std::fstream::in | std::fstream::out);
			if (stream)
			{
				valid = true;
				stream.close();
			}
		}
	}
	return file;
}


void splitYCrCb(char in, int  *r, int  *g, int  *b)
{
	int y = 0;
	int cr = 0;
	int cb = 0;

	cr += in & 1;
	cr += in & 2;
	cb += in & 4;
	cb += in & 8;
	y += in & 16;
	y += in & 32;
	y += in & 64;
	y += in & 128;
	double Y = (double)y;
	double Cb = (double)cb;
	double Cr = (double)cr;
	*r = (int)(Y + 1.40200 * (Cr - 0x80));
	*g = (int)(Y - 0.344136 * (Cb - 0x80) - 0.714136 * (Cr - 0x80));
	*b = (int)(Y + 1.77200 * (Cb - 0x80));
}

char createYCrCb(int r, int g, int b)
{
	char ret = 0;
	int y, cb, cr;
	y = (int)(.299 * r + .587 * g + .114 * b);
	cb = 128 - (int)(.168736 * r - .331264 * g + .5 * b);
	cr = 128 + (int)(.5 * r - .418688 * g - .081312 * b);

	ret += cr & 1;
	ret += cr & 2;
	ret += cb & 4;
	ret += cb & 8;
	ret += y & 16;
	ret += y & 32;
	ret += y & 64;
	//ret += y & 128;
	return ret;
}


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;		  // number of image encoders
	UINT  size = 0;		 // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}
	free(pImageCodecInfo);
	return -1;
}

//https://www.neowin.net/forum/topic/803984-cc-jpg-to-bmp-conversion/
int test(string file)
{
	string jpg = file + ".jpg";
	std::wstring jpgTemp = std::wstring(jpg.begin(), jpg.end());
	wchar_t* jpgFile = (wchar_t *)jpgTemp.c_str();

	string bmp = file + ".bmp";
	std::wstring bmpTemp = std::wstring(bmp.begin(), bmp.end());
	wchar_t* bmpFile = (wchar_t *)bmpTemp.c_str();


	GdiplusStartupInput startupInput;
	ULONG_PTR token;
	GdiplusStartup(&token, &startupInput, NULL);
	Image * testImg = Gdiplus::Image::FromFile(jpgFile, false);
	CLSID clsid;
	int ret = -1;

	if (-1 != GetEncoderClsid(L"image/bmp", &clsid)) {
		ret = testImg->Save(bmpFile, &clsid);
		cout << bmp << endl;
	}
	delete testImg;
	testImg = 0;
	GdiplusShutdown(token);
	return ret;
}

void getThumbnail(int *files, string directory, ifstream &input, vector<string> &allFiles)
{
	int currLocation = input.tellg();
	input.seekg(-2, ios_base::cur);
	int thisFile = *files;
	std::string fileName;
	std::ofstream out;

	int k = 0;

	unsigned char * buffer = new unsigned char[2];
	fileName = directory + "/out" + to_string(thisFile) + ".jpg";
	cout << "Creating file " << fileName << endl;
	out = ofstream(fileName, ios::binary);
	out << (char)0xFF;
	out << (char)0xD8;
	out << (char)0xFF;

	input.seekg(2, ios_base::cur);
	while (input.peek() != EOF)
	{
		input.read((char *)buffer, 1);
		if (buffer[0] == 0xFF || buffer[0] == 0xE5)
		{
			out << buffer[0];
			input.read((char *)buffer, 1);
			out << buffer[0];
			if(buffer[0] == 0xD8)
			{
				getThumbnail(files, directory, input, allFiles);
				break;
			}
			if (buffer[0] == 0xD9)
			{
				cout << "closing file " << fileName << endl;
				out.close();

				allFiles.push_back(directory + "/out" + to_string(thisFile));
				break;
			}
		}
		else
		{
			out << buffer[0];
		}
	}
	k = 0;
	input.seekg(currLocation);
}