/*
*	FILE		  : A2.h
*   PROJECT		  :	ACS - Assignment 2
*   PROGRAMMER	  : Wes Martin
*	FIRST VERSION : 4/1/18
*	DESCRIPTION	  : This file contains the the declarations and definitions needed to encode, decode
*					and recover images from an old camera file.
*/

#pragma once

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

// Defines
#define kSUCCESS		0
#define kFAILURE		-1
#define kDEFAULT		0
#define kEMPTY_S		""
#define kEMPTY			0
#define kCUT_IN_HALF	2
#define kBITS_PER_BYTE	8
#define kFIRST_INDEX	0
#define kSECOND_INDEX	1
#define kBACKWARDS		-1
#define kFORWARDS		1
#define kFOOTER_LENGTH	5
#define kFOOTER			"\r\r\r\r\r"
#define kBIT_EIGHT		0b10000000
#define kFIRST_BIT		0b00000001
#define kCONTROL_BYTE	0xFF
#define kSOI			0xD8
#define kEOI			0xD9
#define kERASED_RECORD	0xE5
#define kSECTOR_SIZE	512
#define kREAD_BUFFER	2
#define kHEADER_SIZE	173
#define kBYTES_TO_READ	1
#define kBYTES_TO_WRITE	1




// Prototypes
using namespace Gdiplus;
using namespace std;
string getOutpuDirectory(string inputfile);
std::string string_to_hex(const std::string& input);
bool get_images();
bool encodeImage();
bool decodeImage();
string validateFile(string fileType, bool checkExists = true);
int convertToBMP(string file);
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
void getThumbnail(int* files, string directory, ifstream& input, vector<string>& allFiles);
unsigned char* thumbnailHeader();
