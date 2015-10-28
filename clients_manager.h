#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include <algorithm>
#include <atomic>
#include <iostream>
#include <fstream>
#include <list>
#include <memory>
#include <map>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <thread>

#include "fifo_tools.h"
#include "commands.h"

#ifndef CLIENTS_NAMAGER_CAMERA_LED_MANAGER_H_
#define CLIENTS_NAMAGER_CAMERA_LED_MANAGER_H_

#define CLIENT_FIFO_NAME "clients_mgm_fifo"

class Color
{
public:
  enum class Colors
  {
    RED,
    GREEN,
    BLUE
  };
  std::map<Colors, std::string> to_str = 
  {
    {Colors::RED, "red"},
    {Colors::GREEN, "green"},
    {Colors::BLUE, "blue"}
  };
  std::map<std::string, Colors> to_enum = 
  {
    {to_str.at(Colors::RED), Colors::RED},
    {to_str.at(Colors::GREEN), Colors::GREEN},
    {to_str.at(Colors::BLUE), Colors::BLUE}
  };
  Color(Colors color_): m_color(color_)
  {}
  Color(std::string str)
  {
    if (to_enum.find(str) == to_enum.end())
    {
      std::stringstream ss;
      ss << "Color " << str << " not supported. ";
      ss << " Supported colors are: ";
      for (auto iter: to_str)
      {
        ss << iter.second;
        if (iter.second != to_str.rbegin()->second)
          ss << ", ";
      }
      throw std::runtime_error(ss.str());
    }
    m_color = to_enum.at(str);
  }
  operator std::string()
  {
    return to_str.at(m_color);
  }
private:
  Colors m_color;
};

class Command
{
public:
  Command(std::string prefix, std::list<std::string> args):
    m_prefix(prefix),
    m_args(args)
  {}
  virtual ~Command() {};
  bool Parse(std::string cmd)
  {
    // parse prefix
    if (cmd.size() < m_prefix.size()
        || cmd.compare(0, m_prefix.size(), m_prefix))
      return false;
    std::string prefix = cmd.substr();
  }
private:
  std::string             m_prefix;
  std::list<std::string>  m_args;
};

inline std::string DelTrNl(std::string str)
{
  while (str.size()
      && str.at(str.size() - 1) == '\n')
  {
    str = str.substr(0, str.size()-1);
  }
  return str;
}

inline std::string DelSpaces(std::string str)
{
  str.erase(std::remove_if(str.begin(), str.end(), isspace), str.end());
  return str;
}

class Server
{
public:
  static Server* Instance()
  {
    static Server *server =
      new Server(false, Color(Color::Colors::BLUE), 0);
    return server;
  }
  std::string RunCmd(std::string cmd)
  {
    size_t del = cmd.find(" ");
    std::string prefix_str("");
    std::string args("");
    if (del == std::string::npos)
    {
      prefix_str = DelSpaces(DelTrNl(cmd));
    }
    else
    {
      prefix_str = cmd.substr(0, del);
      args = DelSpaces(DelTrNl(cmd.substr(del, cmd.size())));
    }

    if (cmd::StrToPrefixes.find(prefix_str) == cmd::StrToPrefixes.end())
    {
      std::cout << "can not find command with prefix "
        << prefix_str << std::endl;
      std::cout << "prefixes:" << std::endl;
      for (auto iter: cmd::StrToPrefixes)
        std::cout << iter.first << std::endl;
      return "FAILED";
    }
    cmd::Prefixes prefix = cmd::StrToPrefixes.at(prefix_str);
    if (cmd::Commands.find(prefix) == cmd::Commands.end())
    {
      throw std::runtime_error(
        "There is no command class for command "
        + prefix_str);
    }
    if (!cmd::Commands.at(prefix))
    {
      throw std::runtime_error(
        "Command class for command "
        + prefix_str + " is null.");
    }

    return cmd::Commands.at(prefix)->Run(this, args);
  }
  bool GetState()
  {
    std::lock_guard<std::mutex> lock(m_state_m);
    return m_state;
  }
  void SetState(bool state)
  {
    std::lock_guard<std::mutex> lock(m_state_m);
    m_state = state;
  }
  Color GetColor()
  {
    std::lock_guard<std::mutex> lock(m_color_m);
    return m_color;
  }
  void SetColor(Color color)
  {
    std::lock_guard<std::mutex> lock(m_color_m);
    m_color = color;
  }
  float GetRate()
  {
    std::lock_guard<std::mutex> lock(m_rate_m);
    return m_rate;
  }
  void SetRate(float rate)
  {
    std::lock_guard<std::mutex> lock(m_rate_m);
    m_rate = rate;
  }

private:
  Server(bool state, Color color, int rate):
    m_state(state),
    m_color(color),
    m_rate(rate)
  {}
  ~Server() {}
  Server(const Server&) = delete;
  Server& operator=(const Server) = delete;

  bool        m_state; // true - ON; false - OFF
  std::mutex  m_state_m;
  Color       m_color;
  std::mutex  m_color_m;
  float       m_rate;
  std::mutex  m_rate_m;
};

class Client
{
public:
	explicit Client(std::string id):
    m_id(id),
    m_thread(),
    m_stop_flag(false),
    m_input_fifo(id + "_input"),
    m_output_fifo(id + "_output")
  {
    const std::string input_cmd = m_input_fifo;
    const std::string output_cmd = m_output_fifo;
    const std::list<std::string> list = {input_cmd, output_cmd};

    for (const auto &iter: list)
    {
   	  unlink(iter.c_str());
  	  int err = mkfifo(iter.c_str(), O_RDWR);
  	  if (err < 0)
      {
  		  throw std::runtime_error("Can not create fifo"
                                 + iter + " for client " + id);
      }
#ifdef __CYGWIN__
  	  system(("chmod.exe 777 " + iter).c_str());
#else
  	  system(("chmod +rw " + iter).c_str());
#endif
    }
    m_thread = std::thread(&Client::ThreadFun, this);
  }
	~Client()
  {
    m_stop_flag = true;
    if (m_thread.joinable())
      m_thread.join();
    const std::string input_cmd = m_input_fifo;
    const std::string output_cmd = m_output_fifo;
    const std::list<std::string> list = {input_cmd, output_cmd};

    for (const auto &iter: list)
    {
   	  unlink(iter.c_str());
    }
  }
private:
	void ThreadFun()
  {
    std::cout << "start client thread" << std::endl;
    while (!m_stop_flag)
    {
      using namespace ff_tools;
      // read from fifo
      std::string cmd;
      Res res = ReadFromFifo(m_input_fifo, cmd, 500000);
      if (res == Res::TIMED_OUT)
        continue;
      std::string uns = Server::Instance()->RunCmd(cmd);
      // write to fifo
      res = WriteToFifo(m_output_fifo, uns, 500000);
      if (res == Res::NOBODY_READ_FIFO)
        std::cout << "Nobody read fifo " << m_id << "_output" << std::endl;
    }
  }

	std::string       m_id;
	std::thread       m_thread;
	std::atomic_bool  m_stop_flag;
  std::string       m_input_fifo;
  std::string       m_output_fifo;
};
typedef std::shared_ptr<Client> ClientPtr;

class ClientsManager
{
	const std::string kAddClientCommand = "add client "/*client name*/;
	const std::string kDelClientCommand = "delete client "/*client name*/;
	const int kMaxCmdLenght = 512;
public:
	ClientsManager();
	~ClientsManager();
private:
	void ThreadFun();
	std::ifstream m_fifo_;

	std::shared_ptr<std::thread>      m_cl;
	std::map<std::string, ClientPtr>	m_clients;
	std::atomic_bool						      m_stop_flag;
};

#endif
