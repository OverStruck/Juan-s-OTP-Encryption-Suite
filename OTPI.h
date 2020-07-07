#ifndef OTPI_H
#define OTPI_H

#include "bbs.h"
#include "terminalFontColors.h"
#include <string>
#include <fstream>
#include <chrono>

class OTPI
{
private:
	const int BLOCK_SIZE = 16;
	BBS PRNG;

public:
	OTPI();
	OTPI(const std::string key[3]);
	void RESETT(const std::string key[3]);
	
	void encrypt(char plainText[], int length);
	void encryptFile(const std::string inputFileName, const std::string outputFileName);

	~OTPI();
};

#endif
