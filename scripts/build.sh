#! /usr/bin/env nix-shell
#! nix-shell -i bash --pure build.nix --keep BUILD_TYPE --keep BIN_PREFIX
#
# The above shebang sets up the necessary packages to run this script without
# installing the packages globally on your computer. It's probably just CMake
# but you can check by reading the scripts/build.nix file.
#
# Run without nix via `bash scripts/build.sh`. This assumes you have setup
# all dependencies in your environment yourself.

set -ex

base=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )/.." &> /dev/null && pwd )

if [[ ! -n "${BUILD_TYPE}" ]]; then
  BUILD_TYPE='Release' # Debug | Release
fi

cmake --version
cmake -S "${base}" -B build -D CMAKE_BUILD_TYPE="${BUILD_TYPE}"
cmake --build build --config "${BUILD_TYPE}" --verbose
if [[ -n "${BIN_PREFIX}" ]]; then
  cmake --install build --config "${BUILD_TYPE}" --verbose --prefix "${BIN_PREFIX}"
fi
