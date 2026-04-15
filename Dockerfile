# Base image
FROM gcc:latest

# Install cmake and git (git is needed for submodules)
RUN apt-get update && \
    apt-get install -y cmake flex clang-tidy git && \
    rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /contract-verify

# Copy the entire project first (needed for .gitmodules and context)
COPY . .

# Initialize and update submodules
# This will populate /contract-verify/deps/CppParser
RUN git submodule update --init --recursive

# Build CppParser dependency
RUN cd deps/CppParser && \
    mkdir -p builds && \
    cd builds && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . --config Release

# Build contract verify executable
RUN mkdir -p build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_CONTRACTVERIFY_TESTS:BOOL=OFF .. && \
    cmake --build . --config Release

# Copy entrypoint script from repository to the filesystem path `/` of the container
COPY --chmod=755 entrypoint.sh /entrypoint.sh

# Execute `entrypoint.sh` when the Docker container starts up
ENTRYPOINT ["/entrypoint.sh"]
