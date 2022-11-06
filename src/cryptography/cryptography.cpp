#include <string>
#include <stdexcept>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "cryptography.h"

Cryptography::Cryptography()
{

  // Set Domain
  {
    CryptoPP::Integer p{
        "0xB10B8F96A080E01DDE92DE5EAE5D54EC52C99FBCFB06A3C6"
        "9A6A9DCA52D23B616073E28675A23D189838EF1E2EE652C0"
        "13ECB4AEA906112324975C3CD49B83BFACCBDD7D90C4BD70"
        "98488E9C219A73724EFFD6FAE5644738FAA31A4FF55BCCC0"
        "A151AF5F0DC8B4BD45BF37DF365C1A65E68CFDA76D4DA708"
        "DF1FB2BC2E4A4371"};

    CryptoPP::Integer g{
        "0xA4D1CBD5C3FD34126765A442EFB99905F8104DD258AC507F"
        "D6406CFF14266D31266FEA1E5C41564B777E690F5504F213"
        "160217B4B01B886A5E91547F9E2749F4D7FBD7D3B9A92EE1"
        "909D0D2263F80A76A6A24C087A091F531DBF0A0169B6A28A"
        "D662A4D18E73AFA32D779D5918D08BC8858F4DCEF97C2A24"
        "855E6EEB22B3B2E5"};

    CryptoPP::Integer q{"0xF518AA8781A8DF278ABA4E7D64B7CB9D49462353"};

    CryptoPP::Integer v = ModularExponentiation(g, q, p);
    if (v != CryptoPP::Integer::One())
      throw std::runtime_error{"Failed to verify order of the subgroup"};

    dh_domain = CryptoPP::DH{p, q, g};
  }

  // Generate Public/Private Keys
  {
    private_key.resize(dh_domain.PrivateKeyLength());
    public_key.resize(dh_domain.PublicKeyLength());
    dh_domain.GenerateKeyPair(rnd, private_key, public_key);
  }

  // Generate Aes Iv
  {
    aes_iv.resize(CryptoPP::AES::BLOCKSIZE);
    rnd.GenerateBlock(aes_iv.BytePtr(), aes_iv.size());
  }
}

void Cryptography::generate_aes(std::string other_public_key_str, std::string other_aes_iv_str /* = "" */)
{
  other_public_key = base64_decode(other_public_key_str);

  if (!other_aes_iv_str.empty())
  {
    aes_iv = base64_decode(other_aes_iv_str);
  }

  shared_key.resize(dh_domain.AgreedValueLength());
  if (!dh_domain.Agree(shared_key, private_key, other_public_key))
    throw std::runtime_error{"Failed to reach shared secret"};

  aes_key.resize(CryptoPP::SHA256::DIGESTSIZE);
  CryptoPP::SHA256().CalculateDigest(aes_key, shared_key, shared_key.size());

  encryption.SetKeyWithIV(aes_key.BytePtr(), aes_key.size(), aes_iv.BytePtr());
  decryption.SetKeyWithIV(aes_key.BytePtr(), aes_key.size(), aes_iv.BytePtr());
}

std::string Cryptography::encrypt(std::string plain_text)
{
  e_len += plain_text.size();
  std::string encrypted_text;
  CryptoPP::StringSource(plain_text, true,
                         new CryptoPP::StreamTransformationFilter(encryption,
                                                                  new CryptoPP::Base64Encoder(
                                                                      new CryptoPP::StringSink(encrypted_text), false)));
  return encrypted_text;
}

std::string Cryptography::decrypt(std::string encrypted_text)
{
  std::string plain_text;
  CryptoPP::StringSource(encrypted_text, true,
                         new CryptoPP::Base64Decoder(
                             new CryptoPP::StreamTransformationFilter(decryption,
                                                                      new CryptoPP::StringSink(plain_text))));
  d_len += plain_text.size();
  return plain_text;
}

std::string Cryptography::get_private()
{
  return base64_encode(private_key);
}

std::string Cryptography::get_public()
{
  return base64_encode(public_key);
}

std::string Cryptography::get_aes_key()
{
  return base64_encode(aes_key);
}

std::string Cryptography::get_aes_iv()
{
  return base64_encode(aes_iv);
}

std::string Cryptography::get_shared()
{
  return base64_encode(shared_key);
}

std::string Cryptography::base64_encode(CryptoPP::SecByteBlock text)
{
  std::string encoded_data;
  CryptoPP::StringSource(text, text.size(), true,
                         new CryptoPP::Base64Encoder(
                             new CryptoPP::StringSink(encoded_data)));

  boost::erase_all(encoded_data, "\n");
  return encoded_data;
}

CryptoPP::SecByteBlock Cryptography::base64_decode(std::string encoded_data)
{
  std::string text;
  CryptoPP::StringSource(encoded_data, true,
                         new CryptoPP::Base64Decoder(
                             new CryptoPP::StringSink(text)));
  return CryptoPP::SecByteBlock(reinterpret_cast<const CryptoPP::byte *>(&text[0]), text.size());
}