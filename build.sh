#/bin/sh

# Simple script to build this program.
# Usage: ./build.sh [PREFIX]
# Prefix defaults to /usr/local
# After this scripts completes, run ‘make install’ to install it.

test -d build && rm -rf build && mkdir build
autoreconf -i && \
cd build && \
../configure --prefix="${1:-/usr/local}" && \
make -j4
