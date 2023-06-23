cd build_release

Remove-Item dist_conan -Recurse -ErrorAction Ignore

# build dist, tests, conan package
cmake -DCMAKE_BUILD_TYPE=Debug ../ -DCMAKE_INSTALL_PREFIX=dist -DDO_BUILD_TESTS=OFF `
      -DDO_BUILD_EXECUTABLE=OFF -DDO_INSTALL_AS_CONAN_PACKAGE=ON -DCONAN_PACKAGE_INSTALL_POSTFIX=_conan `
      -DTESTS_INSTALL_POSTFIX=_tests -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=ON -DSKIP_CLI=ON

cmake --build . --config RelWithDebInfo

cmake --install . --config RelWithDebInfo

mkdir -Force dist_conan

cd dist_conan

conan export-pkg ../../ medelfor/local -s os=Windows -s compiler="Visual Studio" `
      -s compiler.version=16 -s compiler.runtime=MD -s build_type=Release -s arch=x86_64 `
      -j ../install_udocs-processor.json --build-folder=. --source-folder=. `
      --profile ../../conanprofiles/udocs-processor-release-win64 -o udocs-processor:Shared=False -f

cd ../..
