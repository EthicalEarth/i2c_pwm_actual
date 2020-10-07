cmake_minimum_required(VERSION 2.6)
project(i2c_pwm)

add_executable(i2c_pwm main.cpp)
set(CMAKE_CXX_FLAGS "-lconfig++ -std=c++11")

install(TARGETS i2c_pwm RUNTIME DESTINATION bin)
