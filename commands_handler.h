#ifndef COMMANDS_HANDLER_H
#define COMMANDS_HANDLER_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <string>

#include "commands_block.h"
#include "output.h"

class CommandsHandler
{
public:
  CommandsHandler(size_t bulk_size = 0) : _bulk_size(bulk_size)
  {
    _endpoints_blocks.insert(std::make_pair(_default_endpoint, CommandsBlock()));

    _outputs.push_back(std::make_unique<DisplayOutput>());
    _outputs.push_back(std::make_unique<FileOutput>());   
    unique_file_id = 0;
  }

  void add_input(const std::string &endpoint, const std::string &input_line)
  {     
      auto it = _endpoints_blocks.find(endpoint);

      switch (map_command(input_line))
      {
      case opening_bracket:
        if(it != _endpoints_blocks.end()) {
          it->second.increase_brackets_counter();
        }
        else {
          _endpoints_blocks.insert(std::make_pair(endpoint, CommandsBlock(1)));
        }        
        break;

      case closing_bracket:        
        if(it != _endpoints_blocks.end()) {
          it->second.decrease_brackets_counter();
          if(it->second.get_brackets_counter() == 0) {
            it->second.set_unique_file_id(get_unique_file_id());
            for(const auto& output : _outputs) {
              output->update(it->second);             
            }
            it->second.clear_commands();
            _endpoints_blocks.erase(endpoint);
          }
        }
        break;

      default:
        if(it != _endpoints_blocks.end() && it->second.is_block_processing()) {
          it->second.add_command(input_line);          
        }
        else {
          it = _endpoints_blocks.find(_default_endpoint);
          it->second.add_command(input_line);
          if (it->second.get_commands_size() >= _bulk_size)
          {
            it->second.set_unique_file_id(get_unique_file_id());
            for(const auto& output : _outputs) {
             output->update(it->second);             
            }
            it->second.clear_commands();
          } 
        }             
        break;
      }      
  }
  void close_input()
  {
    auto it = _endpoints_blocks.find(_default_endpoint);
    it->second.set_unique_file_id(get_unique_file_id());
    for (const auto &output : _outputs)
    {
      output->update(it->second);
    }
    it->second.clear_commands();
  }

  void set_bulk_size(size_t bulk_size)
  {
    _bulk_size = bulk_size;
  }

  u_int get_unique_file_id()  
  {
    return unique_file_id.exchange(++unique_file_id);
  }

  void print_endpoints_map() const
  {
    for(const auto &item : _endpoints_blocks) {
      std::cout << item.first << " - ";
      for(const auto &vec : item.second.get_commands()) {
        std::cout << vec << ", ";
      }
      std::cout << std::endl;
    }
  }

  std::string data;

private:
  uint32_t map_command(const std::string& command) const
  {
    if(command.front() == '{') return opening_bracket;
    if(command.front() == '}') return closing_bracket;
    return other_command;
  }

  size_t _bulk_size = 0;
  enum { opening_bracket = 0, closing_bracket = 1,  other_command = 2 };
  std::map<const std::string, CommandsBlock> _endpoints_blocks;
  const std::string _default_endpoint{"0.0.0.0:00000"};
  std::vector<std::unique_ptr<Output>> _outputs;
  std::atomic_uint unique_file_id;
};

#endif