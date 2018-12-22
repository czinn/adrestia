/* Cryptography and ancillary functions */

#pragma once


namespace adrestia_hexy {
	// Prints the given buffer as hexadecimal characters
	void print_hexy(const char* not_hexy, int length);


	// Produces a random hex string of the specified length
	string hex_urandom(size_t number_of_characters);


	// Produces a sha256 hash of the given message, and returns it via digest_returncarrier
	void digest_message(const char* message,
	                    size_t message_length,
	                    unsigned char** digest_returncarrier,
	                    unsigned int* digest_length_returncarrier,
	                   );
}
