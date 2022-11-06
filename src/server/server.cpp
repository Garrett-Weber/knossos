#include "serversession.h"
#include "server.h"

enum
{
  port = 1337
};

Server::Server(boost::asio::io_context &io_context, short port) : acceptor(io_context, tcp::endpoint(tcp::v4(), port))
{
  do_accept();
}

void Server::do_accept()
{
  acceptor.async_accept(
      [this](boost::system::error_code ec, tcp::socket socket)
      {
        if (ec)
        {
          throw std::runtime_error{std::string{"Error in accept "} + ec.message()};
        }
        try
        {
          std::make_shared<ServerSession>(std::move(socket))->Entry();
        }
        catch (std::runtime_error &e)
        {
          std::cerr << e.what() << std::endl;
        }
        do_accept();
      });
}

int main()
{
  try
  {
    boost::asio::io_context io_context;

    Server s(io_context, port);

    io_context.run();
  }
  catch (std::exception &e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
}