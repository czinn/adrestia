#ifndef CGIPP_CRYPTO_H
#define CGIPP_CRYPTO_H

#include <string>

#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/objects.h>

namespace cgipp
{

using std::string;
using std::basic_string;


string md5 (const string & text);
string sha1 (const string & text);
string sha224 (const string & text);
string sha256 (const string & text);
string sha384 (const string & text);
string sha512 (const string & text);


typedef std::basic_string<unsigned char> Ciphertext;

string hex_encoded (const Ciphertext & ciphertext);

class RSA_key
{
public:
    RSA_key() {}

    virtual Ciphertext encrypt (const string & plaintext) const = 0;
    virtual string decrypt (const string & hex_encoded_ciphertext) const = 0;
    virtual string decrypt (const Ciphertext & ciphertext) const = 0;

    virtual ~RSA_key();

    unsigned int rsa_size() const
    {
        return d_rsa_size;
    }

    static const unsigned int padding_length = 41;      // RSA_PKCS1_OAEP_PADDING

protected:
    RSA * rsa;
    int d_rsa_size;
    unsigned char * output;

private:
    RSA_key (const RSA_key &);
    RSA_key & operator= (const RSA_key &);
};


enum Signature_type
{
    SHA1        = NID_sha1,
    RIPEMD160   = NID_ripemd160,
    MD5         = NID_md5,
    MD5_SHA1    = NID_md5_sha1
};

class RSA_public_key : public RSA_key
{
public:
    RSA_public_key (const string & PEM_file);

    Ciphertext encrypt (const string & plaintext) const;
    bool verify (const Ciphertext & signature, Signature_type type = MD5_SHA1) const;   // TODO

        // Decryption not recommended --- consider using verify(), combined with RSA_private_key::signature()
    string decrypt (const string & hex_encoded_ciphertext) const;
    string decrypt (const Ciphertext & ciphertext) const;

};


class RSA_private_key : public RSA_key
{
public:
    RSA_private_key (const string & PEM_file);

    string decrypt (const string & hex_encoded_ciphertext) const;
    string decrypt (const Ciphertext & ciphertext) const;
    Ciphertext signature (const string & message, Signature_type type = MD5_SHA1) const;    // TODO

        // Encryption not recommended --- consider using sign, combined with RSA_public_key::verify()
    Ciphertext encrypt (const string & plaintext) const;
};


enum Encryption_algorithm
{
    AES128_CBC,
    AES256_CBC,
    DES3_CBC,
    BF_CBC
};

const EVP_CIPHER * evp_cipher (Encryption_algorithm cipher);


class Base_cipher
{
public:
    Base_cipher (const EVP_CIPHER * cipher, const string & key, const string & iv = string(EVP_MAX_IV_LENGTH, '\0'));

    Ciphertext encrypt (const string & plaintext) const;

    string decrypt (const string & hex_encoded_ciphertext) const;
    string decrypt (const Ciphertext & ciphertext) const;

private:
    const EVP_CIPHER * d_cipher;
    basic_string<unsigned char> d_key;
    basic_string<unsigned char> d_iv;
};

template <Encryption_algorithm cipher = AES256_CBC>
class Generic_cipher : public Base_cipher
{
public:
    Generic_cipher (const string & key, const string & iv = string(EVP_MAX_IV_LENGTH, '\0'))
        : Base_cipher (evp_cipher(cipher), key, iv)
    {}
};


typedef Generic_cipher<AES256_CBC> Cipher;

}   // namespace cgipp

#endif
