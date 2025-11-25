#include "crypto_utils.h"

#include <sstream>
#include <iomanip>

#include <fstream> 
#include <iostream> 

using namespace std; 

#if defined(USE_OPENSSL)
  #include <openssl/evp.h>
  #include <openssl/hmac.h>
  #include <openssl/sha.h>
  #include <openssl/pem.h>
  #include <openssl/err.h>
#endif

namespace dip :: security
{
    static string to_hex(const unsigned char *d, size_t n)
    {
        ostringstream ss; 
        ss << hex << setfill('0'); 
        for (size_t i = 0 ; i < n ; i++)
        ss << setw(2) << (int) d[i]; 
        return ss.str(); 
    }

    string sha256_hex(const string &data)
    {
        #if defined(USE_OPENSSL)
    unsigned char md[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)data.data(), data.size(), md);
    return to_hex(md, SHA256_DIGEST_LENGTH);
        #else
    // fallback: not secure, return simple ad-hoc hash (development only)
    unsigned int h = 2166136261u;
    for (char c : data) h = (h ^ (unsigned char)c) * 16777619u;
    ostringstream ss; ss << hex << h;
    return ss.str();
        #endif
    }

    string file_sha256_hex(const string &path)
    {
        ifstream f(path, ios :: binary); 
        if (!f.is_open())
        cout << "Error" << path << endl; 
        return {}; 

        string all({
            istreambuf_iterator<char>(f), istreambuf_iterator<char>()
        }); 
        return sha256_hex(all); 
    }

    string hmac_sha256_hex(const string &key, const string &data)
    {
        #if defined(USE_OPENSSL)
        unsigned int len = EVP_MAX_MD_SIZE;
        unsigned char md[EVP_MAX_MD_SIZE];
        HMAC(EVP_sha256(), key.data(), (int)key.size(), (const unsigned char*)data.data(), data.size(), md, &len);
        return to_hex(md, len);
        #else
        // fallback: not secure — use sha256 of key+data (dev only)
        return sha256_hex(key + data);
        #endif
    }

    bool verify_signature_pew(const string &pubkey_pem, const string &data, const string &sig_hex)
    {
        #if defined(USE_OPENSSL)
        // implement PEM -> EVP+PKEY -> verify using EVP>> 
        // (Omitted here for beverity ;  add real implementation when OpenSSL available).. 
        return false; 

        #else 
        (void) pubkey_pem ; 
        (void) data;
        (void) sig_hex; 
        return false; 

        #endif 
    }
} // namespace dip :: security.. 