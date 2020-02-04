
#ifndef __CRYPTO_SERVER_H__
#define __CRYPTO_SERVER_H__

#include <string>
#include "../rijndael.h"
static const char* crypto_key = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static char DES3_KEY[24] = "zaqwsxcderfvbgtyhnmjdd";
static UINT8 AES_KEY[32] = "poiuytrewqqasdffghjkknbvcx.;pxs";

std::string Decrypt(const char* cipherText, const char* key = NULL);
#endif