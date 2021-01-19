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

### disclaimer
Протестировано на archlinux, boost-libs 1.75.0-2.
На старых версиях ubuntu могут быть проблемы с совместимостью C++17 и дефолтным boost из репозитория

## Запуск
### Сервер
```(bash)
./apps/chat_server server 4242
```
Взаимодействие:

Сообщение считывается до перевода строки, такжеподдерживаются следующие команды:
* /shutdown

### Клиент
```(bash)
./apps/chat_client ulyanin localhost 4242
```
Взаимодействие:

Сообщение считывается до перевода строки, такжеподдерживаются следующие команды:
* /shutdown
