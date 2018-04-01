/*
*	FILE		  : A2.cpp
*   PROJECT		  :	ACS - Assignment 2
*   PROGRAMMER	  : Wes Martin
*	FIRST VERSION : 4/1/18
*	DESCRIPTION	  : This file contains the the methods needed to encode, decode
*					and recover images from an old camera file.
*/

#include "A2.h"

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
			get_images();
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

/**
 * \brief This method prompts the user for a file and, after verifying its existance, 
 *		  it will prompt the user to enter a string. It will then ecode that string 
 *		  into the image using a top scret encoding scheme.
 * \return Status indicating success
 */
bool encodeImage()
{
	// Get the file from the user and validate it
	string file = validateFile(".bmp");

	// Prepare the stream
	fstream stream(file, std::ios::binary | std::fstream::in | std::fstream::out);
	if (stream)
	{
		// Calculate the length of the file
		stream.seekg(kDEFAULT, stream.end);
		int length = stream.tellg();
		stream.seekg(kDEFAULT, stream.beg);

		// Seek to the middle of the file
		stream.seekg(length / kCUT_IN_HALF, stream.beg);

		// Get the message from the user
		string message;
		int k = kDEFAULT;
		do
		{
			cout << "Please enter the message to encode" << endl;
			getline(cin, message);
		}
		while (message.length() <= kEMPTY);

		// Append the footer
		message += kFOOTER;

		// Prepare variables
		int r, g, b = kDEFAULT;
		int what_bit_i_am_testing = kDEFAULT;
		char input[kSECOND_INDEX];

		// Account for rounding error
		stream.seekp(kBACKWARDS * 1, ios_base::cur);

		// Step through every byte in the secret message
		for (int i = kDEFAULT; i < message.length(); i++)
		{
			// Step through every bit in each byte
			while (what_bit_i_am_testing < kBITS_PER_BYTE)
			{
				// Get the next character to encode into
				stream.seekp(kFORWARDS, ios_base::cur);
				stream.read((char *)input, kBYTES_TO_READ*1);
				stream.seekp(kBACKWARDS * 2, ios_base::cur);

				// Encode the byte with the next bit of the byte of the message 
				input[kFIRST_INDEX] = (input[kFIRST_INDEX] & !kFIRST_BIT) | (message[i] & kFIRST_BIT);
				stream.write(input, kBYTES_TO_READ);

				// Move to the next bit
				what_bit_i_am_testing++;
				message[i] = message[i] >> 1;
				k++;
			}
			what_bit_i_am_testing = kEMPTY;
		}

		// Shut down everything
		stream.close();
	}
	return true;
}



/**
* \brief This method prompts the user for a file and, after verifying its existance,
*		  it will Look through that file for the secret encoded message. It will either 
*		  display the hidden message, or it will display an error message
 * \return Success of the decoding
 */
bool decodeImage()
{
	// Get file from user and validate it
	string file = validateFile(".bmp");
	fstream stream(file, std::ios::binary | std::fstream::in);
	if (stream)
	{
		// Get length of file, seek to middle
		stream.seekg(kDEFAULT, stream.end);
		int length = stream.tellg();
		stream.seekg(kDEFAULT, stream.beg);
		stream.seekg(length / kCUT_IN_HALF, stream.beg);

		// Initialize default values
		string message;
		int k = kDEFAULT;
		int what_bit_i_am_testing = kDEFAULT;
		unsigned char input[kREAD_BUFFER] = kEMPTY_S;
		unsigned char byte = kDEFAULT;

		// Account for rounding error
		stream.seekp(kBACKWARDS, ios_base::cur);

		// Go until the end of the file is reached
		while (stream.peek() != EOF)
		{

			// Step through 8 bytes at a time, pulling out lowest bit
			while (what_bit_i_am_testing < kBITS_PER_BYTE)
			{
				// Shift right, to access next lowest bit
				byte = byte >> 1;
				int test = stream.tellp();
				input[kFIRST_INDEX] = stream.get();

				// Decode the lowest bit from the byte
				byte = byte | ((input[kFIRST_INDEX] & kFIRST_BIT) * kBIT_EIGHT);

				// Increment counters
				what_bit_i_am_testing++;
				k++;
			}
			what_bit_i_am_testing = kDEFAULT;

			// Check to see if the message footer has been found yet
			if (message.find(kFOOTER) != string::npos)
			{
				message = message.erase(message.length() - kFOOTER_LENGTH);
				break;
			}
			message += byte;
			byte = kDEFAULT;
		}

		// Display appropriate message
		if (stream.peek() == EOF)
		{
			cout << "Sorry, no message found" << endl;
		}
		else
		{
			cout << "Message is " << message << endl;
		}
	}
	return true;
}




/**
 * \brief This method prompts the user for a .img file and, after validating it, steps through that file
 *		  one sector at a time, looking for the start of a jpeg. When found, it steps through memory from 
 *		  that point, reading in one sector after another. It scans through, looking for either the start
 *		  of a thumbnail, or the end of the image. IF a thumbnail is found, it will call the get thumbnail 
 *		  method. Once done, it returns. It then saves all the images to a folder based on the img name, 
 *		  and then converts all valid jpegs to bmps.
 * \return Success of image read
 */
bool get_images()
{
	bool getThumbnails = true;
	// Check if thumbnails can be extracted and arrange it with the users
	fstream stream("thumbnailTemplate.jpg", std::ios::binary | std::fstream::in | std::fstream::out);
	if (stream)
	{
		stream.close();
	}
	else
	{
		cout << "You will not be able to get thumbnails without" << endl;
		cout << "including the thumbnailTemplate.jpg file. Do you" << endl;
		cout << "wish to continue without isolating thumbnails?" << endl << endl;
		cout << "Please enter [y] to confirm or [n] for to exit" << endl;
		bool cont = true;
		while(cont)
		{
			switch (cin.get())
			{
			case 'y':
			case 'Y':
				getThumbnails = false;
				cont = false;
				break;

			case'n':
			case 'N':
				return false;
			}
		}
	}
	// Get .img file and validate it
	string file = validateFile(".img");

	// Create necessary objects
	vector<string> allFiles;
	std::string fileName;
	std::ifstream input(file, std::ios::binary);
	std::ofstream out;

	// Get directory name for images from file name
	string directory = getOutpuDirectory(file);

	// Initialize defaults
	int i = kDEFAULT;
	int k = kDEFAULT;
	if (input)
	{
		// get length of file:
		input.seekg(kDEFAULT, input.end);
		int length = input.tellg();
		input.seekg(kDEFAULT, input.beg);

		// initalize more defaults
		unsigned char* buffer = new unsigned char[length];
		int sof = kDEFAULT;
		int eof = kDEFAULT;
		int files = kDEFAULT;
		int depth = kDEFAULT;
		bool copy = false;
		cout << "Reading " << length << " characters... " << endl;

		// read data as a block of 512
		for (i = kDEFAULT; i < length / kSECTOR_SIZE; i++)
		{
			input.read((char *)buffer, kSECTOR_SIZE);

			// Check for either a jpeg or erased sector
			if (buffer[kFIRST_INDEX] == kCONTROL_BYTE && buffer[kSECOND_INDEX] == kSOI 
				|| buffer[kFIRST_INDEX] == kERASED_RECORD)
			{
				copy = true;

				// While copy is enabled
				while (copy)
				{
					// If needed, read in the next chunk
					if (k == kSECTOR_SIZE)
					{
						input.read((char *)buffer, kSECTOR_SIZE);
						k = kEMPTY;
					}
					// Check if at start of jpeg
					if (buffer[k] == kCONTROL_BYTE && buffer[k + kSECOND_INDEX] ==kSOI)
					{
						// IF at current level, create this images file and header
						if (depth == kEMPTY)
						{
							fileName = directory + "/out" + to_string(files) + ".jpg";
							allFiles.push_back(directory + "/out" + to_string(files));
							cout << "Creating file " << fileName << endl;
							out = ofstream(fileName, ios::binary);
						}
						// If "further down" in layers of jpeg, create a thumbnail, setup new image
						else if (getThumbnails)
						{
							files++;
							int test = input.tellg();
							getThumbnail(&files, directory, input, allFiles);
							test = input.tellg();
						}
						// Incremenet values
						sof++;
						depth++;
					}

					// Check if at end of jpeg
					if (buffer[k] == kCONTROL_BYTE && buffer[k + kSECOND_INDEX] == kEOI)
					{
						// Decrease depth
						eof++;
						depth--;

						// Close and seal the file
						if (depth == kEMPTY)
						{
							out << buffer[k];
							out << buffer[k + kSECOND_INDEX];
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
			k = kEMPTY;
		}
		input.close();

		// Print out message
		printf("Found %d files\n", files);
		delete[] buffer;
		
		// Covert all the files to bmps, if possible
		for (int i = kDEFAULT; i < allFiles.size(); i++)
		{
			convertToBMP(allFiles[i]);
		}
	}
	return true;
}



/**
 * \brief Uses the name of the image file to create the directory for the recovered images
 * \param input The string to pull the directory out of
 * \return The directory name
 */
string getOutpuDirectory(string input)
{
	input = input.substr(kFIRST_INDEX, input.find_last_of("."));
	CreateDirectory(input.c_str(), nullptr);
	return input;
}




/**
 * \brief Validate that a user given file exists and that the type is correct
 * \param fileType The type to check for
 * \param checkExists If set to true, check if file exists
 * \return String with name of file
 */
string validateFile(string fileType, bool checkExists)
{
	bool valid = false;
	string file;
	while (!valid)
	{
		// Get file from user until correct type is provided
		do
		{
			cout << "Please enter name of valid " << fileType << " file" << endl;
			getline(cin, file);
		}
		while (file.find(fileType) == string::npos);

		// If requestsed, check if file exists
		if (checkExists)
		{
			// Open, check, and close stream
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


//------------------------------------------------------------------------------------------------------------------------------------------------

// The following two methods, GetEncoderCLsid and convertToBMP, where both pulled from
// https://www.neowin.net/forum/topic/803984-cc-jpg-to-bmp-conversion/ and I owe them a
// for basically saving my life on this.

/**
 * \brief I am not sure what this does, but it works.
 * \param format A magic format type
 * \param pClsid A data type I have never heard of. No touchy
 * \return 
 */
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = kDEFAULT; // number of image encoders
	UINT size = kDEFAULT; // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = nullptr;

	GetImageEncodersSize(&num, &size);
	if (size == kEMPTY)
		return kFAILURE; // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == nullptr)
		return kFAILURE; // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = kDEFAULT; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == kSUCCESS)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j; // Success
		}
	}
	free(pImageCodecInfo);
	return kFAILURE;
}


/**
 * \brief Converts a jpeg, if valid, to bmp.
 * \param file The jpeg to convert
 * \return Indicates success of conversion
 */
int convertToBMP(string file)
{
	// Get name of jpeg file 
	string jpg = file + ".jpg";
	std::wstring jpgTemp = std::wstring(jpg.begin(), jpg.end());
	wchar_t* jpgFile = (wchar_t *)jpgTemp.c_str();


	// Get name of bmp file
	string bmp = file + ".bmp";
	std::wstring bmpTemp = std::wstring(bmp.begin(), bmp.end());
	wchar_t* bmpFile = (wchar_t *)bmpTemp.c_str();

	// Magic data types I have never heard of, it seems to be GDI based
	GdiplusStartupInput startupInput;
	ULONG_PTR token;
	GdiplusStartup(&token, &startupInput, nullptr);
	Image* testImg = Image::FromFile(jpgFile, false);
	CLSID clsid;
	int ret = kFAILURE;

	if (kFAILURE != GetEncoderClsid(L"image/bmp", &clsid))
	{
		// Spit out the fresh faced bmp
		ret = testImg->Save(bmpFile, &clsid);
		cout << bmp << endl;
	}

	// Clean up 
	delete testImg;
	testImg = nullptr;
	GdiplusShutdown(token);
	return ret;
}
//------------------------------------------------------------------------------------------------------------------------------------------------


/** IMPORTANT: FOR THIS TO WORK, THE FILE thumbnailTemplate.jpg MUST EXIST!!!!!!!!!!!!!!
 *
 * \brief This method is used to pull a thumbnail out of a jpeg oject, and creates a new 
 *		  jpeg out of the thumbnail. 
 * \param files The number of files created so far
 * \param directory The directory to write the file to
 * \param input The input stream to read from
 * \param allFiles The list of all file names
 */
void getThumbnail(int* files, string directory, ifstream& input, vector<string>& allFiles)
{
	// Rewind two, so that the who file is read in
	int currLocation = input.tellg();
	input.seekg(kBACKWARDS * 2, ios_base::cur);
	int thisFile = *files;

	// Create objects
	std::string fileName;
	std::ofstream out;
	int k = kDEFAULT;
	unsigned char* buffer = new unsigned char[kREAD_BUFFER];
	fileName = directory + "/out" + to_string(thisFile) + ".jpg";

	// Create output file
	cout << "Creating file " << fileName << endl;
	out = ofstream(fileName, ios::binary);

	// Get thumbnail header from jpeg template and copy it in
	unsigned char* header = thumbnailHeader();
	out.write((char *)header, kHEADER_SIZE);

	// Fast forward 2. 
	input.seekg(kFORWARDS * 2, ios_base::cur);

	// Go until end of file, if needed
	while (input.peek() != EOF)
	{
		// Get the next byt
		input.read((char *)buffer, kBYTES_TO_READ);

		// Check if key value bytes were hit
		if (buffer[kFIRST_INDEX] == kCONTROL_BYTE|| buffer[kFIRST_INDEX] == kERASED_RECORD)
		{
			// Get next byte and write both to file
			out << buffer[kFIRST_INDEX];
			input.read((char *)buffer, kBYTES_TO_READ);
			out << buffer[kFIRST_INDEX];

			// If another thumbnail is detected, dive in again
			if (buffer[kFIRST_INDEX] == kSOI)
			{
				getThumbnail(files, directory, input, allFiles);
				break;
			}
			// IF end of file is reached, close up thumbnail shop and exit method
			if (buffer[kFIRST_INDEX] == kEOI)
			{
				cout << "closing file " << fileName << endl;
				out.close();

				allFiles.push_back(directory + "/out" + to_string(thisFile));
				break;
			}
		}
		else
		{
			// Write regular bytes to file
			out << buffer[kFIRST_INDEX];
		}
	}
	k = kEMPTY;

	// Reset location to not mess with parent jpeg
	input.seekg(currLocation);
}



/** IMPORTANT: FOR THIS TO WORK, THE FILE thumbnailTemplate.jpg MUST EXIST!!!!!!!!!!!!!!
*
* \brief This method creates the header text to be writting to the start of the thumbnail
* \return The character string representing header
*/
unsigned char* thumbnailHeader()
{
	unsigned char* header = (unsigned char *)malloc(kHEADER_SIZE);
	ifstream in("thumbnailTemplate.jpg", ios::binary);
	if (in)
	{
		in.read((char *)header, kHEADER_SIZE);
		in.close();
	}
	else
	{
		cout << "Error: Cannot process thumbnails" << endl;
	}
	return header;
}
