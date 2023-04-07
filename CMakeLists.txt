cmake_minimum_required(VERSION 3.4...3.20 FATAL_ERROR)

set(LIBNAME axme)

project(${LIBNAME})

# global settings
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
string(COMPARE EQUAL ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR} AXME_STANDALONE)
option(AXME_BUILD_EXAMPLES "Build the AXME example programs" ${AXME_STANDALONE})

# cxx standard
if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()

# the axme lib
file(GLOB_RECURSE AXME_SRC ./media/*.h;./media/*.cpp;./media/*.mm)

if(APPLE)
    set_source_files_properties(media/MediaEngine.cpp PROPERTIES LANGUAGE OBJCXX)
    include_directories(/System/Library/Frameworks)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    find_library(AVFOUNDATION_LIBRARY AVFoundation)
    find_library(COREMEDIA_LIBRARY CoreMedia)
    set(PLATFORM_SPECIFIC_LIBS
        ${COREVIDEO_LIBRARY}
        ${AVFOUNDATION_LIBRARY}
        ${COREMEDIA_LIBRARY}
        )
endif()

add_library(${LIBNAME} STATIC ${AXME_SRC} yasio/yasio.natvis)
target_compile_definitions(${LIBNAME} PUBLIC AXME_NO_AXMOL=1)
# target_compile_definitions(${LIBNAME} PUBLIC AXME_USE_IMFME=1)
target_include_directories(${LIBNAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})

if (APPLE)
    foreach(platform_lib ${PLATFORM_SPECIFIC_LIBS})
        target_link_libraries(axme ${platform_lib})
    endforeach()
endif()

# examples
if (AXME_BUILD_EXAMPLES)
    # glad
    add_subdirectory(glad)

    # glfw
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    add_subdirectory(glfw)
    add_executable(meplay main.cpp)
    target_link_libraries(meplay glfw glad ${LIBNAME})
endif()
