#include "encodings.h"

#include "cgipp_exception.h"

namespace 
{
    const char * const Hex_and_base64_digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    const char * const Hex_digits = Hex_and_base64_digits;
    const char * const hex_digits = "0123456789abcdef";
    const char * const base64_digits = Hex_and_base64_digits + 10;

    int hex_val (char c);
    unsigned int base64_val (char c);
}

namespace cgipp
{

string url_encoded (const string & s)
{
    string encoded;
    encoded.reserve (s.length());

    for (string::size_type i = 0; i < s.length(); i++)
    {
        if (isalnum (s[i]))
        {
            encoded += s[i];
        }
        else    // special character -- replace with % followed by the ASCII value in HEX
        {
            const unsigned char c = static_cast<unsigned char>(s[i]);
            encoded += '%';
            encoded += Hex_digits[c/16];
            encoded += Hex_digits[c&0xF];
        }
    }

    return encoded;
}


string url_decoded (const string & s)
{
    string decoded;

    for (string::size_type i = 0; i < s.length(); /* incr inside the loop */)
    {
        if (s[i] == '%')
        {
            if (i+2 >= s.length())
            {
                throw cgipp_exception ("Invalid URL-encoded string");
            }
            const int code = 16 * hex_val(s[i+1]) + hex_val(s[i+2]);
            decoded += static_cast<char>(code);
            if (decoded.length() > 2 && decoded.substr(decoded.length() - 2) == "\r\n")
            {
                decoded.replace (decoded.length() - 2, 2, "\n");
            }
            i += 3;
        }
        else if (s[i] == '+')
        {
            decoded += ' ';
            ++i;
        }
        else
        {
            decoded += s[i];
            ++i;
        }
    }
    return decoded;
}


string url_encoded_spp (const string & s)
{
    string encoded;
    encoded.reserve (s.length());

    for (string::size_type i = 0; i < s.length(); i++)
    {
        if (isalnum (s[i]))
        {
            encoded += s[i];
        }
        else if (s[i] == ' ')
        {
            encoded += '+';
        }
        else    // special character -- replace with % followed by the ASCII value in HEX
        {
            const unsigned char c = static_cast<unsigned char>(s[i]);
            encoded += '%';
            encoded += Hex_digits[c/16];
            encoded += Hex_digits[c&0xF];
        }
    }

    return encoded;
}


string hex_encoded (const string & s)
{
    string encoded;
    encoded.reserve (2*s.length());
    for (string::size_type i = 0; i < s.length(); i++)
    {
        const unsigned char c = static_cast<unsigned char>(s[i]);
        encoded += hex_digits[c/16];
        encoded += hex_digits[c&0xF];
    }

    return encoded;
}

string hex_decoded (const string & s)
{
    string decoded;
    for (string::size_type i = 0; i < s.length(); i+=2)
    {
        if (i+1 >= s.length())
        {
            throw cgipp_exception ("Invalid HEX-encoded string");
        }
        const int code = 16 * hex_val(s[i]) + hex_val(s[i+1]);
        decoded += static_cast<char>(code);
    }

    return decoded;
}

string base64_encoded (const string & s)
{
    const string::size_type len = (s.length() + 2) / 3 * 4;
    string encoded (len, '=');

    string::size_type i = 0, e = 0;
    for ( ; i + 2 < s.length(); i+=3)
    {
        encoded[e++] = base64_digits[(s[i] >> 2) & 0x3F];
        encoded[e++] = base64_digits[(s[i] << 4) & 0x30 | (s[i+1] >> 4) & 0xF];
        encoded[e++] = base64_digits[(s[i+1] << 2) & 0x3C | (s[i+2] >> 6) & 3];
        encoded[e++] = base64_digits[s[i+2] & 0x3F];
    }

    if (i < s.length())
    {
        encoded[e++] = base64_digits[(s[i] >> 2) & 0x3F];
        char next = (s[i++] << 4) & 0x30;
        if (i < s.length())
        {
            next |= (s[i] >> 4) & 0xF;
            encoded[e++] = base64_digits[next];
            encoded[e++] = base64_digits[(s[i] << 2) & 0x3C];
            return encoded;
        }
        encoded[e++] = base64_digits[next];
    }

    return encoded;
}

string base64_decoded (const string & s)
{
    string cleaned = s;
    string::size_type pos = 0;
    while (pos < cleaned.length() && (pos = cleaned.find_first_not_of(base64_digits)) != string::npos)
    {
        cleaned.erase (pos,1);
    }

    string decoded;
    decoded.reserve (cleaned.length() * 3 / 4);

    for (string::size_type i = 0; i < cleaned.length(); i+=4)
    {
        if (i + 3 >= cleaned.length())
        {
            throw cgipp_exception ("While decoding base64:  Invalid input (not a multiple of 4 characters)");
        }

        const unsigned int block = base64_val(s[i]) << 18 
                                   | base64_val(s[i+1]) << 12
                                   | base64_val(s[i+2]) << 6
                                   | base64_val(s[i+3]);
        decoded += static_cast<char>((block >> 16) & 0xFF);
        decoded += static_cast<char>((block >> 8) & 0xFF);
        decoded += static_cast<char>((block) & 0xFF);
    }

    const string::size_type eq = cleaned.find ('=');
    if (eq != string::npos)
    {
        const string::size_type num_eq = cleaned.length() - eq;
        decoded.erase (decoded.length() - num_eq);
    }

    return decoded;
}

string mime_base64_encoded (const string & s)
{
    string added_cr = s;
    string::size_type pos = 0;
    while (pos < added_cr.length() && (pos = added_cr.find ('\n', pos)) != string::npos)
    {
        added_cr.insert (pos, 1, '\r');
        pos += 2;
    }

    const string & encoded = base64_encoded (added_cr);
    string mime = encoded.substr (0,76);

    for (string::size_type b = 76; b < encoded.length(); b+=76)
    {
        mime += "\r\n";
        mime += encoded.substr (b,76);
    }

    return mime;
}

string pem_base64_encoded (const string & s)
{
    const string & encoded = base64_encoded (s);
    string pem = encoded.substr (0,64);

    for (string::size_type b = 64; b < encoded.length(); b+=64)
    {
        pem += "\r\n";
        pem += encoded.substr (b,64);
    }

    return pem;
}

}   // namespace cgipp

namespace
{

int hex_val (char c)
{
    if ('0' <= c && c <= '9')
    {
        return c - '0';
    }
    else if ('A' <= c && c <= 'F')
    {
        return 10 + c - 'A';
    }
    else if ('a' <= c && c <= 'f')
    {
        return 10 + c - 'a';
    }
    else
    {
        throw cgipp::cgipp_exception ("Invalid HEX-encoded character");
    }
}

unsigned int base64_val (char c)
{
    if ('A' <= c && c <= 'Z')
    {
        return c - 'A';
    }
    else if ('a' <= c && c <= 'z')
    {
        return c - 'a' + 26;
    }
    else if ('0' <= c && c <= '9')
    {
        return c - '0' + 52;
    }
    else if (c == '+')
    {
        return 62;
    }
    else if (c == '/')
    {
        return 63;
    }
    else if (c == '=')
    {
        return 0;
    }
    else
    {
        return static_cast<unsigned int>(-1);
    }
}

}   // unnamed namespace
