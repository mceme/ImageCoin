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
    Q_OBJECT

public:

    std::string getpubkey(std::string *privkey);
    std::string encrypt(std::string *filename,std::string *privkey);
    std::string decrypt(std::string *filename,std::string *privkey);


private:


};





#endif /* SRC_QT_ECDSA_H_ */
