/*
 * ecdsa.cpp
 *
 *  Created on: 09/08/2018
 *      Author: manue
 */


// using figures on: https://en.bitcoin.it/wiki/Technical_background_of_version_1_Bitcoin_addresses
// gcc -Wall ecdsapubkey.c -o ecdsapubkey -lcrypto
#include "ecdsa.h"
#include <stdio.h>
#include <stdlib.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <openssl/bn.h>
#include <iostream>
#include <string>

ecdsa::ecdsa() {

}

ecdsa::getpubkey(std::string *privKey)
{
     EC_KEY *eckey = NULL;
     EC_POINT *pub_key = NULL;
     const EC_GROUP *group = NULL;
     BIGNUM start;
     BIGNUM *res;
     BN_CTX *ctx;

     BN_init(&start);
     ctx = BN_CTX_new(); // ctx is an optional buffer to save time from allocating and deallocating memory whenever required

     res = &start;
//     BN_hex2bn(&res,"3D79F601620A6D05DB7FED883AB8BCD08A9101B166BC60166869DA5FC08D936E");
   //  BN_hex2bn(&res,"18E14A7B6A307F426A94F8114701E7C8E774E7F9A47E2C2035DB29A206321725");
       BN_hex2bn(&res, privKey);

     eckey = EC_KEY_new_by_curve_name(NID_secp256k1);
     group = EC_KEY_get0_group(eckey);
     pub_key = EC_POINT_new(group);

     EC_KEY_set_private_key(eckey, res);

     /* pub_key is a new uninitialized `EC_POINT*`.  priv_key res is a `BIGNUM*`. */
     if (!EC_POINT_mul(group, pub_key, res, NULL, NULL, ctx))
       printf("Error at EC_POINT_mul.\n");

//     assert(EC_POINT_bn2point(group, &res, pub_key, ctx)); // Null here

     EC_KEY_set_public_key(eckey, *pub_key);

     char *cc = EC_POINT_point2hex(group, pub_key, 4, ctx);

     char *c=cc;

     int i;
    std::string pubkey;
     for (i=0; i<130; i++) // 1 byte 0x42, 32 bytes for X coordinate, 32 bytes for Y coordinate
     {
    	 pubkey+=*c++;

     }
      return pubkey;
 }


ecdsa::encrypt(std::string *filename,std::string *privkey)
{
std::string pubkey=this->getpubkey(privkey);

string plain = privkey.toStdString();
    string ciphertext;
    // Hex decode symmetric key:
    HexDecoder decoder;
    decoder.Put( (byte *)PRIVATE_KEY, 32*2 );
    decoder.MessageEnd();
    word64 size = decoder.MaxRetrievable();
    char *decodedKey = new char[size];
    decoder.Get((byte *)decodedKey, size);
    // Generate Cipher, Key, and CBC
    byte key[ AES::MAX_KEYLENGTH ], iv[ AES::BLOCKSIZE ];
    StringSource( reinterpret_cast<const char *>(decodedKey), true,
                  new HashFilter(*(new SHA256), new ArraySink(key, AES::MAX_KEYLENGTH)) );
    memset( iv, 0x00, AES::BLOCKSIZE );
    CBC_Mode<AES>::Encryption Encryptor( key, sizeof(key), iv );
    StringSource( plain, true, new StreamTransformationFilter( Encryptor,
                  new HexEncoder(new StringSink( ciphertext ) ) ) );
    return QString::fromStdString(ciphertext);



}

ecdsa::decrypt(std::string *filename,std::string *privkey)
{


}


}
