#!/usr/bin/env bash

set -e

shopt -s extglob

if [ -z "$1" ] || [ -z "$2" ]; then
  echo "Usage: $0 version destdir [--no-package] [--dev-build]"
  exit 1
fi

DXVK_VERSION="$1"
DXVK_SRC_DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd -P)"
DXVK_BUILD_DIR=${2}"/dxvk-macOS-async-$DXVK_VERSION"
DXVK_ARCHIVE_PATH=${2}"/dxvk-macOS-async-$DXVK_VERSION.tar.gz"

if [ -e "$DXVK_BUILD_DIR" ]; then
  echo "Build directory $DXVK_BUILD_DIR already exists"
  exit 1
fi

shift 2

opt_nopackage=0
opt_devbuild=0
opt_buildid=false

crossfile="build-win"

while [ $# -gt 0 ]; do
  case "$1" in
  "--no-package")
    opt_nopackage=1
    ;;
  "--dev-build")
    opt_nopackage=1
    opt_devbuild=1
    ;;
  "--build-id")
    opt_buildid=true
    ;;
  *)
    echo "Unrecognized option: $1" >&2
    exit 1
  esac
  shift
done

function build_arch {
  export WINEARCH="win$1"
  export WINEPREFIX="$DXVK_BUILD_DIR/wine.$1"
  
  cd "$DXVK_SRC_DIR"

  opt_strip=
  if [ $opt_devbuild -eq 0 ]; then
    opt_strip=--strip
  fi

  meson setup --cross-file "$DXVK_SRC_DIR/$crossfile$1.txt" \
        --buildtype "release"                               \
        --prefix "$DXVK_BUILD_DIR"                          \
        $opt_strip                                          \
        --bindir "x$1"                                      \
        --libdir "x$1"                                      \
        -Dbuild_id=$opt_buildid                             \
        "$DXVK_BUILD_DIR/build.$1"

  cd "$DXVK_BUILD_DIR/build.$1"
  ninja install

  if [ $opt_devbuild -eq 0 ]; then
    # get rid of some useless .a files
    rm "$DXVK_BUILD_DIR/x$1/"*.!(dll)
    rm -R "$DXVK_BUILD_DIR/build.$1"
  fi
}

function copy_extras {
  cp "$DXVK_SRC_DIR/dxvk.conf" "$DXVK_BUILD_DIR/dxvk.conf"
}

function package {
  cd "$DXVK_BUILD_DIR/.."
  tar -czf "$DXVK_ARCHIVE_PATH" "dxvk-macOS-async-$DXVK_VERSION"
  rm -R "dxvk-macOS-async-$DXVK_VERSION"
}

build_arch 64
build_arch 32
copy_extras

if [ $opt_nopackage -eq 0 ]; then
  package
fi
