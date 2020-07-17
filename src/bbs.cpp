#include "bbs.h"

//default constructor
BBS::BBS() {}

//set user key
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

BBS::BBS(const char* p, const char* q, const char* seed)
{
	this->setParams(p, q, seed);
}

/*
	Generate next pseudo random number
	@param &nextNum the next number

	Set nextNum to (base^exp) mod n
	i.e. if base = 100 and N = 133 then
	nextNum = 100^2 mod 133 = 25

	At the beginning, base = seed (i.e 4)
	The initial seed is set by the user
*/
void BBS::getNextRand(mpz_t &nextNum)
{
	//Set nextNum to (seed^exp) mod n
	mpz_powm(nextNum, this->x0, this->rExp, this->n);
	/*
		Save the generated psudo random number
		We will use this number to generate the next psudo random number
	*/
	mpz_set(this->x0, nextNum);
	//For whatever reason we're keeping track of how many psudo-random bits we generate
	//We extract 1 bit per number, hence increasing genBits by 1
	mpz_add_ui(this->genBits, this->genBits, 1);
}

/*
	Generate random number - prototype
*/
void BBS::getNextRand(mpz_t &randNum, mpz_t &index)
{
	//calculate new exponent
	//rExpI = 2^index mod carmichaelFun(n)
	mpz_powm(this->rExpI, this->rExp, index, this->carmichaelN);
	//Set randNum to (base^rExpI) mod n
	mpz_powm(randNum, this->x0, this->rExpI, this->n);
}

//Multithreaded function - prototype
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

/*
	Generate random bit
	This function extracts one bit per psudo random number
*/
int BBS::getRandBit()
{
	mpz_t nextBit;
	mpz_init(nextBit);
	//get psudo random number
	this->getNextRand(nextBit);
	//set nextBit = NexBit mod 2 to get bit
	mpz_powm_ui(nextBit, nextBit, 1, this->rExp);

	//gmp_printf("%Zd ", nextBit);

	//convert GMP data type to int
	int bit = mpz_get_si(nextBit);
	mpz_clear(nextBit);

	return bit;
}

/*
	Generate random byte
*/
uint8_t BBS::getRandByte()
{
	uint8_t randByte = 00000000;
	for (int i = 0; i < 8; i++)
	{
		//Bitwise OR assignment
		randByte |= (this->getRandBit() == 1) << (7 - i);
	}
	return randByte;
}

//Return number of generated bits
char* BBS::getGenBits()
{
	return mpz_get_str(NULL, 10, this->genBits);
}

//Return blum prime number P used in this generator
char* BBS::getP()
{
	return mpz_get_str(NULL, 10, this->p);
}

//Return blum prime number Q used in this generator
char* BBS::getQ()
{
	return mpz_get_str(NULL, 10, this->q);
}

//Carmichael Function
//defined as the smallest integer m such that k^m (mod n) = 1
//for all k relatively prime to n
//This function computes the result using the least common multiple (LCM) of all the factors of N (P and Q)
void BBS::carmichaelFunc()
{
	mpz_t gmpP, gmpQ;
    mpz_inits(gmpP, gmpQ, NULL);
	//substract 1 from P & Q
    mpz_sub_ui(gmpP, this->p, 1);
    mpz_sub_ui(gmpQ, this->q, 1);
	//GMP least common multiple function
    mpz_lcm(this->carmichaelN, gmpP, gmpQ);
	//gmp_printf("carmichaelN: %Zd\n", this->carmichaelN);
    mpz_clears(gmpP, gmpQ, NULL);
}

//Destructor - clear all GMP variables
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
