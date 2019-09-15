#ifndef COMMANDS_BLOCK_H
#define COMMANDS_BLOCK_H

#include <string>
#include <chrono>

class CommandsBlock 
{
public:
      CommandsBlock(size_t brackets_counter = 0) :
        _brackets_counter(brackets_counter)
    {
     
    }

    std::string get_first_command_timestamp() const {
      auto duration = _first_command_time.time_since_epoch();
      return std::to_string(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
    }

    void add_command(std::string command_line)
    {
      if(is_first_command()) {
        _first_command_time = std::chrono::system_clock::now();
      }
      _commands.emplace_back(command_line);
    }

    auto get_commands() const  {
      return _commands; 
    }

    std::string get_commands_string() const
    {
      std::ostringstream commands_line;
      if (!_commands.empty())
      {
        std::copy(_commands.begin(), _commands.end() - 1,
                 std::ostream_iterator<std::string>(commands_line, " "));
        commands_line << _commands.back();
      }
      return commands_line.str();
    }

    std::size_t get_commands_size() const 
    {
      return _commands.size();
    }

    void set_unique_file_id(u_int unique_file_id)
    {
      _unique_file_id =  unique_file_id;
    }

    u_int get_unique_file_id() const 
    {
      return _unique_file_id;
    }

    bool is_block_processing() const 
    {
      return _brackets_counter > 0;
    }

    bool is_first_command() const 
    {
      return _commands.size() == 0;
    }

    void clear_commands()
    {
      _commands.clear();
    }

    void increase_brackets_counter() {
      _brackets_counter++;
    }

    void decrease_brackets_counter() {
      _brackets_counter--;
    }

    size_t get_brackets_counter() const {
      return _brackets_counter;
    }

private:
    std::vector<std::string> _commands;
    std::chrono::time_point<std::chrono::system_clock> _first_command_time;
    size_t _brackets_counter;
    u_int _unique_file_id;
};

#endif