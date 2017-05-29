#!/usr/bin/env sh

prepare_dependencies_dir() {
    mkdir -p dependencies dependencies/build;
}
download_source_tarballs() {
    URLS='
        http://ftp.gnu.org/gnu/autoconf/autoconf-latest.tar.gz
        http://ftp.gnu.org/gnu/coreutils/coreutils-8.27.tar.xz
    ';
    old_pwd="$(pwd)";
    cd dependencies/build || exit;
    for url in ${URLS}; do
        wget "${url}";
    done;
    cd "${old_pwd}" || exit;
    unset old_pwd URLS;
}
extract() {
    if [ -z "${1}" ]; then
        printf 'Usage: extract file\n';
        return 1;
    fi;
    ext="$(printf '%s' "${1}" | sed -e 's/^.*\.\([^\.]\+\)$/\1/g')";
    printf 'File %s\t ext=%s\n' "${1}" "${ext}";
    if                                                                        \
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
        fi; then
        rm -f "${1}";
    fi;
    unset ext;
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
    old_pwd="$(pwd)";
    cd "${1}" || exit;
    if [ ! -d "${2}" ]; then
        rm -rf "${2}";
        mkdir -p "${2}";
    fi;
    if [ -x './configure' ]; then
        ./configure --prefix="${2}" 1>/dev/null 2>&1;
    else
        printf 'build_package_from_source(): No configure script in %s\n'     \
            "${1}";
        return 1;
    fi;
    if [ -f 'Makefile' ]; then
        make -s 1>/dev/null 2>&1;
    else
        printf 'build_package_from_source(): configure seems to have failed%s'\
            '; no Makefile present.\n';
        return 1;
    fi;
    sudo make -s install 1>/dev/null 2>&1;
    cd "${old_pwd}" || exit;
    unset old_pwd;
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
    old_pwd="$(pwd)";
    cd dependencies/build || exit;
    for tarball in ${TARBALLS}; do
        extract "${tarball}";
    done;
    cd "${old_pwd}" || exit;
    if                                                                        \
        for sourcedir in ${SOURCEDIRS}; do
            build_package_from_source './dependencies/build/'"${sourcedir}"   \
                '/usr/local'
        done; then
        export OLD_PATH="${PATH}";
        export PATH='/usr/local/bin:'"${PATH}";
        export OLD_LIBRARY_PATH="${LIBRARY_PATH}";
        export LIBRARY_PATH='/usr/local/lib:/usr/lib';
        export OLD_CPATH="${CPATH}";
        export CPATH='/usr/local/include:/usr/include';
    fi;
    unset old_pwd SOURCEDIRS TARBALLS;
}

prepare_dependencies_dir;
download_source_tarballs;
install_from_source;
realpath --version
autoconf --version
