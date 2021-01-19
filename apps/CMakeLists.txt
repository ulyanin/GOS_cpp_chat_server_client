add_executable(chat_server chat_server.cpp)
target_compile_features(chat_server PRIVATE cxx_std_17)
target_link_libraries(chat_server PRIVATE chat_server_library)
target_include_directories(chat_server PUBLIC ${CMAKE_BINARY_DIR}/proto ../src)

add_executable(chat_client chat_client.cpp)
target_compile_features(chat_client PRIVATE cxx_std_17)
target_link_libraries(chat_client PRIVATE chat_server_library)
target_include_directories(chat_client PUBLIC ${CMAKE_BINARY_DIR}/proto ../src)

add_executable(chat_server_native chat_server_native.cpp)
target_compile_features(chat_server_native PRIVATE cxx_std_17)
target_link_libraries(chat_server_native PRIVATE chat_server_library)
target_include_directories(chat_server_native PUBLIC ${CMAKE_BINARY_DIR}/proto ../src)