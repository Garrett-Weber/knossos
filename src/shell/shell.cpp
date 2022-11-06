#include "shell.h"

Shell::Shell(const std::string shell_name)
    : shell_path(boost::process::search_path(shell_name)),
      ioctx(),
      shell_read(ioctx),
      shell_write(ioctx)
{
  shell = boost::process::child{
      shell_path,
      (boost::process::std_out & boost::process::std_err) > shell_read,
      boost::process::std_in < shell_write, ioctx};

  on_read = [this](const boost::system::error_code &ec, size_t n)
  {
    std::string line;
    if (ec)
    {
      throw boost::system::system_error(ec);
      return;
    }
    std::istream input_stream(&read_buf);
    std::getline(input_stream, line);
    data.append(line);
    data += '\n';
    begin_read();
  };

  on_write = [this](boost::system::error_code ec, size_t n)
  {
    if (ec)
    {
      throw boost::system::system_error(ec);
    }
  };

  begin_read();
}

std::string Shell::Run(const std::string cmd)
{
  std::string output;
  boost::process::ipstream run_out;
  boost::process::system(cmd, boost::process::std_out > run_out);
  run_out >> output;
  return output;
}

void Shell::poll()
{
  ioctx.poll();
  ioctx.restart();
}

bool Shell::can_read()
{
  return bool(data.size());
}

void Shell::operator<<(std::string_view write_data)
{
  boost::asio::async_write(shell_write, boost::asio::buffer(write_data.data(), write_data.size()), on_write);
}

void Shell::operator>>(std::string &output)
{
  output = std::move(data);
}

void Shell::begin_read()
{
  boost::asio::async_read_until(shell_read, read_buf, '\n', on_read);
}