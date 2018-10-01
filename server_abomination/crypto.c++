#include "crypto.h"

#include <string>
#include <cstdio>
#include <memory>
using namespace std;

#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#include "cgipp_exception.h"
#include "encodings.h"

namespace
{
    string hex_encoded (const unsigned char * s, unsigned int len);
    string hash (const string & text, const EVP_MD * evp_md);
}


namespace cgipp
{

RSA_key::~RSA_key()
{
    delete [] output;
    RSA_free (rsa);
}


RSA_public_key::RSA_public_key (const string & PEM_file)
{
    FILE * file = fopen (PEM_file.c_str(), "r");
    if (file == NULL)
    {
        throw cgipp_exception ("While loading public-key: could not open file " + PEM_file);
    }

    rsa = PEM_read_RSA_PUBKEY (file, NULL, NULL, NULL);

    if (rsa == NULL)
    {
        throw cgipp_exception ("Could not read public-key from file " + PEM_file);
    }

    d_rsa_size = RSA_size(rsa);
    if (rsa_size() <= padding_length)
    {
        throw cgipp_exception ("While loading public-key: RSA size less than padding length");
    }

    output = new unsigned char [d_rsa_size];
}


RSA_private_key::RSA_private_key (const string & PEM_file)
{
    FILE * file = fopen (PEM_file.c_str(), "r");
    if (file == NULL)
    {
        throw cgipp_exception ("While loading private-key: could not open file " + PEM_file);
    }

    rsa = PEM_read_RSAPrivateKey (file, NULL, NULL, NULL);

    if (rsa == NULL)
    {
        throw cgipp_exception ("Could not read private-key from file " + PEM_file);
    }

    d_rsa_size = RSA_size(rsa);
    if (rsa_size() <= padding_length)
    {
        throw cgipp_exception ("While loading private-key: RSA size less than padding length");
    }

    output = new unsigned char [d_rsa_size];
}


Ciphertext RSA_public_key::encrypt (const string & plaintext) const
{
    if (plaintext.length() >= static_cast<string::size_type>(rsa_size() - padding_length))
    {
        throw cgipp_exception ("RSA public-key encryption: plaintext exceeds maximum length "
                               "(must be less than RSA size - padding length)");
    }

    RSA_public_encrypt (plaintext.length(), reinterpret_cast<const unsigned char *>(plaintext.c_str()),
                        output,
                        rsa, RSA_PKCS1_OAEP_PADDING);

    return Ciphertext (output, output + d_rsa_size);
}


string RSA_public_key::decrypt (const string & hex_encoded_ciphertext) const
{
    const string & ciphertext = hex_decoded (hex_encoded_ciphertext);
    return decrypt (Ciphertext (ciphertext.begin(), ciphertext.end()));
}


string RSA_public_key::decrypt (const Ciphertext & ciphertext) const
{
    if (ciphertext.length() != static_cast<Ciphertext::size_type>(d_rsa_size))
    {
        throw cgipp_exception ("RSA public-key decryption: invalid ciphertext (incorrect size)");
    }

    const int length = RSA_public_decrypt (ciphertext.length(), ciphertext.c_str(),
                                           output,
                                           rsa, RSA_PKCS1_PADDING);

    return string (output, output + length);
}


Ciphertext RSA_private_key::encrypt (const string & plaintext) const
{
    if (plaintext.length() >= static_cast<string::size_type>(rsa_size() - padding_length))
    {
        throw cgipp_exception ("RSA public-key encryption: plaintext exceeds maximum length "
                               "(must be less than RSA size - padding length)");
    }

    RSA_private_encrypt (plaintext.length(), reinterpret_cast<const unsigned char *>(plaintext.c_str()),
                        output,
                        rsa, RSA_PKCS1_PADDING);

    return Ciphertext (output, output + d_rsa_size);
}


string RSA_private_key::decrypt (const string & hex_encoded_ciphertext) const
{
    const string & ciphertext = hex_decoded (hex_encoded_ciphertext);
    return decrypt (Ciphertext (ciphertext.begin(), ciphertext.end()));
}


string RSA_private_key::decrypt (const Ciphertext & ciphertext) const
{
    if (ciphertext.length() != static_cast<Ciphertext::size_type>(d_rsa_size))
    {
        throw cgipp_exception ("RSA private-key decryption: invalid ciphertext (incorrect size)");
    }

    const int length = RSA_private_decrypt (ciphertext.length(), ciphertext.c_str(),
                                            output,
                                            rsa, RSA_PKCS1_OAEP_PADDING);

    return string (output, output + length);
}


string hex_encoded (const Ciphertext & ciphertext)
{
    return ::hex_encoded (ciphertext.c_str(), ciphertext.length());
}



//******************** Symmetric encryption **********************

const EVP_CIPHER * evp_cipher (Encryption_algorithm cipher)
{
    switch (cipher)
    {
        case AES128_CBC:
            return EVP_aes_128_cbc();

        case AES256_CBC:
            return EVP_aes_256_cbc();

        case DES3_CBC:
            return EVP_des_ede3_cbc();

        case BF_CBC:
            return EVP_bf_cbc();
    }

    return EVP_aes_256_cbc();   // Dummy, just to avoid warning
}


Base_cipher::Base_cipher (const EVP_CIPHER * cipher, const string & key, const string & iv)
    : d_cipher (cipher),
      d_key (key.begin(), key.end()),
      d_iv (iv.begin(), iv.end())
{}

Ciphertext Base_cipher::encrypt (const string & plaintext) const
{
    auto_ptr<unsigned char> ciphertext (new unsigned char [plaintext.length() + EVP_MAX_KEY_LENGTH]);
    int ctlen, extlen;
    EVP_CIPHER_CTX ctx;

    EVP_EncryptInit (&ctx, d_cipher, d_key.c_str(), d_iv.c_str());
    EVP_EncryptUpdate (&ctx, ciphertext.get(), &ctlen,
                       reinterpret_cast<const unsigned char *>(plaintext.c_str()), plaintext.length());
    EVP_EncryptFinal (&ctx, ciphertext.get() + ctlen, &extlen);

    return Ciphertext(ciphertext.get(), ciphertext.get() + ctlen + extlen);
}

string Base_cipher::decrypt (const string & hex_encoded_ciphertext) const
{
    const string & decoded = hex_decoded (hex_encoded_ciphertext);
    return decrypt (Ciphertext (decoded.begin(), decoded.end()));
}

string Base_cipher::decrypt (const Ciphertext & ciphertext) const
{
    auto_ptr<unsigned char> decrypted (new unsigned char [ciphertext.length()]);
    int dlen, extlen;
    EVP_CIPHER_CTX ctx;

    EVP_DecryptInit (&ctx, d_cipher, d_key.c_str(), d_iv.c_str());
    EVP_DecryptUpdate (&ctx, decrypted.get(), &dlen, ciphertext.c_str(), ciphertext.length());
    EVP_DecryptFinal (&ctx, decrypted.get() + dlen, &extlen);

    return string (decrypted.get(), decrypted.get() + dlen + extlen);
}




//************* Hashes (inline forwarder functions) **************

string md5 (const string & text)
{
    return hash (text, EVP_md5());
}

string sha1 (const string & text)
{
    return hash (text, EVP_sha1());
}

string sha224 (const string & text)
{
    return hash (text, EVP_sha224());
}

string sha256 (const string & text)
{
    return hash (text, EVP_sha256());
}

string sha384 (const string & text)
{
    return hash (text, EVP_sha384());
}

string sha512 (const string & text)
{
    return hash (text, EVP_sha512());
}

}   // namespace cgipp



namespace
{
    string hex_encoded (const unsigned char * s, unsigned int len)
    {
        const char * const hex_digits = "0123456789abcdef";

        string encoded;
        encoded.reserve (2*len);
        for (unsigned int i = 0; i < len; i++)
        {
            encoded += hex_digits[s[i]/16];
            encoded += hex_digits[s[i]&0xF];
        }

        return encoded;
    }

    string hash (const string & text, const EVP_MD * evp_md)
    {
        unsigned char hash[EVP_MAX_MD_SIZE];

        EVP_MD_CTX ctx;
        unsigned int mdlen;

        EVP_DigestInit (&ctx, evp_md);
        EVP_DigestUpdate (&ctx, reinterpret_cast<const unsigned char *>(text.c_str()), text.length());
        EVP_DigestFinal (&ctx, hash, &mdlen);

        return hex_encoded (hash, mdlen);
    }

} // unnamed namespace
