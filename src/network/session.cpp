
#include <iostream>
#include <string>
#include <thread>

#include <boost/asio.hpp>
#include <boost/chrono.hpp>

#include "session.h"

Session::Session(tcp::socket socket)
    : socket(std::move(socket)),
      endreq("\r\n\r\n"),
      expected_inital("GET /server/teapot.asp HTTP/1.0"),
      expected_response("HTTP/1.0 418 I'm a teapot"),
      max_timeout(500)
{
}

void Session::do_read()
{
  read();
}

void Session::do_eread()
{
  read();
  data = crypto.decrypt(data);
}

void Session::do_write(std::string_view write_data)
{
  write(write_data);
  write(endreq);
}

void Session::do_ewrite(std::string write_data)
{
  write(crypto.encrypt(write_data));
  write(endreq);
}
bool Session::is_message_queued()
{
  boost::asio::socket_base::bytes_readable bytes_readable_cmd(true);
  socket.io_control(bytes_readable_cmd);
  return bool(bytes_readable_cmd.get());
}

bool Session::is_socket_timeout()
{
  if (boost::chrono::duration_cast<boost::chrono::seconds>(boost::chrono::system_clock::now() - last_read).count() > max_timeout)
  {
    return true;
  }
  return false;
}

void Session::read()
{
  boost::asio::read_until(socket, read_buf, endreq, ec);
  if (ec)
  {
    throw boost::system::system_error(ec);
  }
  std::istream input_stream(&read_buf);
  std::getline(input_stream, data);
  input_stream.ignore(2);
  data.pop_back();
  last_read = boost::chrono::system_clock::now();
}

void Session::write(std::string_view write_data)
{
  boost::asio::write(socket, boost::asio::buffer(write_data.data(), write_data.size()), boost::asio::transfer_all(), ec);
  if (ec)
  {
    throw std::runtime_error{"Error in Network Write: " + ec.message()};
  }
}