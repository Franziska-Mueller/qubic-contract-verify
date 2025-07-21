cmake_minimum_required(VERSION 3.4)
cmake_policy(SET CMP0054 NEW)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++1z -fvisibility=hidden -fvisibility-inlines-hidden")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fvisibility=hidden")
else()
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -std=c++1z -fvisibility=hidden -fvisibility-inlines-hidden -fprofile-arcs -ftest-coverage --coverage")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fvisibility=hidden")
endif()

set(CMAKE_CXX_STANDARD 17)

# Define DLLEXPORT and DLLIMPORT
if(WIN32 OR CYGWIN )
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(DLLEXPORT "__attribute__((dllexport))")
		set(DLLIMPORT "")
	else()
		set(DLLEXPORT "__declspec(dllexport)")
		set(DLLIMPORT "__declspec(dllimport)")
	endif()
else()
	set(DLLEXPORT "__attribute__((visibility (\"default\")))")
	set(DLLIMPORT "")
endif()

