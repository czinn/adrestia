/* Cryptography and ancillary functions */

#ifndef ADRESTIA_HEXY_INCLUDE_GUARD
#define ADRESTIA_HEXY_INCLUDE_GUARD

// Crypto modules
#include <openssl/evp.h>

// System modules
#include <string>

namespace adrestia_hexy {
	const unsigned int MAX_HASH_LENGTH = EVP_MAX_MD_SIZE;

	// Re-seed this thread's RNG with urandom.
	void reseed();

	// Prints the given buffer as hexadecimal characters
	void print_hexy(const char* not_hexy, int length);


	// Produces a random hex string of the specified length
	std::string hex_urandom(size_t number_of_characters);

	// Produces a random decimal string of length [length].
	std::string random_dec_string(size_t length);


	// Produces a sha256 hash of the given message, and returns it via digest_returncarrier
	void digest_message(const char* message,
	                    size_t message_length,
	                    unsigned char** digest_returncarrier,
	                    unsigned int* digest_length_returncarrier
	                   );
}

#endif
