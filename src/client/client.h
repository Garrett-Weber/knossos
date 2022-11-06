#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <utility>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Client
{
public:
  Client(boost::asio::io_context &, std::string, short);

private:
  void do_connect();
  tcp::endpoint endpoint;
  tcp::socket socket;
  boost::system::error_code ec;
};

#endif /* CLIENT_H */