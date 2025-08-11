# Base image
FROM gcc:latest

# Install cmake
RUN apt-get update;              \
    apt-get install -y cmake ;   \
    apt-get install -y flex ;    \
    apt-get install -y clang-tidy  

# Copy source code and cmake files
COPY ./deps /contract-verify/deps
COPY ./src /contract-verify/src
COPY ./CMakeLists.txt /contract-verify/

# Build CppParser dependency
RUN cd /contract-verify/deps/CppParser ;     \
    mkdir builds ;                           \
    cd builds ;                              \
    cmake -DCMAKE_BUILD_TYPE=Release .. ;    \
    cmake --build . --config Release

# Build contract verify executable
RUN cd /contract-verify/ ;             \
    mkdir build ;                      \
    cd build ;                         \
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_CONTRACTVERIFY_TESTS:BOOL=OFF .. ;    \
    cmake --build . --config Release

# Copy entrypoint script from repository to the filesystem path `/` of the container
COPY --chmod=755 entrypoint.sh /entrypoint.sh

# Execute `entrypoint.sh` when the Docker container starts up
ENTRYPOINT ["/entrypoint.sh"]