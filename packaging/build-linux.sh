#!/usr/bin/env bash
set -euo pipefail
format=${1:?Usage: build-linux.sh DEB|RPM output-directory}
out=${2:?Missing output directory}
case "$format" in DEB|RPM) ;; *) echo 'Expected DEB or RPM' >&2; exit 2;; esac
mkdir -p "$out"
out=$(realpath "$out")
build="$out/build"
cmake -S . -B "$build" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build "$build" --parallel "${BUILD_JOBS:-2}"
ctest --test-dir "$build" --output-on-failure
cpack --config "$build/CPackConfig.cmake" -G "$format" -B "$out"

# Inspect dependency metadata and verify the installed executable and notices.
case "$format" in
    DEB) dpkg-deb --field "$out"/*.deb Package Version Architecture Depends ;;
    RPM) rpm -qp --requires "$out"/*.rpm ;;
esac
stage="$out/install-check"
DESTDIR="$stage" cmake --install "$build"
test -x "$stage/usr/bin/cursedap"
test -f "$stage/usr/share/man/man1/cursedap.1"
test -f "$stage/usr/share/licenses/cursedap/PFFFT"
"$stage/usr/bin/cursedap" --help
