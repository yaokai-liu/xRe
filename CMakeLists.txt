cmake_minimum_required(VERSION 3.24)
project(xRe C)

set(CMAKE_C_STANDARD 23)
#set(CMAKE_BUILD_TYPE Release)
#add_compile_options("-O3")

add_compile_definitions("XTYPES_USING_STDC_TYPE")
add_compile_definitions("XDEF_USING_C_STD_DEF")
add_compile_definitions("XRE_CHAR_USING_ASCII")
add_compile_definitions("XRE_DEBUG")
add_compile_definitions("XRE_BASIC_ALLOCATE_LENGTH=1")

add_library(xRe
        char_t.c char_t.h
        string.c string.h
        structs.c structs.h
        compile.c compile.h
        xregexp.c xregexp.h
        match.c match.h
        parse.c parse.h
        alloc.h)
target_compile_options(xRe PRIVATE "-fno-builtin")
target_link_options(xRe PRIVATE "-nodefaultlibs")
target_link_options(xRe PRIVATE "-nostdlib")

add_library(xReRT runtime.c runtime.h)
target_compile_options(xRe PRIVATE "-fno-builtin")
target_link_options(xRe PRIVATE "-nodefaultlibs")
target_link_options(xRe PRIVATE "-nostdlib")
target_include_directories(xReRT PRIVATE .)

add_executable(test_rt test_rt.c)
target_link_libraries(test_rt PRIVATE xReRT xRe)
target_compile_options(test_rt PRIVATE "-fno-builtin")
target_link_options(xRe PRIVATE "-nostdlib")

add_executable(test_xRe test.c)
target_link_libraries(test_xRe PRIVATE xRe)
#target_compile_options(test_xRe PRIVATE "-fno-builtin")
