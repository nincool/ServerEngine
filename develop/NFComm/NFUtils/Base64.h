#ifndef _TOOLS_BASE64_H
#define _TOOLS_BASE64_H

// BASE64����
int base64_decode(unsigned char* dst, const char* src, int inlen);

// BASE64����
int base64_encode(const void* inbuf, int inlen, char* outbuf);

#endif // _TOOLS_TEXTPROTOCOL_H
