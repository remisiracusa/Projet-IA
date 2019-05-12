cmake_minimum_required(VERSION 3.7)
project(Client C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(fonctionsTCP)

add_executable(Client
		fonctionsTCP/fonctionsTCP.c
		fonctionsTCP/fonctionsTCP.h
		client.c
		client.h
		protocol.h
		protocolYokai.h)
