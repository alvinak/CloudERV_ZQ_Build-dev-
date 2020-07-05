#pragma once

#include <string>

extern std::string g_sDefaultAESKey;
extern std::string aes_encrypt(std::string in, std::string key);
extern std::string aes_decrypt(std::string in, std::string key);
extern std::string base64_encode(const ::std::string &bindata);
extern std::string base64_decode(const std::string &ascdata);


