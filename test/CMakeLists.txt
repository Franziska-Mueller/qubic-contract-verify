# Fetch googletest for test project
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        52eb8108c5bdec04579160ae17225d66034bd723 # release-1.17.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Write base dir for test files to config
get_filename_component(TEST_FILE_PATH testfiles ABSOLUTE)
configure_file(test_files_config.h.in test_files_config.h)

enable_testing()

add_executable(
  ${TEST_EXE}
  contract_verify_test.cpp
  test_files_config.h
)
target_link_libraries(
  ${TEST_EXE}
  GTest::gtest_main
  GTest::gmock_main
  ${CONTRACT_VERIFY_LIB}
)
target_include_directories(
  ${TEST_EXE}
  PUBLIC
  ${CMAKE_CURRENT_BINARY_DIR}
)

include(GoogleTest)
gtest_discover_tests(${TEST_EXE})
