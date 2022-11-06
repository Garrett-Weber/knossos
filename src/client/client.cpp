#include <iostream>
#include <string>
#include <utility>

#include <boost/asio.hpp>

#include "clientsession.h"
#include "client.h"

enum
{
  port = 1337
};

Client::Client(boost::asio::io_context &io_context, std::string ip_address_str, short port) : endpoint(boost::asio::ip::address_v4::from_string(ip_address_str), port),
                                                                                              socket(io_context)
{

  do_connect();
}

void Client::do_connect()
{
  socket.connect(endpoint, ec);
  if (ec)
  {
    throw std::runtime_error{std::string{"Error in connect "} + ec.message()};
  }
  try
  {
    std::make_shared<ClientSession>(std::move(socket))->Entry();
  }
  catch (std::runtime_error &e)
  {
    std::cerr << e.what() << std::endl;
  }
}

int main(int argc, char **argv)
{
  try
  {
    char host[] = "127.0.0.1";
    boost::asio::io_context io_context;

    Client c(io_context, host, port);
  }
  catch (std::exception &e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return EXIT_SUCCESS;
}
