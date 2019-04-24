cmake_minimum_required(VERSION 3.7)
project(Serveur C)

set(CMAKE_C_STANDARD 11)

add_executable(Serveur src/fonctionsTCP.c src/serveur.c src/fonctionsTCP.h src/protocolYokai.h src/serveur.h)