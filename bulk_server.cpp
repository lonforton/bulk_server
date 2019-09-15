#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#pragma GCC diagnostic ignored "-Wcast-function-type"
#include <boost/asio.hpp>

#include "commands_handler.h"

using boost::asio::ip::tcp;
namespace ba = boost::asio;
namespace bs = boost::system;

static CommandsHandler commands_handler;
static size_t connected_clients;

class bulk_session
  : public std::enable_shared_from_this<bulk_session>
{
public: 
  bulk_session(tcp::socket socket, size_t bulk_size)
    : _socket(std::move(socket))
    {
      ++connected_clients;
      commands_handler.set_bulk_size(bulk_size);      
      _current_endpoint = _socket.remote_endpoint().address().to_string() + ":" + std::to_string(_socket.remote_endpoint().port());
    }

  void start()
  {
     do_read_data();
  }

private:

   void do_read_data()
   {
     auto self(shared_from_this());
     
      ba::async_read_until(_socket, buf, '\n',
          [this, self](bs::error_code ec, std::size_t)
          {   
            if (!ec)
            {    
              std::istream is(&buf);
              std::string result_line;
              std::getline(is, result_line);  

              if(!result_line.empty() && result_line.back() == '\r') {    //removing \r for win files
                result_line.erase(result_line.size() - 1);
              }           
            
              commands_handler.add_input(_current_endpoint, result_line);
              
              do_read_data();
            }
            else
            {
              std::cerr << "connection stopped for " << _current_endpoint << ": " << ec.message() << std::endl;
              if(--connected_clients == 0) {
                commands_handler.close_input();
              }
            }
          });
   }

  std::string _current_endpoint;
  ba::streambuf buf;
  tcp::socket _socket;
};

class bulk_server
{
public:
  bulk_server(ba::io_context &io_context, const tcp::endpoint &endpoint, size_t bulk_size)
             : _acceptor(io_context, endpoint), _bulk_size(bulk_size)
  {
    do_accept();
  }

private:
  void do_accept()
  {
    _acceptor.async_accept(
         [this](bs::error_code ec, tcp::socket socket)
         {           
           std::cerr << "connection accepted from " << socket.remote_endpoint().address().to_string() << ":" << socket.remote_endpoint().port() << std::endl;
           if(!ec)
           {
             std::make_shared<bulk_session>(std::move(socket), _bulk_size)->start();
           }

           do_accept();
         });
  }

  tcp::acceptor _acceptor;
  size_t _bulk_size;
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc < 3)
    {
      std::cerr << "Usage: bulk_server <port> <bulk_size>\n";
      return 1;
    }
    size_t bulk_size = std::atoi(argv[2]);

    ba::io_context io_context;

    std::list<bulk_server> servers;
    tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[1]));
    servers.emplace_back(io_context, endpoint, bulk_size);

    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}