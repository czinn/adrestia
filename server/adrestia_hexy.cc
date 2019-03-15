/* Cryptography and ancillary functions */

// Us
#include "adrestia_hexy.h"

#include "logger.h"

// Crypto modules
#include <openssl/evp.h>

// System modules
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <random>
using namespace std;

thread_local mt19937 rng;

void adrestia_hexy::reseed() {
	ifstream urandom("/dev/urandom", ios::in|ios::binary);
	size_t seed;
	urandom.read(reinterpret_cast<char*>(&seed), sizeof(seed));
	rng.seed(seed);
}

void adrestia_hexy::print_hexy(const char* not_hexy, int length) {
	/* @brief Outputs to cout the hexadecimal form of bytes contained in not_hexy. */

	stringstream ss;
	ss << "HEXY: |";
	for (int i = 0; i < length; i += 1) {
		ss << ":";
		ss << hex << (int)not_hexy[i];
		ss << ";";
	}
	ss << "|";
	cout << ss.str();
}


string adrestia_hexy::hex_urandom(size_t length) {
	/* Creates and returns random hex string of the requested length. */
	const static char* hex_chars = "0123456789abcdef";
	string result;
	for (size_t i = 0; i < length; i++) {
		result += hex_chars[rng() % 16];
	}
	return result;
}


string adrestia_hexy::random_dec_string(size_t length) {
	string result;
	for (size_t i = 0; i < length; i++) {
		result += rng() % 10 + '0';
	}
	return result;
}


void adrestia_hexy::digest_message(const char* message,
	size_t message_length,
	unsigned char** digest_returncarrier,
	unsigned int* digest_length_returncarrier
) {
	/* @brief Produces a sha256 digest of the given message.
	 *
	 * @param message: The c-string message that should be hashed.
	 * @param message_length: The length of message.
	 * @param digest_returncarrier: A pointer to the c-string where the hashed message will be inserted.
	 * @param digest_length_returncarrier: A pointer to an unsigned integer where the length of the hashed message will
	 *        be inserted.
	 *
	 * @returns Nothing
	 */

	EVP_MD_CTX* mdctx;

	if ((mdctx = EVP_MD_CTX_new()) == nullptr) {
		cerr << "Indigestion type 1!" << endl;
		throw string("Indigestion type 1.");
	}
	if (1 != EVP_DigestInit_ex(mdctx, EVP_sha512(), nullptr)) {
		cerr << "Indigestion type 2!" << endl;
		throw string("Indigestion type 2.");
	}
	if (1 != EVP_DigestUpdate(mdctx, message, message_length)) {
		cerr << "Indigestion type 3!" << endl;
		throw string("Indigestion type 3.");
	}
	if ((*digest_returncarrier = (unsigned char*)OPENSSL_malloc(EVP_MD_size(EVP_sha512()))) == nullptr) {
		cerr << "Indigestion type 4!" << endl;
		throw string("Indigestion type 4.");
	}
	if (1 != EVP_DigestFinal_ex(mdctx, *digest_returncarrier, digest_length_returncarrier)) {
		cerr << "Indigestion type 5!" << endl;
		throw string("Indigestion type 5.");
	}

	EVP_MD_CTX_free(mdctx);
}
