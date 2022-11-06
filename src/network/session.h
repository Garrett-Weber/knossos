#ifndef SESSION_H
#define SESSION_H

#include <iostream>
#include <string>
#include <thread>

#include <boost/asio.hpp>
#include <boost/chrono.hpp>

#include "../cryptography/cryptography.h"

using boost::asio::ip::tcp;

class Session
    : public std::enable_shared_from_this<Session>
{
public:
  virtual void Entry() = 0;

protected:
  Session(tcp::socket socket);
  void do_read();
  void do_eread();
  void do_write(std::string_view write_data);
  void do_ewrite(std::string write_data);

  bool is_message_queued();
  bool is_socket_timeout();

  tcp::socket socket;
  boost::system::error_code ec;
  boost::chrono::system_clock::time_point last_read;
  std::string data;
  boost::asio::streambuf read_buf;
  Cryptography crypto;
  const std::string endreq;
  const std::string expected_inital;
  const std::string expected_response;
  const size_t max_timeout;

private:
  void read();

  void write(std::string_view write_data);
};

#endif /* SESSION_H */