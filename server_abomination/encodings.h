#ifndef CGIPP_ENCODINGS_H
#define CGIPP_ENCODINGS_H

#include <string>

namespace cgipp
{

using std::string;


string url_encoded (const string & s);
string url_decoded (const string & s);

string url_encoded_spp (const string & s);
    // Special version, replacing SPace with Plus sign

string hex_encoded (const string & s);
string hex_decoded (const string & s);

string base64_encoded (const string & s);
string base64_decoded (const string & s);

string mime_base64_encoded (const string & s);
string pem_base64_encoded (const string & s);

}

#endif
