#include "clients_manager.h"

ClientsManager::ClientsManager():
              m_stop_flag(false)
{
  unlink(CLIENT_FIFO_NAME);
  int err = mkfifo(CLIENT_FIFO_NAME, 0666);
  if (err < 0)
    throw std::runtime_error("Can not create user management fifo.");
  m_cl = std::make_shared<std::thread>(&ClientsManager::ThreadFun, this);
}

ClientsManager::~ClientsManager() 
{
  m_stop_flag = true;
  if (m_cl && m_cl->joinable())
    m_cl->join();
  unlink(CLIENT_FIFO_NAME);
}

class RaiiFile
{
public:
  RaiiFile(int fd): m_fd(fd) {}
  RaiiFile(const RaiiFile&) = delete;
  RaiiFile& operator=(const RaiiFile&) = delete;
  ~RaiiFile()
  {
    close(m_fd);
  }
  operator int() const
  {
    return m_fd;
  }
private:
  int m_fd;
};
void DelTrailingNewLines(std::string &str)
{
  while (str.size()
         && str.at(str.size() - 1) == '\n')
  {
    str = str.substr(0, str.size() - 1);
  }
}

void ClientsManager::ThreadFun()
{
  try
  {
    while (!m_stop_flag)
    {
      std::string command;
      ff_tools::Res res =
        ff_tools::ReadFromFifo(CLIENT_FIFO_NAME, command, kReadFifoTimeout);
      if (res == ff_tools::Res::OK)
      {
        DelTrailingNewLines(command);
        if (!command.compare(0,
                             kAddClientCommand.size(),
                             kAddClientCommand))
        {
          const std::string id =
            command.substr(kAddClientCommand.size(),
                            command.size());
          if (m_clients.find(id) != m_clients.end())
          {
            std::cerr << "client" << id
                      << "already exist"
                      << std::endl;
          }
          else
          {
            m_clients.insert(std::make_pair(id, std::make_shared<Client>(id)));
            std::cout << "client " << id << " added"
              << std::endl;
          }
        }
        else if (!command.compare(0,
                                  kDelClientCommand.size(),
                                  kDelClientCommand))
        {
          const std::string id =
            command.substr(kDelClientCommand.size(),
                            command.size());
          if (m_clients.find(id) != m_clients.end())
          {
            m_clients.erase(id);
            std::cout << "client " << id << " deleted"
              << std::endl;
          }
          else
          {
            std::cerr << "client" << id
                      << " not found"
                      << std::endl;
          }
        }
        else
        {
          std::cerr << "wrong command: \""
                    << command << "\"" << std::endl;
        }
      }
    }
  }
  catch (const std::runtime_error &err)
  {
    std::cerr << "error reading from clients"
      " management fifo: "
      << err.what();
  }

  return;
}
