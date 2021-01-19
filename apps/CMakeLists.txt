add_executable(app app.cpp)
target_compile_features(app PRIVATE cxx_std_17)
target_link_libraries(app PRIVATE chat_server_library)

add_executable(chat_server chat_server.cpp)
target_compile_features(chat_server PRIVATE cxx_std_17)
target_link_libraries(chat_server PRIVATE chat_server_library)

add_executable(chat_client chat_client.cpp)
target_compile_features(chat_client PRIVATE cxx_std_17)
target_link_libraries(chat_client PRIVATE chat_server_library)
