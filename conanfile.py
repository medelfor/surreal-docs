#
#  Copyright © 2022, Medelfor, Limited. All rights reserved.
#  For any additional information refer to https://medelfor.com
#

# This conanfile.py is supposed to package *pre-built* binaries

import shutil
from conans import ConanFile
import os


class UDocsProcessorConan(ConanFile):
    name = "udocs-processor"
    url = "https://medelfor.com"
    license = "Copyright © 2022, Medelfor, Limited. All rights reserved."
    description = "Library that converts output of DoxygenUE and Unrealistic Docs UE" \
                  " plugin into beautiful JSON+PNG/HTML+PNG"

    version = os.getenv("CONAN_PACKAGE_VERSION", "")

    if len(version) == 0:
        version = "unspecified"

    settings = {
        "os": ["Windows", "Linux"],
        "compiler": None,
        "build_type": None,
        "arch": None
    }
    options = {"Shared": [True, False]}
    default_options = {"Shared": True}

    requires = [("spdlog/1.10.0"),
        "md4c/0.4.8",
        "boost/1.80.0",
        "ftxui/4.0.0",
        "grpc/1.50.0",
        "poco/1.12.2",
        "date/3.0.1",
        "termcolor/2.1.0",
        "cryptopp/8.7.0",
        "openssl/1.1.1t",
        "api-proto/1.2.0@medelfor/master",
        "surreal-api-proto/1.0.0@medelfor/master",
        ("cli11/2.0.0", "private"),
        ("base64/0.4.0", "private"),
        "inja/3.3.0",
        ("crc32c/1.1.2", "private"),
        ("nlohmann_json/3.11.2", "private"),
        ("stringtoolbox/0.0.4", "private")]
    build_requires = "gtest/cci.20210126"

    generators = "cmake_find_package"

    def imports(self):
        self.copy("*.dll", dst="deps", src="bin")
        self.copy("*.dll", dst="deps", src="lib")
        self.copy("*.so", dst="deps", src="bin")
        self.copy("*.so", dst="deps", src="lib")
        self.copy("*.dll", dst="deps", src="bin")
        self.copy("*.dll", dst="deps", src="lib")
        self.copy("*.so", dst="deps", src="bin")
        self.copy("*", dst="execs", src="bin")
        self.copy("*.so", dst="deps", src="lib")
        shutil.rmtree("external_res", True)
        self.copy("*", dst="external_res", src="res")

    def package(self):
        self.copy("*", "include", "include")  # headers
        self.copy("*", "lib", "lib")  # static libs
        self.copy("*.so", "lib", "bin")  # static libs
        self.copy("*", "bin", "bin")  # static libs
        self.copy("*", "res", "res")  # resources

    def package_info(self):
        self.cpp_info.libs = ["M-UDocs-Processor", "M-UDocs-Report",
            "M-UDocs-Serializer", "M-UDocs-Base", "M-UDocs-Util"]
