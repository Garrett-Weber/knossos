#include <iostream>
#include <string>
#include <stdexcept>

#include <cryptopp/osrng.h>
#include <cryptopp/integer.h>
#include <cryptopp/nbtheory.h>
#include <cryptopp/dh.h>

#include "../src/cryptography/cryptography.h"

#define BOOST_TEST_MODULE Cryptography Test
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(crypto_instantiate_test)
{
  Cryptography c;
}

BOOST_AUTO_TEST_CASE(shared_key_test)
{

  Cryptography crypto_client;
  Cryptography crypto_server;

  crypto_client.generate_aes(crypto_server.get_public());
  crypto_server.generate_aes(crypto_client.get_public(), crypto_client.get_aes_iv());

  BOOST_TEST(crypto_client.get_shared() == crypto_server.get_shared());
  BOOST_TEST(crypto_client.get_aes_key() == crypto_server.get_aes_key());
  BOOST_TEST(crypto_client.get_aes_iv() == crypto_server.get_aes_iv());
}

BOOST_AUTO_TEST_CASE(message_encrypt_test)
{
  Cryptography crypto_client;
  Cryptography crypto_server;

  crypto_client.generate_aes(crypto_server.get_public());
  crypto_server.generate_aes(crypto_client.get_public(), crypto_client.get_aes_iv());

  std::string message(1024, 'A');

  BOOST_TEST(message == crypto_client.decrypt(crypto_server.encrypt(message)));
  BOOST_TEST(message == crypto_server.decrypt(crypto_client.encrypt(message)));
}

// ptext -> aes encrypt -> base64 encode -> etext
// etext -> base64 decode -> aes decrypt -> ptext

// ptext == decrypt( decode( encode( encrypt( ptext ) ) ) )