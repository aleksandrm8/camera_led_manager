all:
	g++ -g -std=c++11 fifo_tools.cpp commands.cpp server.cpp clients_manager.cpp -lpthread -o server
clean:
	rm server
