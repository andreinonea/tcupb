# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/andra/source/repos/tcupb/out/build/x64-Debug/_deps/dataframe-src"
  "C:/Users/andra/source/repos/tcupb/out/build/x64-Debug/_deps/dataframe-build"
  "C:/Users/andra/source/repos/tcupb/out/build/x64-Debug/_deps/dataframe-subbuild/dataframe-populate-prefix"
  "C:/Users/andra/source/repos/tcupb/out/build/x64-Debug/_deps/dataframe-subbuild/dataframe-populate-prefix/tmp"
  "C:/Users/andra/source/repos/tcupb/out/build/x64-Debug/_deps/dataframe-subbuild/dataframe-populate-prefix/src/dataframe-populate-stamp"
  "C:/Users/andra/source/repos/tcupb/out/build/x64-Debug/_deps/dataframe-subbuild/dataframe-populate-prefix/src"
  "C:/Users/andra/source/repos/tcupb/out/build/x64-Debug/_deps/dataframe-subbuild/dataframe-populate-prefix/src/dataframe-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/andra/source/repos/tcupb/out/build/x64-Debug/_deps/dataframe-subbuild/dataframe-populate-prefix/src/dataframe-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/andra/source/repos/tcupb/out/build/x64-Debug/_deps/dataframe-subbuild/dataframe-populate-prefix/src/dataframe-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
