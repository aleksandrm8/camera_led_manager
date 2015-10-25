
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <atomic>
#include <iostream>
#include <fstream>
#include <memory>
#include <map>
#include <stdexcept>
#include <stdlib.h>
#include <thread>

#define CLIENT_FIFO_NAME "clients_mgm_fifo"

class Client
{
public:
	Client() {};
	~Client() {};
private:
	std::string m_id;
};

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

	std::shared_ptr<std::thread>	m_cl;
	std::map<std::string, Client>	m_clients;
	std::atomic_bool							m_stop_flag;
};
