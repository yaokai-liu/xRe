cmake_minimum_required(VERSION 3.24)
project(xRe-new C)

set(CMAKE_C_STANDARD 23)


add_compile_definitions("XTYPES_USING_STDC_TYPE")
add_compile_definitions("XDEF_USING_C_STD_DEF")
add_compile_definitions("XCHAR_USING_ASCII")

add_compile_definitions(DEBUG)

add_library(xRe-new
        char_t.c char_t.h
        structs.c structs.h
        parse.c parse.h allocator.h)

target_compile_options(xRe-new PRIVATE "-fno-builtin")
target_link_options(xRe-new PRIVATE "-nodefaultlibs")
target_link_options(xRe-new PRIVATE "-nostdlib")

#add_executable(test_xRe-new test.c)
#target_link_libraries(test_xRe-new PRIVATE xRe)
