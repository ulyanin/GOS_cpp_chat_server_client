# GOS_cpp_chat_server_client
Chat server/client between two participant implemented on C++

## Установка
```(Bash)
sudo apt install libboost-dev libboost-system-dev libboost-thread-dev 

mkdir build
cd build
cmake ..
make
```

## Запуск
### Сервер
```(bash)
./apps/chat_server server 4242
```

### Клиент
```(bash)
./apps/chat_client ulyanin localhost 4242
```
