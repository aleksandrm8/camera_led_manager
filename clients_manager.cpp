#include "clients_manager.h"

ClientsManager::ClientsManager():
							m_stop_flag(false)
{
 	unlink(CLIENT_FIFO_NAME);
	int err = mkfifo(CLIENT_FIFO_NAME, O_RDWR);
	if (err < 0)
		throw std::runtime_error("Can not create user management fifo.");
#ifdef __CYGWIN__
	system("chmod.exe 777 " CLIENT_FIFO_NAME);
#endif
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

void ClientsManager::ThreadFun()
{
	char read_buf[kMaxCmdLenght];

	while (!m_stop_flag)
	{
		RaiiFile file(open(CLIENT_FIFO_NAME, O_RDONLY | O_NONBLOCK));
		if (file < 0)
		{
			throw std::runtime_error(
				std::string("Can not open user management fifo: ") 
				+ std::string(strerror(errno)) );
		}
		fd_set set;
	  timeval timeout;
		timeout.tv_sec = 0;
  	timeout.tv_usec = 500000;
		FD_ZERO(&set);
	  FD_SET(file, &set);

		int err = 0;
		err = select(file+1, &set, NULL, NULL, &timeout);
		if (err == -1)
		{
			std::cerr << "select file error: "
								<< strerror(errno) << std::endl;
		}
		else if (err)
		{
			int cmd_lenght = read(file, read_buf, sizeof(read_buf));
			if ( cmd_lenght < 0)
			{
				throw std::runtime_error(
					std::string("Can not read from fifo: ")
					+ std::string(strerror(errno)) );
			}
			std::string command(read_buf, cmd_lenght);
		
			if (!command.compare(0,
													 kAddClientCommand.size(),
													 kAddClientCommand))
			{
				std::cout << "add client command" << std::endl;
			}
			else if (!command.compare(0,
																kDelClientCommand.size(),
																kDelClientCommand))
			{
				std::cout << "del client command" << std::endl;
			}
			else
			{
				std::cerr << "wrong command: \""
									<< command << "\"" << std::endl;
			}
		}
		else
		{
		}
	}

	return;
}
