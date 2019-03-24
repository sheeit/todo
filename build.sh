#!/bin/sh

# Simple script to build this program.
# Usage: ./build.sh [PREFIX]
# Prefix defaults to /usr/local
# After this scripts completes, run ‘make install’ to install it.

if [ "${#}" -ge 1 ] && { [ "${1}" = '-h' ] || [ "${1}" = '--help' ]; }; then
	cat 1>&2 <<-EOF
		Usage: "${0}" [PREFIX]
		PREFIX defaults to /usr/local.
		After this script completes, run ‘make install’ to install
		You may need to run it as root, depending on the PREFIX.
EOF
	exit 0
fi

CC='gcc'
CFLAGS='-Wall -Wextra -pedantic-errors -O2 -march=native -mtune=native'
export CC CFLAGS

test -d build && rm -rf build
mkdir build || exit 1
autoreconf -i || exit 1
cd build || exit 1
../configure --prefix="${1:-/usr/local}" || exit 1
make -j4 || exit 1

unset CC CFLAGS
