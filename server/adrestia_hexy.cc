/* Cryptography and ancillary functions */

// Us
#include "hexy.h"

// Crypto modules
#include <openssl/evp.h>

// System modules
#include <fstream>
#include <sstream>
#include <string>


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


string adrestia_hexy::hex_urandom(size_t number_of_characters) {
	/* Creates and returns random hex string of the requested length. */

	char proto_output[number_of_characters + 1];

	ifstream urandom("/dev/urandom", ios::in|ios::binary);

	if (!urandom) {
		cerr << "Failed to open urandom!\n";
		throw;
	}

	for (size_t i = 0; i < number_of_characters; i = i + 1) {
		char next_number = 0;

		urandom.read((char*)(&next_number), sizeof(char));
		next_number &= 0x0F;  // Ensure we generate only one character at a time.

		if (!urandom) {
			cerr << "Failed to read from urandom!\n";
			throw;
		}

		if (next_number < 10) {  // 0-10
			proto_output[i] = (char)(next_number + 48);
		}
		else {  // A-F
			proto_output[i] = (char)(next_number + 87);
		}
	}

	proto_output[number_of_characters] = '\0';

	string returnVar(proto_output);

	return returnVar;
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