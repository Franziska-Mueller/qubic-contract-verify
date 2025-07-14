cmake_minimum_required(VERSION 3.21)
project(QubicContractVerify LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CONTRACT_VERIFY_LIB "contractverifylib")
set(CONTRACT_VERIFY_EXE "contractverify")
add_subdirectory(src)

set(BUILD_CONTRACTVERIFY_TESTS TRUE CACHE BOOL "Builds the contract verify test project")

if(${BUILD_CONTRACTVERIFY_TESTS})
  set(TEST_EXE "contractverifytest")
  add_subdirectory(test)
endif()