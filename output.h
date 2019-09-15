#ifndef OUTPUT_H
#define OUTPUT_H

#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cctype>

#include "commands_handler.h"
#include "commands_block.h"

class Output
{
public:
  virtual ~Output() = default;
  virtual void update(const CommandsBlock &commands_blockm) = 0;
};

class DisplayOutput : public Output
{
public:
  virtual ~DisplayOutput() = default;
  void update(const CommandsBlock &commands_block)
  {
    std::cout << "bulk: " << commands_block.get_commands_string() << std::endl;
  }
};

class FileOutput : public Output
{
public:
  virtual ~FileOutput() = default;
  void update(const CommandsBlock &commands_block)
  {
     std::ofstream bulk_file;
     bulk_file.open(commands_block.get_first_command_timestamp() + "_" + std::to_string(commands_block.get_unique_file_id()) + ".log");
     bulk_file << commands_block.get_commands_string() << std::endl;
     bulk_file.close();
  }
};

#endif