cmake_minimum_required(VERSION 3.22.1)
project(nerdphone LANGUAGES CXX)
add_library(nerdphone SHARED engine.cpp sha256_armv8.cpp)
target_compile_features(nerdphone PRIVATE cxx_std_17)
target_compile_options(nerdphone PRIVATE -O3 -Wall -Wextra)

# Optional ARMv8 SHA-256 hardware path.
# The crypto extensions are enabled for ONE translation unit only: with
# -march=armv8-a+crypto on the whole library the process would die with SIGILL
# on any CPU that does not implement SHA256H/SHA256H2.
if(ANDROID_ABI STREQUAL "arm64-v8a")
  set_source_files_properties(sha256_armv8.cpp PROPERTIES
    COMPILE_OPTIONS "-march=armv8-a+crypto")
  target_compile_definitions(nerdphone PRIVATE NERDPHONE_HAVE_SHA2_HW=1)
endif()
