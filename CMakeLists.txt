cmake_minimum_required(VERSION 3.22)
project(Dag_lib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-pthread -O2")

include_directories("/home/boy/fastflow")
add_executable(Dag_lib main.cpp Graph.hpp Node.hpp ThreadPool.hpp ConcurrentQueue.hpp mdf.hpp mdfi.hpp Utils.hpp)
