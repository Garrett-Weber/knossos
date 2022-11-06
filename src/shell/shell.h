#ifndef SHELL_H
#define SHELL_H

#include <iostream>
#include <string>
#include <vector>
#include <queue>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/process.hpp>

/* Outline:
 * Constructor: Open a shell process and keep a handle on it
 * Run(command) Run and command with the shell process so it keeps track - return stdout + stderr
 */

class Shell
{
public:
  Shell(const std::string);
  static std::string Run(const std::string);
  void poll();
  bool can_read();
  void operator<<(std::string_view);
  void operator>>(std::string &);

private:
  boost::asio::streambuf read_buf;
  std::string data;
  unsigned short read_queued;

  boost::filesystem::path shell_path;
  boost::asio::io_context ioctx;

  boost::process::async_pipe shell_read;
  boost::process::async_pipe shell_write;
  boost::process::child shell;
  std::function<void(const boost::system::error_code &ec, size_t n)> on_read;
  std::function<void(boost::system::error_code ec, size_t n)> on_write;

  void begin_read();
};

#endif /* SHELL_H */