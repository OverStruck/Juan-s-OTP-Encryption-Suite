#include "bbs.h"

//default constructor (unsafe key parameters)
BBS::BBS()
{
	this->setParams(7, 19, 4);
}

void BBS::setParams(const char* p, const char* q, const char* seed)
{
	mpz_init(this->p);
	mpz_init(this->q);
	mpz_init(this->n);
	mpz_init(this->i);				//
	mpz_init(this->x0);				//the seed
	mpz_init(this->rExp);			//raise exponent (the number 2)
	mpz_init(this->rExpI);			//raise exponent i (for multithreaded calculations)
	mpz_init(this->genBits);		//number of generated bits
	mpz_init(this->decryptionExp);	//decryption exponent
	mpz_init(this->carmichaelN);

	//two large prime numbers p and q such that gcd(p, q) = 1
	mpz_set_str(this->p, p, 10);
	mpz_set_str(this->q, q, 10);
	//random seed 2 < x0 < n-1
	mpz_set_str(this->x0, seed, 10);
	//modulo n, n = p * q
	mpz_mul(this->n, this->p, this->q);


	//decryption exponent
	//this->decryptionExp = ((this->p - 1) * (this->q - 1) + 4) / 8;
	//mpz_t number "2" used as raising exponent and modulo 2 for bit extraction
	mpz_set_si(this->rExp, 2);
	this->carmichaelFunc();
}

void BBS::setParams(long p, long q, long seed)
{
	mpz_init(this->p);
	mpz_init(this->q);
	mpz_init(this->n);
	mpz_init(this->i);				//
	mpz_init(this->x0);				//the seed
	mpz_init(this->rExp);			//raise exponent (the number 2)
	mpz_init(this->rExpI);			//raise exponent i (for multithreaded calculations)
	mpz_init(this->genBits);		//number of generated bits
	mpz_init(this->decryptionExp);	//decryption exponent
	mpz_init(this->carmichaelN);

	//two large prime numbers p and q such that gcd(p, q) = 1
	mpz_set_si(this->p, p);
	mpz_set_si(this->q, q);
	//random seed 2 < x0 < n-1
	mpz_set_si(this->x0, seed);
	//modulo n, n = p * q
	mpz_mul(this->n, this->p, this->q);
	//decryption exponent
	//this->decryptionExp = ((this->p - 1) * (this->q - 1) + 4) / 8;
	//mpz_t number "2" used as raising exponent and modulo 2 for bit extraction
	mpz_set_si(this->rExp, 2);
	this->carmichaelFunc();
}

BBS::BBS(const char* p, const char* q, const char* seed)
{
	this->setParams(p, q, seed);
}

BBS::BBS(long p, long q, long seed)
{
	this->setParams(p, q, seed);
}
/*
	Generate next pseudo random number
	@param &nextNum the next number
*/
void BBS::getNextRand(mpz_t &nextNum)
{
	//Set nextBit to (base^exp) mod n
	mpz_powm(nextNum, this->x0, this->rExp, this->n);
	mpz_set(this->x0, nextNum);
	mpz_add_ui(this->genBits, this->genBits, 1);
}

void BBS::getNextRand(mpz_t &randNum, mpz_t &index)
{
	//calculate new exponent
	//rExpI = 2^index mod carmichaelFun(n)
	mpz_powm(this->rExpI, this->rExp, index, this->carmichaelN);
	//Set randNum to (base^rExpI) mod n
	mpz_powm(randNum, this->x0, this->rExpI, this->n);
}

void BBS::getNextRand_MT(mpz_t &nextNum)
{
	//if we are not generating the first random number
	if ( mpz_cmp_si(this->genBits, 0) > 0 )
	{
		//increase index i to i + 4
		mpz_add_ui(this->i, this->i, 1);
	}
	//calculate new exponent
	//rExpI = 2^i mod carmichaelFun(n)
	mpz_powm(this->rExpI, this->rExp, this->i, this->carmichaelN);
	//Set nextNum to (base^rExpI) mod n
	mpz_powm(nextNum, this->x0, this->rExpI, this->n);
	//increase genBits count 
	mpz_add_ui(this->genBits, this->genBits, 1);
}

int BBS::getRandBit()
{
	mpz_t nextBit;
	mpz_init(nextBit);
	this->getNextRand(nextBit);
	//set nextBit = NexBit mod 2 to get bit
	mpz_powm_ui(nextBit, nextBit, 1, this->rExp);
	//gmp_printf("%Zd ", nextBit);
	int bit = mpz_get_si(nextBit);
	mpz_clear(nextBit);
	return bit;
}

uint8_t BBS::getRandByte()
{
	uint8_t randByte = 00000000;
	for (int i = 0; i < 8; i++)
	{
		randByte |= (this->getRandBit() == 1) << (7 - i);
	}
	return randByte;
}

char* BBS::getGenBits()
{
	return mpz_get_str(NULL, 10, this->genBits);
}

char* BBS::getP()
{
	return mpz_get_str(NULL, 10, this->p);
}

char* BBS::getQ()
{
	return mpz_get_str(NULL, 10, this->q);
}

//Carmichael Function
//defined as the smallest integer m such that k^m (mod n) = 1
//for all k relatively prime to n
void BBS::carmichaelFunc()
{
	mpz_t gmpP, gmpQ;
    mpz_inits(gmpP, gmpQ, NULL);
    mpz_sub_ui(gmpP, this->p, 1);
    mpz_sub_ui(gmpQ, this->q, 1);
    mpz_lcm(this->carmichaelN, gmpP, gmpQ);
	//gmp_printf("carmichaelN: %Zd\n", this->carmichaelN);
    mpz_clears(gmpP, gmpQ, NULL);
}

void BBS::setP(const long p) { mpz_set_si(this->p, p); }

void BBS::setQ(const long q) { mpz_set_si(this->q, q); }

void BBS::setSeed(const long seed) { mpz_set_si(this->x0, seed); }

BBS::~BBS()
{
	mpz_clear(this->p);
	mpz_clear(this->q);
	mpz_clear(this->n);
	mpz_clear(this->i);
	mpz_clear(this->x0);
	mpz_clear(this->rExp);
	mpz_clear(this->rExpI);
	mpz_clear(this->genBits);
	mpz_clear(this->decryptionExp);
	mpz_clear(this->carmichaelN);
}
