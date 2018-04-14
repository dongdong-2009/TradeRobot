#include "Include/Foundation/Md5HmacSha256.h"

/* Openssl */
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>  

/* Base Type */
#include "Include/Type/BaseType.h"

/* Foundation */
#include "Include/Foundation/SystemInclude.h"

#if defined(_WIN32) && defined(_MSC_VER)
#	pragma comment(lib, "libcrypto-37.lib")
#endif

using namespace std;

/*
Python Codes:
    import hashlib
    import hmac
    privateKey = b"mykey";
    message    = b"abcd";

    md5prikey = hashlib.md5(privateKey).hexdigest();
    print(md5prikey);
    md5prikey=md5prikey.encode();
    sign = hmac.new(md5prikey, message, digestmod=hashlib.sha256).hexdigest();
    print(sign);

Example:
    string result = GetMd5HmacSha256("mykey", "abcd");
    cout << result << endl;

    Output: 7910f7038e4568a7b17a9401f920f4d8933380ffc5f0d753113faf0dd6ff24b2
*/
string GetMd5HmacSha256(const char *key, const char *msg)
{
    const uint_t HashLength = (SHA256_DIGEST_LENGTH > MD5_DIGEST_LENGTH ? SHA256_DIGEST_LENGTH : MD5_DIGEST_LENGTH);

    uchar_t hash[HashLength];
    uint_t  i;
    std::stringstream ss;

    //MD5_CTX  md5Ctx;
    //MD5_Init(&md5Ctx);
    //MD5_Update(&md5Ctx, (uchar_t*)key, strlen(key));
    //MD5_Final(sign, &md5Ctx);
    MD5((uchar_t*)key, strlen(key), hash);
    for  (i = 0; i < MD5_DIGEST_LENGTH; i++)
    { 
        ss << std::hex << std::setw(2) << std::setfill('0') << (unsigned int)hash[i];
    }

    //HMAC_CTX hmac;
    //HMAC_CTX_init(&hmac);
    //HMAC_Init_ex(&hmac, key, strlen(key), EVP_sha256(), NULL);
    //HMAC_Update(&hmac, (unsigned char*)str, strlen(str));
    //HMAC_Final(&hmac, hash, &len);
    //HMAC_CTX_cleanup(&hmac);
    uint_t len = HashLength;
    HMAC(EVP_sha256(), ss.str().c_str(), ss.str().length(), (unsigned char*)msg, strlen(msg), hash, &len);
    ss.str("");
    for(i = 0; i < len; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (unsigned int)hash[i];
    }

    return ss.str();
}

string GetMd5(const char *msg)
{
    uchar_t hash[MD5_DIGEST_LENGTH];
    //MD5_CTX  md5Ctx;
    //MD5_Init(&md5Ctx);
    //MD5_Update(&md5Ctx, (uchar_t*)key, strlen(key));
    //MD5_Final(sign, &md5Ctx);
    MD5((uchar_t*)msg, strlen(msg), hash);
    std::stringstream ss;
    for (uint_t i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (unsigned int)hash[i];
    }

    string sign(ss.str());

    transform(sign.begin(), sign.end(), sign.begin(), ::toupper);
    return sign;
}
