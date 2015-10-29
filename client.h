
#ifndef CAMERA_LED_MANAGER_CLIENT_H_
#define CAMERA_LED_MANAGER_CLIENT_H_

#include "server.h"

class Client
{
  static const int kReadFifoTimeout = 500000;
  static const int kWriteFifoTimeout = 500000;
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
      int err = mkfifo(iter.c_str(), 0666);
      if (err < 0)
      {
        throw std::runtime_error("Can not create fifo"
                                 + iter + " for client " + id);
      }
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
      Res res = ReadFromFifo(m_input_fifo, cmd, kReadFifoTimeout);
      if (res == Res::TIMED_OUT)
        continue;
      std::string ans;

      // parse command
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
  
      if ((cmd::StrToPrefixes.find(prefix_str) == cmd::StrToPrefixes.end())
          || cmd::Commands.find(cmd::StrToPrefixes.at(prefix_str))
            == cmd::Commands.end()
          || !cmd::Commands.at(cmd::StrToPrefixes.at(prefix_str)))
      {
        std::cerr << "Command with prefix "
          << prefix_str << " not supported" << std::endl;
        std::cerr << "Supported commands: " << std::endl;
        for (auto iter: cmd::StrToPrefixes)
          std::cerr << iter.first << std::endl;
        ans = "FAILED\n";
      }
      else
      {
        cmd::Prefixes prefix = cmd::StrToPrefixes.at(prefix_str);
        try
        {
          ans = cmd::Commands.at(prefix)->Run(Server::Instance(), args);
        }
        catch (const std::exception &ex)
        {
          std::cerr << "Error runninng command " << prefix_str<< ": "
            << ex.what();
          ans = "FAILED\n";
        }
      }

      // write to fifo
      res = WriteToFifo(m_output_fifo, ans, kWriteFifoTimeout);
      if (res == Res::NOBODY_READ_FIFO)
        std::cerr << "Nobody read fifo " << m_id << "_output" << std::endl;
    }
  }

  std::string       m_id;
  std::thread       m_thread;
  std::atomic_bool  m_stop_flag;
  std::string       m_input_fifo;
  std::string       m_output_fifo;
};
typedef std::shared_ptr<Client> ClientPtr;

#endif // CAMERA_LED_MANAGER_CLIENT_H_
