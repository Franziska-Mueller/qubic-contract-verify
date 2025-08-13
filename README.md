# Qubic Contract Verification Tool

This in a tool that automatically checks that C++ files comply with the C++ language feature restrictions Qubic imposes on smart contract (SC) files.

## Contract Verify Container Action

This repository provides a docker-based container action that can be used in a GitHub workflow.

Example:

```
jobs:
  contract_verify_job:
    runs-on: ubuntu-latest
    name: Verify smart contract files
    steps:
      # Checkout repo to use files of the repo as input for container action
      - name: Checkout
        uses: actions/checkout@v4
      - name: Contract verify action step
        uses: Franziska-Mueller/qubic-contract-verify@v0.3.0-beta
        with:
          filepaths: 'fileA.h,fileB.h'
```

The `filepaths` input can be a single file or a comma-separated list of files (without spaces).

## Build and Run from Source

### Prerequisites

Configure the CppParser dependency using cmake:

```
cd deps/CppParser
mkdir builds
cd builds
cmake ..
```

You can then build CppParser, depending on your OS:
- [ Linux ] Run `make` in the `builds` folder.
- [ Windows ] Open the solution file cmake created in the `builds` folder in Visual Studio and build it.

### Build the Contract Verify Tool

```
mkdir build
cd build
cmake ..
```

cmake will try to find the CppParser dependency (variable `cppparser_DIR`). If it cannot find it automatically, manually point the variable to the `deps/CppParser/builds/` directory (the one containing `cppparserConfig.cmake`).
You can configure cmake whether to include the test project by setting the `BUILD_CONTRACTVERIFY_TESTS` variable accordingly.

You can then build the tool via `make` (Linux) or Visual Studio (Windows) as detailed in the previous step when building the dependencies.

### Run the Contract Verify Tool

Navigate to your `build/src/<CONFIGURATION>` directory where `CONFIGURATION` is `Debug` or `Release`.
Run the tool on your contract file:

`./contractverify <FILEPATH>`
