/*
 * base64.cpp
 *
 *  Created on: 03/11/2018
 *      Author: manue
 */

#include "base64.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <boost/regex.hpp>
base64::base64() {
	// TODO Auto-generated constructor stub


}

base64::~base64() {
	// TODO Auto-generated destructor stub
}


static const unsigned char from_base64[] = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62, 255,  62, 255,  63,
                                     52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255, 255, 255, 255, 255,
                                    255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
                                     15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255,  63,
                                    255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
                                     41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51, 255, 255, 255, 255, 255};




static const char to_base64[] =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";


typedef unsigned char BYTE;

static inline bool is_base64(BYTE c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64::encode(std::string filename)
{

	std::ifstream ifp (filename, std::ifstream::binary);

	std::vector<BYTE> v(100);

	std::string encodedData;
	while ( ifp.read(reinterpret_cast<char*>(v.data()), 100) )
	{
	   // Find out how many characters were actually read.
	   auto count = ifp.gcount();

	   // Use v up to count BTYEs.
		 encodedData += base64_encode(reinterpret_cast<BYTE*>(v.data()),count);
	}



	return encodedData;

}

std::vector<BYTE> base64::decode(std::string imgbase64)
{

	    return base64::base64_decode(imgbase64);
}



std::string base64::base64_encode(BYTE const* buf,unsigned int bufLen) {
    size_t ret_size = bufLen+2;

    ret_size = 4*ret_size/3;

    std::string ret;
    ret.reserve(ret_size);

    for (unsigned int i=0; i<ret_size/4; ++i)
    {
        size_t index = i*3;
        BYTE b3[3];
        b3[0] = buf[index+0];
        b3[1] = buf[index+1];
        b3[2] = buf[index+2];

        ret.push_back(to_base64[ ((b3[0] & 0xfc) >> 2) ]);
        ret.push_back(to_base64[ ((b3[0] & 0x03) << 4) + ((b3[1] & 0xf0) >> 4) ]);
        ret.push_back(to_base64[ ((b3[1] & 0x0f) << 2) + ((b3[2] & 0xc0) >> 6) ]);
        ret.push_back(to_base64[ ((b3[2] & 0x3f)) ]);
    }

    return ret;
}



std::vector<BYTE> base64::base64_decode(std::string encoded_string) {
    size_t encoded_size = encoded_string.size();
    std::vector<BYTE> ret;
    ret.reserve(3*encoded_size/4);
try {
    for (size_t i=0; i<encoded_size; i += 4)
    {
    	BYTE b4[4];
        b4[0] = from_base64[encoded_string[i+0]];
        b4[1] = from_base64[encoded_string[i+1]];
        b4[2] = from_base64[encoded_string[i+2]];
        b4[3] = from_base64[encoded_string[i+3]];

        BYTE b3[3];
        b3[0] = ((b4[0] & 0x3f) << 2) + ((b4[1] & 0x30) >> 4);
        b3[1] = ((b4[1] & 0x0f) << 4) + ((b4[2] & 0x3c) >> 2);
        b3[2] = ((b4[2] & 0x03) << 6) + ((b4[3] & 0x3f));

        ret.push_back(b3[0]);
        ret.push_back(b3[1]);
        ret.push_back(b3[2]);
    }



  LogPrintf("success: %s","decodebase64");
    return ret;
  }
  catch(std::exception& e) {
        //Other errors
		std::vector<BYTE> vempty;
		LogPrintf("exception decode: %s",e.what());
	  return vempty;
     }
}



bool base64::base64Validator(string encoded_string)
{
	string expr="^([A-Za-z0-9+/]{4})*([A-Za-z0-9+/]{4}|[A-Za-z0-9+/]{3}=|[A-Za-z0-9+/]{2}==)$";

	 return base64::regexValidate(expr,encoded_string);

}



bool base64::regexValidate(string expr, string teststring)
{
    boost::regex ex(expr);
    if ( boost::regex_match (teststring,ex) ) {

        return true;

    }
    return false;
}


