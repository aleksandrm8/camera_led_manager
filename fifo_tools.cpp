#include "fifo_tools.h"

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include <atomic>
#include <iostream>
#include <fstream>
#include <list>
#include <memory>
#include <map>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <thread>

namespace ff_tools
{
class RaiiFile
{
public:
	RaiiFile(int fd, std::string name): m_fd(fd)
  {
  	if (m_fd < 0)
  	{
  		throw std::runtime_error(
  			std::string("Can not open fifo ") 
        + name + ": "
  			+ std::string(strerror(errno)) );
	  }
  }
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

Res ReadFromFifo(std::string name, std::string &res, size_t timeout_)
{
	char read_buf[kMaxCmdLenght];
  RaiiFile file(open(name.c_str(), O_RDONLY | O_NONBLOCK), name);

	fd_set set;
  timeval timeout;
	timeout.tv_sec = 0;
 	timeout.tv_usec = timeout_;
	FD_ZERO(&set);
  FD_SET(file, &set);
	int err = 0;
	err = select(file + 1, &set, NULL, NULL, &timeout);
	if (err == -1)
	{
		throw std::runtime_error(
			std::string("Select file error:: ") 
			+ std::string(strerror(errno)) );
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
    res = command;
    return Res::OK;
  }
	else
	{
    return Res::TIMED_OUT;
	}
}

Res WriteToFifo(std::string name, std::string cmd, size_t timeout_)
{
  if (-1 == access(name.c_str(), F_OK))
  {
		throw std::runtime_error(
			std::string("FIFO ") + name + "does not exist.");
  }
  //int fd = open(name.c_str(), O_WRONLY | O_NONBLOCK);
  int fd = -1;
  while (1)
  {
    fd = open(name.c_str(), O_WRONLY);
    if (-1 == fd && ENXIO == errno)
    {
      size_t retry_timeout = 100000;
      std::this_thread::sleep_for(std::chrono::microseconds(retry_timeout));
      if (retry_timeout > timeout_)
      {
        timeout_ = 0;
        break;
      }
      else
      {
        timeout_ -= retry_timeout;
      }
    }
    else
    {
      break;
    }
  }
  RaiiFile file(fd, name);
	fd_set set;
  timeval timeout;
	timeout.tv_sec = 0;
 	timeout.tv_usec = timeout_;
	FD_ZERO(&set);
  FD_SET(file, &set);
	int err = 0;
	err = select(file + 1, NULL, &set, NULL, &timeout);
	if (err == -1)
	{
		throw std::runtime_error(
			std::string("Select file error: ") 
			+ std::string(strerror(errno)) );
	}
	else if (err)
	{
		int cmd_lenght = write(file, cmd.c_str(), cmd.size());
		if ( cmd_lenght < 0)
		{
			throw std::runtime_error(
				std::string("Can not read from fifo: ")
				+ std::string(strerror(errno)) );
		}
    return Res::OK;
  }
	else
	{
    return Res::TIMED_OUT;
	}
}

} // end if namespace
