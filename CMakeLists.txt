cmake_minimum_required(VERSION 3.25)
project(O_o_OOH5 LANGUAGES C CXX)

# Standalone builds fetch the compatible core. When integrated, the host supplies it.
if(NOT TARGET oor::core)
    include(FetchContent)
    set(OOR_FORMATS "")
    set(OOR_BUILD_TESTS OFF)
    set(OOR_BUILD_PYTHON OFF)
    set(OOR_BUILD_WASM OFF)
    FetchContent_Declare(oor_reader
        GIT_REPOSITORY https://github.com/jcdulas/O_o_Reader.git
        GIT_TAG 04b1bda11a05a2696d97cf5859944f5f87063f01)
    FetchContent_MakeAvailable(oor_reader)
endif()

add_library(oor_format_ooh5 STATIC
    src/formats/Ooh5.cpp
    src/results/core/ReadResults.cpp
    src/query/table/Hdf5Export.cpp
    src/results/archive/ArchiveModel.cpp
    src/results/archive/ArchiveResultFile.cpp
    src/results/archive/ArchiveWriter.cpp
    src/io/hdf5/Hdf5.cpp
    src/io/hdf5/Hdf5GlobalHeapCheck.cpp
    src/io/hdf5/Hdf5Internal.cpp
    src/io/hdf5/Hdf5StreamWriter.cpp
    src/io/hdf5/Hdf5Table.cpp
    src/io/hdf5/Hdf5Tree.cpp
    src/io/hdf5/Hdf5Writer.cpp)
add_library(oor::ooh5 ALIAS oor_format_ooh5)
target_compile_features(oor_format_ooh5 PUBLIC cxx_std_20)
set_target_properties(oor_format_ooh5 PROPERTIES POSITION_INDEPENDENT_CODE ON CXX_EXTENSIONS OFF)
target_include_directories(oor_format_ooh5 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(oor_format_ooh5 PUBLIC oor::core)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(oor_format_ooh5 PRIVATE -Wall -Wextra -Wpedantic)
    if(OOR_WARNINGS_AS_ERRORS)
        target_compile_options(oor_format_ooh5 PRIVATE -Werror)
    endif()
endif()
if(EMSCRIPTEN)
    target_compile_options(oor_format_ooh5 PUBLIC -sMEMORY64=1 -fwasm-exceptions)
    target_link_options(oor_format_ooh5 PUBLIC -sMEMORY64=1 -fwasm-exceptions)
endif()
if(EMSCRIPTEN)
    set(OOR_HDF5_WASM_ROOT "$ENV{OOR_HDF5_WASM_ROOT}" CACHE PATH "Location of the Memory64 HDF5 installation")
    set(hdf5_archive "${OOR_HDF5_WASM_ROOT}/lib/libhdf5.a")
    if(NOT EXISTS "${hdf5_archive}")
        message(FATAL_ERROR "Set OOR_HDF5_WASM_ROOT to an HDF5 installation built for Memory64")
    endif()
    target_include_directories(oor_format_ooh5 PRIVATE "${OOR_HDF5_WASM_ROOT}/include")
    target_link_libraries(oor_format_ooh5 PRIVATE "${hdf5_archive}")
    target_compile_options(oor_format_ooh5 PRIVATE -sUSE_ZLIB=1)
    target_link_options(oor_format_ooh5 PUBLIC -sUSE_ZLIB=1)
else()
    find_package(HDF5 REQUIRED COMPONENTS C)
    find_package(ZLIB REQUIRED)
    target_link_libraries(oor_format_ooh5 PRIVATE HDF5::HDF5 ZLIB::ZLIB)
endif()

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
    if(BUILD_TESTING AND NOT EMSCRIPTEN)
        add_executable(ooh5_round_trip tests/RoundTrip.cpp)
        target_link_libraries(ooh5_round_trip PRIVATE oor::ooh5)
        add_test(NAME ooh5.round-trip COMMAND ooh5_round_trip "${CMAKE_CURRENT_BINARY_DIR}/round-trip.ooh5")
    endif()
endif()
