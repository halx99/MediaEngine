cmake_minimum_required(VERSION 3.4...3.20 FATAL_ERROR)

project(anme)

# global settings
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
string(COMPARE EQUAL ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR} ANME_STANDALONE)
option(ANME_BUILD_EXAMPLES "Build the ANME example programs" ${ANME_STANDALONE})

# cxx standard
if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()

# the anme lib
file(GLOB_RECURSE ANME_SRC ./media/*.h;./media/*.cpp;./media/*.mm)
add_library(anme STATIC ${ANME_SRC} yasio/yasio.natvis)
target_compile_definitions(anme PUBLIC ANME_NO_AXMOL=1)
target_include_directories(anme PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# examples
if (ANME_BUILD_EXAMPLES)
    # glad
    add_subdirectory(glad)

    # glfw
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    add_subdirectory(glfw)
    add_executable(meplay main.cpp)
    target_link_libraries(meplay glfw glad anme)
endif()
