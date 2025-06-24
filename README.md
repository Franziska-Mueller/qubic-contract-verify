# Prerequisites

Download/clone the CppParser repository from https://github.com/satya-das/cppparser and build it.

# How to build

`mkdir build`

`cd build`

`cmake ..`

cmake will try to find the CppParser dependency (variable `cppparser_DIR`). If it cannot find it automatically, manually point the variable to the `CppParser/builds/` directory (the one containing `cppparserConfig.cmake`).
You can configure cmake whether to include the test project by setting the `BUILD_TESTS` variable accordingly.

On linux, you can then build via `make`. On Windows, you can build the solution file cmake created.

# How to run

Navigate to your `build/src/<CONFIGURATION>` directory where `CONFIGURATION` is `Debug` or `Release`.
Run the tool on your contract file:

`./contractverify <FILEPATH>`
