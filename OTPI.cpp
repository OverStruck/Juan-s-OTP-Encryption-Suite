#include "OTPI.h"
#include <iostream>
#include <bitset>

OTPI::OTPI()
{
	this->PRNG.setParams(6011, 5011, 4);
}

OTPI::OTPI(const std::string key[3])
{
	this->PRNG.setParams(key[0].c_str(), key[1].c_str(), key[2].c_str());
}

void OTPI::RESETT(const std::string key[3])
{
	this->PRNG.setParams(key[0].c_str(), key[1].c_str(), key[2].c_str());
}


/*
	@param char plainText the plainText
*/
void OTPI::encrypt(char plainText[], int length)
{
	uint8_t randByte, cipherByte;

	for (int i = 0; i < length; i++)
	{
		//std::cout << "Plaintext: " << plainText[i] << " Bits:" << std::bitset<8>(plainText[i]) << " ";
		randByte = this->PRNG.getRandByte();
		//std::cout << "Encryption Byte: " << std::bitset<8>(randByte) << " ";
		cipherByte = (plainText[i] ^ randByte);
		plainText[i] = cipherByte;
		//std::cout << " Result: " << std::bitset<8>(cipherByte) << std::endl;
	}
}

void OTPI::encryptFile(const std::string inputFileName, const std::string outputFileName)
{
	//file length
	double length, pos = 0.0;
	//buffer to read file to
	char buffer[this->BLOCK_SIZE];
	//input src file stream
	std::ifstream srcFile(inputFileName, std::ios::ate | std::ios::binary);
	//output file stream
	std::ofstream outputFile(outputFileName, std::ios::out | std::ios::app | std::ios::binary);
	//basic file sanity check
	if (!srcFile)
	{
		std::cerr << "File: " << inputFileName << " failed to open" << std::endl;
		exit(1);
	}
	else
	{
		length = srcFile.tellg();
		srcFile.seekg (0, std::ios::beg);
	}
	

	size_t streamSize;
	//PERFORMANCE MEASUREMENT TIME START
	auto t1 = std::chrono::high_resolution_clock::now();
	//MAIN ENCRYPTION BLOCK BEGINS
	while( srcFile.read(buffer, this->BLOCK_SIZE) )
	{
		//Get character count
    	streamSize = srcFile.gcount();
		pos += streamSize;
		//std::cout << "Read: " << streamSize << " bytes" << std::endl;
		this->encrypt(buffer, streamSize);
		outputFile.write(buffer, streamSize);
		printf(BOLDCYAN "\rProcessing file: %.2f%s done" RESET, (pos/length) * 100.0, "%" );
	}

	//read remaining data
	streamSize = srcFile.gcount();
	std::cout << "\nRemaining bytes: " << streamSize << std::endl;
	if (streamSize)
	{
		srcFile.read(buffer, streamSize);
		this->encrypt(buffer, streamSize);
		outputFile.write(buffer, streamSize);
	}
	//PERFORMANCE MEASUREMENT TIME ENDS
	auto t2 = std::chrono::high_resolution_clock::now();

	srcFile.close();
	outputFile.close();

	std::cout << "Generated " << this->PRNG.getGenBits() << " random bits" << std::endl;
	std::cout << "P: " << this->PRNG.getP() << std::endl;
	std::cout << "Q: " << this->PRNG.getQ() << std::endl;
	auto execTimeNanoSecs = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
	auto execTimeSecs = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
	std::cout << "Computation took: " << execTimeNanoSecs << " nano seconds" << std::endl;
	std::cout << "Computation took: " << execTimeSecs << " seconds" << std::endl;
}

OTPI::~OTPI()
{
}
