# training-raft-impl
Raft consensus algorithm implementation for learning purposes

# How to install``
1. Install clang-10 (https://apt.llvm.org/ on Ubuntu/Debian). Do not forget `libc++abi-10-dev` and `libc++-10-dev` packages.
2. Create `build` subfolder and execute `conan install . --profile clang-10 --build=missing`. Look in `clang-10.conan-profile.example` file for the profile example.
3. Use cmake in the same `build` folder to build the project.
