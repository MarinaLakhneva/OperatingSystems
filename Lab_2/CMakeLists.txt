project(lab2)
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Werror)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(host client conn utils)
FILE(GLOB HOST host/*.cpp)
FILE(GLOB CLIENT client/*.cpp)
FILE(GLOB UTILS utils/*.cpp)

add_executable(host-mq ${HOST} ${UTILS} conn/conn_mq.cpp)
target_link_libraries(host-mq Threads::Threads rt)

add_executable(client-mq ${CLIENT} ${UTILS} conn/conn_mq.cpp)
target_link_libraries(client-mq Threads::Threads rt)

add_executable(host-shm ${HOST} ${UTILS} conn/conn_shm.cpp)
target_link_libraries(host-shm Threads::Threads rt)

add_executable(client-shm ${CLIENT} ${UTILS} conn/conn_shm.cpp)
target_link_libraries(client-shm Threads::Threads rt)

add_executable(host-fifo ${HOST} ${UTILS} conn/conn_fifo.cpp)
target_link_libraries(host-fifo Threads::Threads rt)

add_executable(client-fifo ${CLIENT} ${UTILS} conn/conn_fifo.cpp)
target_link_libraries(client-fifo Threads::Threads rt)
