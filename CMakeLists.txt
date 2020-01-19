# used features:
#   FindPython3 (3.12+)
#   MSVC_RUNTIME_LIBRARY (3.15+)
cmake_minimum_required(VERSION 3.15)

# tldr; this is required when setting MSVC_RUNTIME_LIBRARY (/MT, /MD, ...) property
# see also: https://cmake.org/cmake/help/latest/policy/CMP0091.html#policy:CMP0091
cmake_policy(SET CMP0091 NEW)


project(seismo_reader LANGUAGES CXX)


include_directories(src)

set(HEADERS
  src/data_conversion.h
  src/data_types.h
  src/obj_base.h
  src/segy_bin_header.h
  src/segy_file.h
  src/segy_header_map.h
  src/segy_reader.h
  src/segy_smart_trc_buffer.h
  src/segy_trace.h
  src/segy_trace_header.h
  src/seismic_abstract_header.h
  src/seismic_cube.h
  src/seismic_data_provider.h
  src/seismic_exception.h
  src/seismic_geometry.h
  src/seismic_header_field.h
  src/seismic_header_map.h
  src/seismic_trace.h
  src/seismic_trace_header.h
  src/utils.h
)

set(SOURCES
  src/base.cpp
  src/data_conversion.cpp
  src/data_types.cpp
  src/py_mudule.cpp
  src/segy_bin_header.cpp
  src/segy_file.cpp
  src/segy_header_map.cpp
  src/segy_reader.cpp
  src/segy_smart_trc_buffer.cpp
  src/segy_trace.cpp
  src/segy_trace_header.cpp
  src/utils.cpp
)

# Python3 (NB: you can use Python3_ROOT_DIR cmake var as a hint)
find_package(Python3 COMPONENTS Development REQUIRED)

# add python module; this implies handling of .pyd/.so ext and include dirs/link libraries
Python3_add_library(seismo_reader MODULE ${HEADERS} ${SOURCES})


### properties

target_compile_definitions(seismo_reader PRIVATE PYTHON)

set_target_properties(seismo_reader
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ../bin
    LIBRARY_OUTPUT_DIRECTORY ../bin
    RUNTIME_OUTPUT_DIRECTORY ../bin
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
if (WIN32)
  # use /MT[d] instead of /MD[d]
  set_property(TARGET seismo_reader PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  # _snprintf
  target_compile_definitions(seismo_reader PRIVATE HAVE_SNPRINTF)
else()
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()


### dependencies

# Eigen, pybind11
include_directories(lib/Eigen lib/pybind11/include)

# boost
find_package(Boost REQUIRED filesystem system)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(seismo_reader PRIVATE ${Boost_LIBRARIES})
endif()