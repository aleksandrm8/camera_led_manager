map <F5> :!./client.sh<CR>
map <F7> :!./test.sh<CR>
map <F6> :!g++ -g -std=c++11
      \ fifo_tools.cpp
      \ commands.cpp
      \ server.cpp
      \ clients_manager.cpp
      \ -lpthread -o server
			\&& ./server<CR>
map <F8> :!gdb a.out<CR>
nnoremap tj :tabprev<CR>
nnoremap tk :tabnext<CR>
set tabstop=2
set shiftwidth=2
set expandtab 
set ai
set cin
