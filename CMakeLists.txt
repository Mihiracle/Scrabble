# CMake buildfile for my HW4 solution.
# NOTE: this file isn't part of the solution, it's just so that I can use CLion
# to develop my code.

cmake_minimum_required(VERSION 3.1)

project(hw4-jamies_solution)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(SCRABBLE_SOURCES
	Bag.cpp
	ConsolePrinter.cpp
	Board.cpp
	Player.cpp
	main.cpp
	ConfigFile.h
	Dictionary.cpp
	Move.cpp
	Util.h
	Tile.h
	Square.h)

add_executable(scrabble ${SCRABBLE_SOURCES})