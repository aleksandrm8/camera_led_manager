map <F5> :!./client.sh<CR>
map <F7> :!./test.sh<CR>
map <F6> :!g++ -std=c++11 fifo_tools.cpp commands.cpp server.cpp clients_manager.cpp  -lpthread
					\&& ./a.out<CR>
nnoremap tj :tabprev<CR>
nnoremap tk :tabnext<CR>
set tabstop=2
set shiftwidth=2
set ai
set cin
