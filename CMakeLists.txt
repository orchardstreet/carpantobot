cmake_minimum_required(VERSION 3.0)

project(carpantobot LANGUAGES C)

add_executable(carpantobot main.c stdin.c user_interaction.c network.c startup.c)

install(TARGETS carpantobot RUNTIME DESTINATION bin)
