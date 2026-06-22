#!/bin/bash
set -e

BUILD_DIR="build"
INSTALL_PREFIX="$HOME/warcraft-server"

case "$1" in
  clean)
    rm -rf "$BUILD_DIR"
    ;;

  configure)
    mkdir -p "$BUILD_DIR"

    BUILD_TYPE=Release
    C_FLAGS="-march=native -O3"
    CXX_FLAGS="-march=native -O3"
    BUILD_TESTING=0

    for opt in "$@"; do
      case "$opt" in
        --debug)
          BUILD_TYPE=Debug
          C_FLAGS="-march=native"
          CXX_FLAGS="-march=native"
          ;;
        --tests)
          BUILD_TESTING=1
          ;;
      esac
    done

    cmake -S . -B "$BUILD_DIR" \
      -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      -DCMAKE_C_FLAGS="$C_FLAGS" \
      -DCMAKE_CXX_FLAGS="$CXX_FLAGS" \
      -DCMAKE_C_COMPILER_LAUNCHER=ccache \
      -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
      -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=gold" \
      -G Ninja \
      -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
      -DBUILD_TESTING="$BUILD_TESTING" \
      -DELUNA=0 \
      -DTOOLS=0
    ;;

  build)
    if [ ! -d "$BUILD_DIR" ]; then
      echo "Error: build directory '$BUILD_DIR' does not exist. Run 'configure' first." >&2
      exit 1
    fi
    TARGETS="authserver worldserver"
    if grep -q "BUILD_TESTING:BOOL=ON" "$BUILD_DIR/CMakeCache.txt" 2>/dev/null; then
      TARGETS="$TARGETS tests tests-scripts"
    fi
    cmake --build "$BUILD_DIR" --target $TARGETS
    ;;

  test)
    if [ ! -d "$BUILD_DIR" ]; then
      echo "Error: build directory '$BUILD_DIR' does not exist. Run 'configure --tests' first." >&2
      exit 1
    fi
    cd "$BUILD_DIR"
    ctest --output-on-failure "$@"
    ;;

  install)
    mkdir -p "$INSTALL_PREFIX/bin"
    if [ -f "$INSTALL_PREFIX/bin/authserver" ]; then
      mv "$INSTALL_PREFIX/bin/authserver" "$INSTALL_PREFIX/bin/authserver_old"
      echo "Backed up authserver -> authserver_old"
    fi
    cmake --install "$BUILD_DIR" --prefix "$INSTALL_PREFIX"

    BIN_DIR="$INSTALL_PREFIX/bin"
    VERSIONED_DIR="$BIN_DIR/worldservers"
    TIMESTAMP=$(date +%Y%m%d-%H%M)
    WORLDVERSION="worldserver-$TIMESTAMP"

    mkdir -p "$VERSIONED_DIR"
    cp "$BIN_DIR/worldserver" "$VERSIONED_DIR/$WORLDVERSION"
    ln -sf "$VERSIONED_DIR/$WORLDVERSION" "$BIN_DIR/worldserver"
    echo "Installed worldserver -> worldservers/$WORLDVERSION"

    find "$VERSIONED_DIR" -name 'worldserver-*' -mtime +7 -delete 2>/dev/null || true
    echo "Cleaned up worldserver versions older than 7 days"
    ;;

  *)
    echo "Usage: $0 {configure|build|clean|install|test}"
    echo "  configure [--debug] [--tests]  - configure build tree"
    echo "    --debug   Debug build (default: Release)"
    echo "    --tests   Enable unit test targets (auto-included in 'build')"
    echo "  build                 - compile (includes test targets when configured with --tests)"
    echo "  clean                 - remove build directory"
    echo "  install               - install built binaries to $INSTALL_PREFIX (archives worldserver with timestamp, prunes >7 days)"
    echo "  test [<ctest args>]   - run unit tests via ctest"
    ;;
esac
