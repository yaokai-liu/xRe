cmake_minimum_required(VERSION 3.24)
project(xRe C)

set(CMAKE_C_STANDARD 23)
#set(CMAKE_BUILD_TYPE Release)
#add_compile_options("-O3")

add_compile_definitions("XTYPES_USING_STDC_TYPE")
add_compile_definitions("XDEF_USING_C_STD_DEF")
add_compile_definitions("XRE_CHAR_USING_ASCII")
add_compile_definitions("XRE_DEBUG")
add_compile_definitions("XRE_BASIC_ALLOCATE_LENGTH")

add_library(xRe
        char_t.c char_t.h
        string.c string.h
        structs.c structs.h
        parse.c parse.h
        match.c match.h
        compile.c compile.h
        xregexp.c xregexp.h
        alloc.h)

target_compile_options(xRe PRIVATE "-fno-builtin")
target_link_options(xRe PRIVATE "-nodefaultlibs")
target_link_options(xRe PRIVATE "-nostdlib")

add_library(xReVM rvm.c rvm.h)
target_compile_options(xRe PRIVATE "-fno-builtin")
target_link_options(xRe PRIVATE "-nodefaultlibs")
target_link_options(xRe PRIVATE "-nostdlib")
target_link_libraries(xReVM PRIVATE xRe)
target_include_directories(xReVM PRIVATE .)

add_executable(test_xRe test.c)
target_link_libraries(test_xRe PRIVATE xRe)
#target_compile_options(test_xRe PRIVATE "-fno-builtin")
