cmake_minimum_required(VERSION 3.22)
project(trading_engine)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(networking)
add_subdirectory(exchange)
add_subdirectory(client)
add_subdirectory(server)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME}
        PUBLIC
            exchange
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
            exchange
)