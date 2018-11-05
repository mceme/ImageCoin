/*
 * base64.h
 *
 *  Created on: 03/11/2018
 *      Author: manue
 */

#ifndef BITCOIN_QT_BASE64_H
#define BITCOIN_QT_BASE64_H

#include <vector>
#include <string>
#include <iostream>

class base64 {

	typedef unsigned char BYTE;

public:
	explicit base64();
	 ~base64();
	     std::string encode(std::string filename);
	     std::vector<BYTE> decode(std::string imgbase64);
	     std::string base64_encode(BYTE const* buf, unsigned int bufLen);
	     std::vector<BYTE> base64_decode(std::string encoded_string);
	     bool base64::base64Validator(string encoded_string)
	     bool base64::regexValidate (string expr, string teststring);
};

#endif /* SRC_QT_BASE64_H_ */
