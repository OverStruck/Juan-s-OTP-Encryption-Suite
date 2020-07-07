#include <iostream>
#include <string>
#include <gmp.h>
#include "OTPI.h"
#include <chrono>

//g++ -O3 main.cpp OTPI.cpp bbs.cpp print_uint128_t.cpp -std=c++1z -o encryptor -lgmp

using namespace std;

/*
	Check wheter a given integer number is a prime number
	using GMP's mpz_probab_prime_p function

	@param string number - the integer number to check
	@return answer - the possible answer to display to the user
*/
string isPrime(mpz_t& number)
{
    string answer = "No";
    int result = mpz_probab_prime_p(number, 50);
    if (result == 1) answer = "Probably yes";
    if (result == 2) answer = "Definitely yes";
    return answer;
}

/*
	Checks that the given Blum prime numbers P & Q are congruent to 3 (modulus 4)
	The results of this check are shown to the user

	@param string p - blum prime number p
	@param string q - blum prime number q
	@param string seed - the seed
*/

bool checkParams(const string p, const string q, const string seed)
{
	printf(CYAN "======================================================\n");
	printf("CHECKING P & Q...");
	printf(CYAN "\n======================================================\n" RESET);

    mpz_t gmpP, gmpQ, gmpSeed, gmpN, gmpPQ_GCD, gmpSN_GCD;

    mpz_inits(gmpP, gmpQ, gmpSeed, gmpN, gmpPQ_GCD, gmpSN_GCD, NULL);
    mpz_set_str(gmpP, p.c_str(), 10);
    mpz_set_str(gmpQ, q.c_str(), 10);
    mpz_set_str(gmpSeed, seed.c_str(), 10);
    mpz_mul(gmpN, gmpP, gmpQ);
    mpz_gcd(gmpPQ_GCD, gmpP, gmpQ);
    mpz_gcd(gmpSN_GCD, gmpSeed, gmpN);

	string isPprime = isPrime(gmpP);
	string isQprime = isPrime(gmpQ);
	unsigned long int pMod4 = mpz_fdiv_ui(gmpP, 4);
	unsigned long int qMod4 = mpz_fdiv_ui(gmpQ, 4);

    gmp_printf("P is: %d bits long\n", mpz_sizeinbase(gmpP, 2));
	gmp_printf("Q is: %d bits long\n", mpz_sizeinbase(gmpQ, 2));
	gmp_printf("N is: %d bits long\n", mpz_sizeinbase(gmpN, 2));
	gmp_printf("The modulus N = P * Q is: %Zd\n", gmpN);

    cout << "is P prime? " << isPprime  << endl;
    cout << "is Q prime? " << isQprime  << endl;
    gmp_printf("Greatest Common Demoninator of P & Q is: %Zd\n", gmpPQ_GCD);
    gmp_printf("Greatest Common Demoninator of S & N is: %Zd\n", gmpSN_GCD);
    printf("P mod 4 is: %lu\n",  pMod4);
    printf("Q mod 4 is: %lu\n",  qMod4);

	printf("Max (long long) in system: %lld\n", LLONG_MAX);
    printf("Max (unsgined long long) in system: %llu\n", ULLONG_MAX);

	printf(RED "======================================================\n");

	bool result = true;

	if (isPprime == "No" || isQprime == "No")
	{
		printf("P or Q are NOT prime numbers...\n");
		result = false;
	}
	if (mpz_cmp_d(gmpPQ_GCD, 1) != 0)
	{
		printf("The GCD of P & Q is NOT 1\n");
		result = false;
	}
	if (mpz_cmp_d(gmpSN_GCD, 1) != 0)
	{
		printf("The GCD of S & N is NOT 1\n");
		result = false;
	}
	if (pMod4 != 3)
	{
		printf("P is NOT congruent to 3 mod 4\n");
		result = false;
	}
	if (qMod4 != 3)
	{
		printf("Q is NOT congruent to 3 mod 4\n");
		result = false;
	}
	printf("======================================================\n" RESET);

	//clear
    mpz_clears(gmpP, gmpQ, gmpSeed, gmpN, gmpPQ_GCD, gmpSN_GCD, NULL);

	if (result)
	{
		printf(BOLDGREEN "ALL TEST OF P & Q PASSED" RESET);
	}
	else
	{
		printf(BOLDRED "CANNOT CONTINUE WITH ENCRYPTION\n" RESET);
		printf(RED "PLEASE SELECT APPROPIATE SECRETS P AND Q\n\n" RESET);
	}

	return result;
}

void encryptFile(const string p, const string q, const string seed)
{
	string inputFileName, outputFileName;
	cout << "Enter input file name: ";
	cin >> inputFileName;
	cout << "Enter output name: ";
	cin >> outputFileName;

	const string key[3] = {p, q, seed};
	OTPI encryptor(key);
	encryptor.encryptFile(inputFileName, outputFileName);
}

void decryptFile(const string p, const string q, const string seed)
{
	encryptFile(p, q, seed);
}

void encryptionMenu()
{
    int action;
	string p, q;
	cout << YELLOW << "======================================================\n"
	<< "Juan's OTP Encryption Suite Version 0.1\n"
	<< "======================================================" << RESET
	<< "\nEnter the Blum Prime \"p\" you want to use for encryption/decryption: ";
	cin >> p;
	cout << "Enter the Blum Prime \"q\" you want to use for encryption/decryption: ";
	cin >> q;
	if (checkParams(p, q, "4"))
	{
		cout << "\nChoose task:\n"
		<< "[1] Encrypt File\n[2] Decrypt File" << endl
		<< "Task: "; cin >> action;

		switch(action)
		{
			case 1:
				encryptFile(p, q, "4");
			break;
			case 2:
				decryptFile(p, q, "4");
			break;
			default:
				cerr << "Invalid option. Try again.";
		}
	}
}

int main()
{
	encryptionMenu();
    return 0;
}
