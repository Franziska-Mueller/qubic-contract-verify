# Search dependency cppparser
find_package(cppparser REQUIRED)

# Library for contract verification
add_library(${CONTRACT_VERIFY_LIB}
  lib/defines.h
  lib/check_compliance.h
  lib/check_compliance.cpp
  lib/check_expressions.h
  lib/check_expressions.cpp
  lib/check_variables.h
  lib/check_variables.cpp
  lib/check_branching_looping.h
  lib/check_branching_looping.cpp
  lib/check_names_and_types.h
  lib/check_names_and_types.cpp
  lib/check_functionlike.h
  lib/check_functionlike.cpp
)
target_link_libraries(${CONTRACT_VERIFY_LIB} cppparser)
target_include_directories(${CONTRACT_VERIFY_LIB} 
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Main executable
add_executable(${CONTRACT_VERIFY_EXE} qubic_contract_verify.cpp)
target_link_libraries(${CONTRACT_VERIFY_EXE} ${CONTRACT_VERIFY_LIB})
