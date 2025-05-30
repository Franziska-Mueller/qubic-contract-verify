cmake_minimum_required(VERSION 3.21)
project(QubicContractVerify LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(cppparser REQUIRED)

# Main executable
add_executable(${PROJECT_NAME} src/qubic_contract_verify.cpp src/check_compliance.cpp src/check_compliance.h)

# Linking
target_link_libraries(${PROJECT_NAME} cppparser)
