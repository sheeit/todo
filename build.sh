#/bin/sh

# Simple script to build this program.
# Usage: ./build.sh [PREFIX]
# Prefix defaults to /usr/local
# After this scripts completes, run ‘make install’ to install it.

if [ "${#}" -ge 1 ] && ([ "${1}" = '-h' ] || [ "${1}" = '--help' ]); then
	cat 1>&2 <<-EOF
		Usage: "${0}" [PREFIX]
		PREFIX defaults to /usr/local.
		After this script completes, run ‘make install’ to install
		You may need to run it as root, depending on the PREFIX.
EOF
	exit
fi

test -d build && rm -rf build && mkdir build
autoreconf -i && \
cd build && \
../configure --prefix="${1:-/usr/local}" && \
make -j4
