project(las)

include_directories(../include)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-fno-exceptions -fno-rtti")

add_executable(xldz loader.cpp executor.cpp snapshot.cpp main.cpp)
#add_executable(tester labelTable.cpp phase1.cpp phase2.cpp main.cpp)
