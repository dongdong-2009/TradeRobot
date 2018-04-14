#ifndef _Md5HmacSha256_h_
#define _Md5HmacSha256_h_

#include <string>

/* 
Example:
    string result = GetMd5HmacSha256("mykey", "abcd");
    cout << result << endl;
*/
std::string GetMd5HmacSha256(const char *key, const char *msg);

std::string GetMd5(const char *msg);

#endif