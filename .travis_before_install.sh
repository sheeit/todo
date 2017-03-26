#!/usr/bin/env sh

#old_cwd="$(realpath .)"
#AUTOCONF_VERSION="$(autoconf -V | grep -Eo 'autoconf \(GNU Autoconf\) ([[:dig'\
#'it:]\.])+' | sed -e 's/[^0-9\.]\+//g')"
#mkdir -p dependencies{,/{build,usr}} && cd dependencies/build && pwd
#wget 'http://ftp.gnu.org/gnu/autoconf/autoconf-latest.tar.gz'
#tar -xzf 'autoconf-latest.tar.gz'
#cd autoconf-2*
#./configure --prefix="$(realpath ../../usr)" && make # && make check && \
#make install
#export PATH="$(realpath ../../usr/bin):${PATH}"
#cd "${old_cwd}" && unset old_cwd
#printf "Old Autoconf version: ${AUTOCONF_VERSION}\n" 
#AUTOCONF_VERSION="$(autoconf -V | grep -Eo 'autoconf \(GNU Autoconf\) ([[:dig'\
#'it:]\.])+' | sed -e 's/[^0-9\.]\+//g')"
#printf "New Autoconf version: ${AUTOCONF_VERSION}\n" && unset AUTOCONF_VERSION

prepare_dependencies_dir() {
    mkdir -p dependencies{,/{build,usr}};
}
download_source_tarballs() {
    URLS='
        http://ftp.gnu.org/gnu/autoconf/autoconf-latest.tar.gz
        http://ftp.gnu.org/gnu/coreutils/coreutils-8.27.tar.xz
    ';
    pushd dependencies/build;
    for url in ${URLS}; do
        wget "${url}";
    done;
    popd;
    unset URLS;
}
extract() {
    if [ -z "${1}" ]; then
        printf 'Usage: extract file\n';
        return 1;
    fi;
    ext="$(printf '%s' "${1}" | sed -e 's/^.*\.\([^\.]\+\)$/\1/g')"
    printf 'File %s\t ext=%s\n' "${1}" "${ext}";
    if [ "${ext}" = 'gz' ] || [ "${ext}" = 'tgz' ]; then
        tar -xzf "${1}";
    elif [ "${ext}" = 'bz2' ] || [ "${ext}" = 'tbz' ]; then
        tar -xjf "${1}";
    elif [ "${ext}" = 'xz' ] || [ "${ext}" = 'txz' ]; then
        tar -xJf "${1}";
    elif [ "${ext}" = 'tar' ]; then
        tar -xf "${1}";
    else
        printf 'extract(): An error occurred.\n';
    fi;
    if [ "${?}" -eq 0 ]; then
        rm -f "${1}";
    fi;
    unset ext
}
build_package_from_source() {
    if [ -z "${2}" ]; then
        printf 'Usage: build_package_from_source directory prefix\n';
        return 1;
    fi;
    if [ ! -d "${1}" ]; then
        printf 'build_package_from_source(): %s No such directory.\n' "${1}";
        return 1;
    fi;
    pushd "${1}";
    if [ ! -d "${2}" ]; then
        rm -rf "${2}";
        mkdir -p "${2}";
    fi;
    if [ -x './configure' ]; then
        ./configure --prefix="${2}";
    else
        printf 'build_package_from_source(): No configure script in %s\n'     \
            "${1}";
        return 1;
    fi;
    if [ -f 'Makefile' ]; then
        make;
    else
        printf 'build_package_from_source(): configure seems to have failed%s'\
            '; no Makefile present.\n';
        return 1;
    fi;
    make install;
    popd;
}
install_from_source() {
    TARBALLS='
        autoconf-latest.tar.gz
        coreutils-8.27.tar.xz
    '
    SOURCEDIRS='
        autoconf-2.69
        coreutils-8.27
    '
    pushd dependencies/build;
    for tarball in ${TARBALLS}; do
        extract "${tarball}";
    done;
    popd
    for sourcedir in ${SOURCEDIRS}; do
        build_package_from_source './dependencies/build/'"${sourcedir}"       \
            './dependencies/usr'
    if [ "${?}" -eq 0 ]; then
        export OLD_PATH="${PATH}";
        export PATH='./dependencies/usr/bin:'"${PATH}";
        export OLD_LIBRARY_PATH="${LIBRARY_PATH}";
        export LIBRARY_PATH='./dependencies/usr/lib:/usr/local/lib:/usr/lib';
        export OLD_CPATH="${CPATH}";
        export CPATH='./dependencies/usr/include:/usr/local/include:/usr/incl'\
'ude';
    fi;
    unset TARBALLS;
    unset SOURCEDIRS;
}

prepare_dependencies_dir;
download_source_tarballs;
install_from_source;
realpath --version
autoconf --version
