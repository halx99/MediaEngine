cmake_minimum_required(VERSION 3.20)

project(axplay)

# global settings
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
string(COMPARE EQUAL ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR} AXME_STANDALONE)
option(AXME_BUILD_EXAMPLES "Build the AXME example programs" ${AXME_STANDALONE})

# cxx standard
if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
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

set(LIBNAME axme)
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

    # axplay
    set(target_name axplay)
    file(GLOB_RECURSE leanopengl_src ${CMAKE_CURRENT_LIST_DIR}/learnopengl/*.h)
    message(STATUS "leanopengl_src=${leanopengl_src}")
    add_executable(${target_name} main.cpp ${leanopengl_src})
    target_link_libraries(${target_name} glfw glad ${LIBNAME})
    set_target_properties(${target_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

    configure_file(root_directory.h.in ${CMAKE_BINARY_DIR}/root_directory.h)
    target_include_directories(${target_name} PRIVATE ${CMAKE_BINARY_DIR})

    # copy shader files to build directory
    file(GLOB_RECURSE SHADERS
        "${CMAKE_CURRENT_LIST_DIR}/Content/*.vert"
        "${CMAKE_CURRENT_LIST_DIR}/Content/*.frag"
    )

    if(AXPLAY_ENABLE_SPIRV_LINT)
       # enable glslcc for check shader spirv compatible
       set(GLSLCC_FIND_PROG_ROOT "${CMAKE_CURRENT_LIST_DIR}/tools/external/glslcc")
       include(cmake/AXGLSLCC.cmake)
       ax_target_compile_shaders(${target_name} FILES ${SHADERS})
    else()
       target_sources(${target_name} PRIVATE ${SHADERS})
    endif()
    
	# copy dlls
	# file(GLOB DLLS "dlls/*.dll")
    # foreach(SHADER ${SHADERS})
    #     if(WIN32)
    #         # configure_file(${SHADER} "test")
    #         add_custom_command(TARGET ${target_name} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} ${CMAKE_BINARY_DIR})
    #         # add_custom_command(TARGET ${target_name} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} $<TARGET_FILE_DIR:${target_name}>)
	# 		# add_custom_command(TARGET ${target_name} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${DLLS} $<TARGET_FILE_DIR:${target_name}>)
    #     elseif(UNIX AND NOT APPLE)
    #         file(COPY ${SHADER} DESTINATION ${CMAKE_SOURCE_DIR}/bin/${target_name})
    #     elseif(APPLE)
    #         # create symbolic link for *.vs *.fs *.gs
    #         get_filename_component(SHADERNAME ${SHADER} NAME)
    #         makeLink(${SHADER} ${CMAKE_SOURCE_DIR}/bin/${SHADERNAME} ${NAME})
    #     endif(WIN32)
    # endforeach(SHADER)
    add_custom_command(TARGET ${target_name} POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_SOURCE_DIR}/Content" $<TARGET_FILE_DIR:${target_name}>/Content)
    # execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_SOURCE_DIR}/Content" $<TARGET_FILE_DIR:${target_name}>/Content)

    if(WIN32)
        set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "${target_name}")
    endif()
endif()
