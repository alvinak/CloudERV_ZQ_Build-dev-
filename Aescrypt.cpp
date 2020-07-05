#include "stdafx.h"

#include <string>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <cctype>

#include <openssl/aes.h>
#include <openssl/evp.h>

#pragma	comment(lib, "ssleay32.lib")
#pragma	comment(lib, "libeay32.lib")

static std::string g_sDefaultAESKey("#Xd234r(d d342do0$9874dDx.2");
static const char g_b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char g_reverse_table[128] = {
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
   52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
   64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
   64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
};

///////////////////////////////////////////////////////////////////////////////
// Encrypts a string using AES 256
// Note: If the key is less than 32 bytes, it will be null padded.
//       If the key is greater than 32 bytes, it will be truncated
// \param in The string to encrypt
// \param key The key to encrypt with
// \return The encrypted data
///////////////////////////////////////////////////////////////////////////////

std::string aes_encrypt(std::string in, std::string key)
{ 
	std::string strResult;
	unsigned char ivec[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	
	if (key.length() == 0)
		key = g_sDefaultAESKey;

	std::string ivecString((char*) ivec, 16); // Save this for later
 
    // Always pad the key to 32 bits.. because we can
    if (key.length() < 32){
		key.append(32 - key.length(), '\0');
    }
       
    // Get some space ready for the output
    unsigned char *output = new unsigned char[in.length() + AES_BLOCK_SIZE];
       
    // Encrypt the data
    int length = 0, finalLength = 0;
    EVP_CIPHER_CTX *encryptHandle = new EVP_CIPHER_CTX;
   
	EVP_CIPHER_CTX_init(encryptHandle);
    EVP_EncryptInit_ex(encryptHandle, EVP_aes_256_cbc(), NULL, (unsigned char*) key.c_str(), ivec);

    EVP_EncryptUpdate(encryptHandle, output, &length, (unsigned char*)in.c_str(), in.length());

    finalLength += length;
    EVP_EncryptFinal_ex(encryptHandle, output + length, &length);
    finalLength += length;
 
    // Make the data into a string
    std::string ret((char*) output, finalLength);
       
    // clean up
    delete output;

    EVP_CIPHER_CTX_cleanup(encryptHandle);
    
	delete encryptHandle;

	strResult = ivecString;
	strResult += ret;
       
    return strResult;
}

///////////////////////////////////////////////////////////////////////////////
// Decrypts a string using AES 256
// Note: If the key is less than 32 bytes, it will be null padded.
//       If the key is greater than 32 bytes, it will be truncated
// \param in The string to decrypt
// \param key The key to decrypt with
// \return The decrypted data
///////////////////////////////////////////////////////////////////////////////

std::string aes_decrypt(std::string in, std::string key)
{
	// Get the ivec from the front
    unsigned char ivec[16];

	if (key.length() == 0)
		key = g_sDefaultAESKey;

    strncpy((char*)ivec, in.c_str(), 16);
    in = in.substr(16);
 
    // Always pad the key to 32 bits.. because we can
    if(key.length() < 32){
		key.append(32 - key.length(), '\0');
    }
 
    // Create some space for output
    unsigned char *output = new unsigned char[in.length()];
    int length, finalLength = 0;
       
    // Decrypt the string
    EVP_CIPHER_CTX *encryptHandle = new EVP_CIPHER_CTX;
    EVP_CIPHER_CTX_init(encryptHandle);
    EVP_DecryptInit_ex(encryptHandle, EVP_aes_256_cbc(), NULL, (unsigned char*) key.c_str(), ivec);
    EVP_DecryptUpdate(encryptHandle, output, &length, (unsigned char*)in.c_str(), in.length());
    finalLength += length;
    EVP_DecryptFinal_ex(encryptHandle, output + length, &length);
    finalLength += length;
              
    // Make the output into a string
    std::string ret((char*) output, finalLength);
       
    // Clean up
    delete output;
    EVP_CIPHER_CTX_cleanup(encryptHandle);
    delete encryptHandle;
       
    return ret;
}
///////////////////////////////////////////////////////////////////////////////

std::string base64_encode(const ::std::string &bindata)
{
	using ::std::string;
	using ::std::numeric_limits;

   ///if (bindata.size() > (numeric_limits<string::size_type> ::max() / 4u) * 3u) {
///   throw ::std::length_error("Converting too large a string to base64.");
   ///}

   const std::size_t binlen = bindata.size();
   // Use = signs so the end is properly padded.
   string retval((((binlen + 2) / 3) * 4), '=');
   std::size_t outpos = 0;
   int bits_collected = 0;
   unsigned int accumulator = 0;
   const string::const_iterator binend = bindata.end();

   for (string::const_iterator i = bindata.begin(); i != binend; ++i) {
      accumulator = (accumulator << 8) | (*i & 0xffu);
      bits_collected += 8;
      while (bits_collected >= 6) {
         bits_collected -= 6;
         retval[outpos++] = g_b64_table[(accumulator >> bits_collected) & 0x3fu];
      }
   }
   if (bits_collected > 0) { // Any trailing bits that are missing.
      assert(bits_collected < 6);
      accumulator <<= 6 - bits_collected;
      retval[outpos++] = g_b64_table[accumulator & 0x3fu];
   }
   assert(outpos >= (retval.size() - 2));
   assert(outpos <= retval.size());
   return retval;
}

///////////////////////////////////////////////////////////////////////////////

std::string base64_decode(const std::string &ascdata)
{
   using std::string;
   string retval("");
   const string::const_iterator last = ascdata.end();
   int bits_collected = 0;
   unsigned int accumulator = 0;

   for (string::const_iterator i = ascdata.begin(); i != last; ++i) {
      const int c = *i;
      if (std::isspace(c) || c == '=') {
         // Skip whitespace and padding. Be liberal in what you accept.
         continue;
      }
      if ((c > 127) || (c < 0) || (g_reverse_table[c] > 63)) {
          throw ::std::invalid_argument("This contains characters not legal in a base64 encoded string.");
      }
      accumulator = (accumulator << 6) | g_reverse_table[c];
      bits_collected += 6;
      if (bits_collected >= 8) {
         bits_collected -= 8;
         retval += (char)((accumulator >> bits_collected) & 0xffu);
      }
   }
   return retval;
}
