//
//  base64 encoding and decoding with C++.
//  Version: 1.01.00
//

#ifndef BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A
#define BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A

#include <string>

std::string base64_encode(char const* bytes_to_encode, unsigned int len);
std::string base64_decode(const std::string & s);

#endif /* BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A */
