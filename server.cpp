#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <atomic>
#include <iostream>
#include <memory>
#include <map>
#include <stdexcept>
#include <stdlib.h>
#include <thread>

#include "clients_manager.h"

int main()
{
	try
	{
		std::shared_ptr<std::atomic_bool> stop_flag =
			std::make_shared<std::atomic_bool>(false);
		ClientsManager clients_manager;
    std::cout << "Camera LED manager server started. "
      "Press q<enter> for quit."
      << std::endl;
		while (1)
		{
			std::string str;
			std::cin >> str;
			if (!str.compare("q"))
				break;
		}
	}
	catch (const std::exception &ex)
	{
		std::cerr << "error: " << ex.what() << std::endl;
	}
	return 0;
}
