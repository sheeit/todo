#!/usr/bin/env sh

old_cwd="$(realpath .)"
AUTOCONF_VERSION="$(autoconf -V | grep -Eo 'autoconf \(GNU Autoconf\) ([[:dig'\
'it:]\.])+' | sed -e 's/[^0-9\.]\+//g')"
mkdir -p dependencies{,/{build,usr}} && cd dependencies/build && pwd
wget 'http://ftp.gnu.org/gnu/autoconf/autoconf-latest.tar.gz'
tar -xzf 'autoconf-latest.tar.gz'
cd autoconf-2*
./configure --prefix="$(realpath ../../usr)" && make # && make check && \
make install
export PATH="$(realpath ../../usr/bin):${PATH}"
cd "${old_cwd}" && unset old_cwd
printf "Old Autoconf version: ${AUTOCONF_VERSION}\n" 
AUTOCONF_VERSION="$(autoconf -V | grep -Eo 'autoconf \(GNU Autoconf\) ([[:dig'\
'it:]\.])+' | sed -e 's/[^0-9\.]\+//g')"
printf "New Autoconf version: ${AUTOCONF_VERSION}\n" && unset AUTOCONF_VERSION
