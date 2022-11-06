#include <algorithm>

#include "clientsession.h"

using boost::asio::ip::tcp;

ClientSession::ClientSession(tcp::socket socket) : Session(std::move(socket))
{
}

void ClientSession::Entry()
{
  do_handshake();
  test_handshake();
  std::cout << "Secure channel established and functional" << '\n';
  loop_shell();
}

inline void ClientSession::check_if_server()
{
  if (data == expected_response)
    return;

  std::cerr << "Data: " << data << "\nExpected Data: " << expected_response << '\n';
  throw std::runtime_error{"Not Teapot Server"};
}

inline void ClientSession::do_handshake()
{
  do_write(expected_inital);
  do_read();
  check_if_server();
  std::cout << "We have a teapot!\nEstablishing secure channel...\n";

  do_write(crypto.get_public());
  do_write(crypto.get_aes_iv());

  do_read();
  crypto.generate_aes(data);
}

inline void ClientSession::test_handshake()
{
  const std::string rand_str{"evl3YvLjxETsfGqhSCSU"};
  do_ewrite(rand_str);
  do_eread();
  reverse(data.begin(), data.end());

  if (rand_str == data)
    return;

  throw std::runtime_error{"Handshake Failed"};
}

void ClientSession::loop_shell()
{
  size_t readsome_size, input_char_size = 0x1000;
  char input_char_ptr[input_char_size];
  std::ios_base::sync_with_stdio(false);
  for (;;)
  {
    while (is_message_queued())
    {
      do_eread();
      std::cout << data;
    }
    readsome_size = std::cin.readsome(input_char_ptr, input_char_size - 1);
    while (readsome_size)
    {
      input_char_ptr[readsome_size] = '\0';
      do_ewrite(input_char_ptr);
      readsome_size = std::cin.readsome(input_char_ptr, input_char_size - 1);
    }
  }
}