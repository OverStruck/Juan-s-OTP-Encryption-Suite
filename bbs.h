#include<iostream>
#include<gmp.h>
#include<stdint.h>

/*
	C++ Implementation of the Blum-Blum-Shub (BBS) Psudo Random Number Generator
*/
class BBS
{
private:
	mpz_t genBits;
	mpz_t p, q, n, i, x0, rExp, rExpI, decryptionExp;
	mpz_t carmichaelN;

	void carmichaelFunc();

public:
	BBS();
	~BBS();
	BBS(long p, long q, long seed);
	BBS(const char* p, const char* q, const char* seed);

	char* getGenBits();
	char* getP();
	char* getQ();

	uint8_t getRandByte();
	int getRandBit();
	
	void getNextRand(mpz_t &nextBit);
	void getNextRand_MT(mpz_t &nextBit);
	void getNextRand(mpz_t &randNum, mpz_t &index);

	void setParams(long p, long q, long seed);
	void setParams(const char* p, const char* q, const char* seed);

	void setP(const long p);
	void setQ(const long q);
	void setSeed(const long seed);
};
