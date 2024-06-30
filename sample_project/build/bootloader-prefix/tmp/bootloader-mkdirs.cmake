# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/Lucas/esp/v5.2.1/esp-idf/components/bootloader/subproject"
  "/Users/Lucas/Documents/Engineering/SmartWatch/sample_project/build/bootloader"
  "/Users/Lucas/Documents/Engineering/SmartWatch/sample_project/build/bootloader-prefix"
  "/Users/Lucas/Documents/Engineering/SmartWatch/sample_project/build/bootloader-prefix/tmp"
  "/Users/Lucas/Documents/Engineering/SmartWatch/sample_project/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/Lucas/Documents/Engineering/SmartWatch/sample_project/build/bootloader-prefix/src"
  "/Users/Lucas/Documents/Engineering/SmartWatch/sample_project/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/Lucas/Documents/Engineering/SmartWatch/sample_project/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/Lucas/Documents/Engineering/SmartWatch/sample_project/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
