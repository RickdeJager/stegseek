#!/bin/bash

##################################################
# Basic packaging script
##################################################
#
# Creates a very basic package, meant for Github releases
# It won't hold up to Debian/Ubuntu standards, but will still
# be useful as a release
#

NAME="stegseek"
VER="0.4.1"
REV="1"
VERSION="${VER}-${REV}"
PACKAGE="${NAME}_${VERSION}"
INSTALL_PATH="${PACKAGE}/usr/local/bin/"

# Create a relative root
mkdir -p ${INSTALL_PATH}
# Copy the binary over
cp "build/src/${NAME}" ${INSTALL_PATH}

# Create a DEBIAN directory
mkdir -p "${PACKAGE}/DEBIAN"

# Copy the control file over
cp package/control "${PACKAGE}/DEBIAN"
# Edit the version
sed -i "s/<<VERSION>>/${VERSION}/g" "${PACKAGE}/DEBIAN/control"

# Build the package
dpkg-deb --build ${PACKAGE}
