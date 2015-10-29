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
#include "client.h"

#ifndef CLIENTS_NAMAGER_CAMERA_LED_MANAGER_H_
#define CLIENTS_NAMAGER_CAMERA_LED_MANAGER_H_

#define CLIENT_FIFO_NAME "clients_mgm_fifo"

class ClientsManager
{
  const std::string kAddClientCommand = "add client "/*client name*/;
  const std::string kDelClientCommand = "delete client "/*client name*/;
  const int kMaxCmdLenght = 512;
  static const int kReadFifoTimeout = 500000;
public:
  ClientsManager();
  ~ClientsManager();
private:
  void ThreadFun();
  std::ifstream m_fifo_;

  std::shared_ptr<std::thread>      m_cl;
  std::map<std::string, ClientPtr>  m_clients;
  std::atomic_bool                  m_stop_flag;
};

#endif
