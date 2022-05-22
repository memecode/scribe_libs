# cmake -G "Visual Studio 16 2019" -A x64

cmake_minimum_required (VERSION 3.5)
cmake_policy(SET CMP0054 NEW)
project(libiconv)

set(DLL_VARIABLE "DLL_VARIABLE")
set(ICONV_CONST "const")
set(USE_MBSTATE_T "1")
set(BROKEN_WCHAR_H "0")
set(HAVE_WCHAR_T "1")
set(EILSEQ "42")
string(CONCAT DLL_EXPORT
	"#ifdef _WIN32\n"
	"    #ifdef _DLL\n"
	"    #define DLL_VARIABLE __declspec(dllexport)\n"
	"    #else\n"
	"    #define DLL_VARIABLE __declspec(dllimport)\n"
	"    #endif\n"
	"#else\n"
	"    #define DLL_VARIABLE\n"
	"#endif")

configure_file(	"include/iconv.h.in"
				"include/iconv.h")
configure_file(	"srclib/uniwidth.in.h"
				"srclib/uniwidth.h")
configure_file(	"srclib/unitypes.in.h"
				"srclib/unitypes.h")

# I've had to edit this and commit the edited version, this was the generation step:
#file(READ config.h.in CONFIG_H)
#string(REPLACE "#undef ICONV_CONST" "#define ICONV_CONST const" CONFIG_H "${CONFIG_H}")
#string(REPLACE "#undef EILSEQ" "#define EILSEQ 42" CONFIG_H "${CONFIG_H}")
#file(WRITE config.h "${CONFIG_H}")

# libiconv library
add_library(libiconv
	SHARED
		config.h
		lib/iconv.c
		libcharset/lib/localcharset.c
		srclib/progname.c
		srclib/error.c
		windows/iconv.def)
target_include_directories(libiconv
	PUBLIC
		.
		include
		srclib)
target_compile_definitions(libiconv
	PRIVATE
		"_DLL"
		"_CRT_SECURE_NO_WARNINGS")
set_target_properties(libiconv PROPERTIES PREFIX "")

# iconv executable
add_executable(iconv
	config.h
	src/iconv.c
	srclib/safe-read.c
	srclib/uniwidth/width.c
	gnulib-local/lib/xmalloc.c
	include/iconv.h)
target_include_directories(iconv
	PUBLIC
		.
		include
		srclib)
target_link_libraries(iconv libiconv)
target_compile_definitions(iconv
	PRIVATE
		"_CRT_SECURE_NO_WARNINGS")
