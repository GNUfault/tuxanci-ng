#!/bin/sh
set -e

PREFIX=../..
BUILD_TYPE=Release
CPUS=$(nproc)
DESTDIR=$(pwd)/install

# Clean previous build
rm -rf ${PREFIX}/build build .flatpak-builder repo install io.github.GNUfault.tuxanci-x86_64.flatpak

# Make build directory if it doesn't exist
mkdir -p ${PREFIX}/build

# Change to the build directory
cd ${PREFIX}/build

# Configure the project.
cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=/app

# Build the project
make -j${CPUS}

# Install the files
make install DESTDIR=${DESTDIR}

# Change back to the packaging directory
cd ../packaging/flatpak

# Build the Flatpak package
flatpak-builder --repo=repo build manifest.json

# Create the Flatpak bundle
flatpak build-bundle repo io.github.GNUfault.tuxanci-x86_64.flatpak io.github.GNUfault.tuxanci

# Clean up build artifacts
rm -rf ${PREFIX}/build build .flatpak-builder install repo
