/*
 * ecdsa.h
 *
 *  Created on: 09/08/2018
 *      Author: manue
 */

#ifndef QT_ECDSA_H
#define QT_ECDSA_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>


class ecdsa
{

public:
	explicit ecdsa();
	~ecdsa();
    //std::string getpubkey(std::string *privkey);
    void encrypt(std::string filename,std::string privkey, std::bool &status);
    void decrypt(std::string filename,std::string privkey, std::bool &status);


private:


};





#endif /* SRC_QT_ECDSA_H_ */
