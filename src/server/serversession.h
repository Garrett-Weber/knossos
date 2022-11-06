#ifndef SERVERSESSION_H
#define SERVERSESSION_H

#include <boost/asio.hpp>

#include "../network/session.h"
#include "../shell/shell.h"

using boost::asio::ip::tcp;

class ServerSession
    : public Session
{
public:
  ServerSession(tcp::socket);
  void Entry();

private:
  std::unique_ptr<Shell> shell;
  inline void check_if_client();
  inline void do_handshake();
  inline void test_handshake();
  inline void start_shell();
  void loop_shell();
};

#endif /* SERVERSESSION_H */