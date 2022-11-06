#ifndef CLIENTSESSION_H
#define CLIENTSESSION_H

#include <boost/asio.hpp>

#include "../network/session.h"

using boost::asio::ip::tcp;

class ClientSession
    : public Session
{
public:
  ClientSession(tcp::socket);
  void Entry();

private:
  inline void check_if_server();
  inline void do_handshake();
  inline void test_handshake();
  void loop_shell();
};

#endif /* CLIENTSESSION_H */