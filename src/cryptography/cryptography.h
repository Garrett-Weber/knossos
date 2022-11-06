#ifndef CRYPTOGRAPHY_H
#define CRYPTOGRAPHY_H

#include <cryptopp/osrng.h>
#include <cryptopp/integer.h>
#include <cryptopp/nbtheory.h>
#include <cryptopp/dh.h>
#include <cryptopp/modes.h>
#include <cryptopp/aes.h>
#include <cryptopp/base64.h>

class Cryptography
{
public:
  Cryptography();
  void generate_aes(std::string, std::string = "");
  std::string encrypt(std::string);
  std::string decrypt(std::string);

  std::string get_private();
  std::string get_public();
  std::string get_aes_key();
  std::string get_aes_iv();
  std::string get_shared();
  size_t e_len, d_len;

private:
  CryptoPP::AutoSeededRandomPool rnd;
  CryptoPP::Integer decoder;
  CryptoPP::DH dh_domain;
  CryptoPP::SecByteBlock public_key;
  CryptoPP::SecByteBlock private_key;
  CryptoPP::SecByteBlock shared_key;
  CryptoPP::SecByteBlock other_public_key;
  CryptoPP::SecByteBlock aes_iv;
  CryptoPP::SecByteBlock aes_key;
  CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption encryption;
  CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption decryption;

  std::string base64_encode(CryptoPP::SecByteBlock);
  CryptoPP::SecByteBlock base64_decode(std::string);
};

#endif /* CRYPTOGRAPHY_H */