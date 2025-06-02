cmake_minimum_required(VERSION 3.21)
project(QubicContractVerify LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(cppparser REQUIRED)

# Library for contract verification
add_library(${CONTRACT_VERIFY_LIB} lib/check_compliance.cpp lib/check_compliance.h)
target_link_libraries(${CONTRACT_VERIFY_LIB} cppparser)
target_include_directories(${CONTRACT_VERIFY_LIB} 
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Main executable
add_executable(${CONTRACT_VERIFY_EXE} qubic_contract_verify.cpp)
target_link_libraries(${CONTRACT_VERIFY_EXE} ${CONTRACT_VERIFY_LIB})
