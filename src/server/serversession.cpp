#include <iostream>
#include <string>
#include <algorithm>

#include "serversession.h"

ServerSession::ServerSession(tcp::socket socket)
    : Session(std::move(socket))
{
}

void ServerSession::Entry()
{
  do_handshake();
  test_handshake();
  start_shell();
  loop_shell();
}

inline void ServerSession::check_if_client()
{
  if (data == expected_inital)
    return;

  if (data.find("HTTP/") != std::string::npos)
  {
    do_write("HTTP/1.0 404 Not Found");
  }
  else
  {
    do_write("HTTP/1.0 400 Bad Request");
  }
  throw std::runtime_error{"Not Teapot Client"};
}

inline void ServerSession::do_handshake()
{
  do_read();
  check_if_client();
  do_write(expected_response);

  do_read();
  const std::string other_pub_key = data;
  do_read();

  crypto.generate_aes(other_pub_key, data);
  do_write(crypto.get_public());
}

inline void ServerSession::test_handshake()
{
  do_eread();
  std::string reversed_data = data;
  std::reverse(reversed_data.begin(), reversed_data.end());
  do_ewrite(reversed_data);
}

inline void ServerSession::start_shell()
{
  shell = std::make_unique<Shell>("sh");
}

void ServerSession::loop_shell()
{
  std::string shell_out;
  for (;;)
  {
    shell->poll();
    while (shell->can_read())
    {
      shell->operator>>(shell_out);
      do_ewrite(shell_out);
    }
    if (is_message_queued())
    {
      do_eread();
      shell->operator<<(data);
    }
    if (is_socket_timeout())
    {
      throw std::runtime_error("Socket Timeout");
    }
  }
}