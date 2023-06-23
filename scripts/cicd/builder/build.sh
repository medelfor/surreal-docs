#!/bin/bash

#
# Copyright © 2022, Medelfor, Limited. All rights reserved.
# For any additional information refer to https://medelfor.com
#

branch=$1
mbs_root=$2
template=$3

#must be called from the project's root

function Check-ExitCode {
    if [ $? -ne 0 ]; then
        exit 1
    fi
}

source ~/.nvm/nvm.sh
mkdir -p build_release
mkdir -p build_debug

###############
### RELEASE ###
###############

cd build_release

rm -rf dist
rm -rf dist_conan
rm -rf dist_tests
rm -rf dist_static

# build resources
gulp default -f ../gulpfile.js --destination build_release/dist --auto_res_prefix build_release
Check-ExitCode

# build dist, tests, conan package
cmake -DCMAKE_BUILD_TYPE=Release ../ -DCMAKE_INSTALL_PREFIX=dist -DDO_BUILD_TESTS=ON \
      -DDO_INSTALL_AS_CONAN_PACKAGE=ON -DCONAN_PACKAGE_INSTALL_POSTFIX=_conan \
      -DTESTS_INSTALL_POSTFIX=_tests -DRESOURCES_DIR=dist
Check-ExitCode

cmake --build . --config Release
Check-ExitCode

cmake --install . --config Release
Check-ExitCode

# clean symlinks to script modules in order for them not to be packaged
gulp clean-symlinks -f ../gulpfile.js --destination build_release/dist --auto_res_prefix build_release
Check-ExitCode

# copy bundled resources to dist_conan
mkdir -p dist_conan/res/bundled
cp -rv dist/res/. dist_conan/res/bundled

# copy dev resources to dist_conan
mkdir -p dist_conan/res/dev
cp -rv ../res/. dist_conan/res/dev

# remove theme/* (except for html) from dev
rm -rfv dist_conan/res/dev/misc/graphics/udocs_processor/theme/
rm -rfv dist_conan/res/dev/scripts/udocs_processor/theme/
rm -rfv dist_conan/res/dev/styles/udocs_processor/theme/

# move theme/* (except for html) from bundled to a separate artifact
mkdir -p dist_static/misc/graphics/udocs_processor/theme/
mkdir -p dist_static/scripts/udocs_processor/theme/
mkdir -p dist_static/styles/udocs_processor/theme/
mv -fv dist_conan/res/bundled/misc/graphics/udocs_processor/theme/* dist_static/misc/graphics/udocs_processor/theme/
mv -fv dist_conan/res/bundled/scripts/udocs_processor/theme/* dist_static/scripts/udocs_processor/theme/
mv -fv dist_conan/res/bundled/styles/udocs_processor/theme* dist_static/styles/udocs_processor/

# package the conan package

cd dist_conan

conan export-pkg ../../ medelfor/$branch -s os=Linux -s compiler="gcc" \
      -s compiler.version=11 -s compiler.libcxx=libstdc++11 -s build_type=Release -s arch=x86_64 \
      -j ../install_udocs-processor.json --build-folder=. --source-folder=. \
      --profile ../../conanprofiles/udocs-processor-release-linux -o udocs-processor:Shared=True

cd ../..

#############
### DEBUG ###
#############

cd build_debug

rm -rf dist_conan

# build conan package
cmake -DCMAKE_BUILD_TYPE=Debug ../ -DCMAKE_INSTALL_PREFIX=dist -DDO_BUILD_TESTS=OFF \
      -DDO_INSTALL_AS_CONAN_PACKAGE=ON -DCONAN_PACKAGE_INSTALL_POSTFIX=_conan \
      -DTESTS_INSTALL_POSTFIX=_tests
Check-ExitCode

cmake --build . --config Debug
Check-ExitCode

cmake --install . --config Debug
Check-ExitCode

# copy bundled and dev resources to dist_conan
mkdir -p dist_conan/res
cp -r ../build_release/dist_conan/res/. dist_conan/res

# package the conan package (debug)

cd dist_conan

conan export-pkg ../../ medelfor/$branch -s os=Linux -s compiler="gcc" \
      -s compiler.version=11 -s compiler.libcxx=libstdc++11 -s build_type=Debug -s arch=x86_64 \
      -j ../install_udocs-processor.json --build-folder=. --source-folder=. \
      --profile ../../conanprofiles/udocs-processor-debug-linux -o udocs-processor:Shared=True
Check-ExitCode

#it's copied here instead of Install directive, because it's actually not a part of the project
#but just a mark for CI/CD system and it can be as well absent (it's guaranteed that the
#project works without it)
cd ../../
cp githash build_release/dist/githash
Check-ExitCode
