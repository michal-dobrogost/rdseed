cmake_minimum_required(VERSION 3.5.0)

project(rdseed C)
add_executable(rdseed main.c)
target_compile_options(rdseed PRIVATE "-mrdrnd")
install(TARGETS rdseed DESTINATION .)
