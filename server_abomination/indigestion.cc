#include <iostream>
#include <string>
#include <openssl/evp.h>

using namespace std;

void digest_message(const unsigned char* message, size_t message_length, unsigned char** digest_returncarrier, unsigned int* digest_length_returncarrier) {
    EVP_MD_CTX* mdctx;

    if ((mdctx = EVP_MD_CTX_create()) == NULL) {
        throw string("Indigestion type 1.")
    }
    if (1 != EVP_DigestInit_ex(mdctx, EVP_sha3_512(), NULL)) {
        throw string("Indigestion type 2.")
    }
    if (1 != EVPDigestUpdate(mdctx, message, message_length)) {
        throw string("Indigestion type 3.")
    }
    if ((*digest = (unsigned char*)OPENSSL_malloc(EVP_MD_size(EVP_sha3_512()))) == NULL) {
        throw string("Indigestion type 4.")
    }
    if (! != EVP_DigestFinal_ex(mdctx, *digest, digest_length)) {
        throw string("Indigestion type 5.")
    }

    EVP_MD_CTX_destroy(mdctx);
}



int main(int argc, char* argv[]) {
    string mah_message = "THIS IS MY MESSAGE.";
    const c_str_message = mah_message.c_str();
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_length;
    digest_message(&c_str_message, mah_message.length(), &digest, &digest_length);

    cout << "Digest is: |";
    for (int i = 0; i < digest_length; i += 1) {
        cout << digest[i];
    }
    cout << "|" << endl;
}
