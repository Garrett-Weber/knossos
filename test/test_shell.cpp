#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "../src/shell/shell.h"

#define BOOST_TEST_MODULE Shell Test
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(shell_run_command_test)
{
  std::string cmd{"echo Dolphins!"};
  std::string run_out = Shell::Run(cmd);
  BOOST_TEST(run_out == "Dolphins!");
}

BOOST_AUTO_TEST_CASE(shell_instantiate_test)
{
  Shell shell{"sh"};
}

BOOST_AUTO_TEST_CASE(shell_piped_command_test)
{
  std::string piped_out;
  Shell shell{"sh"};

  shell << "echo test\n";

  while (!shell.can_read())
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    shell.poll();
  }
  while (shell.can_read())
  {
    shell >> piped_out;
    BOOST_TEST(piped_out == "test\n");
  }
}