# cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_BUILD_TYPE=Debug ..
cmake_minimum_required (VERSION 3.12)

set(CMAKE_CXX_STANDARD 11)

function(RemoveNDEBUG sym)
	if (1)
		string(REPLACE " -DNDEBUG" "" ${sym} ${${sym}})
		string(REPLACE " /DNDEBUG" "" ${sym} ${${sym}})
		set(${sym} ${${sym}} PARENT_SCOPE)
	endif()
endfunction(RemoveNDEBUG)

RemoveNDEBUG(CMAKE_CXX_FLAGS_MINSIZEREL)
RemoveNDEBUG(CMAKE_CXX_FLAGS_MINSIZEREL_INIT)
RemoveNDEBUG(CMAKE_CXX_FLAGS_RELEASE)
RemoveNDEBUG(CMAKE_CXX_FLAGS_RELEASE_INIT)
RemoveNDEBUG(CMAKE_CXX_FLAGS_RELWITHDEBINFO)
RemoveNDEBUG(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT)
RemoveNDEBUG(CMAKE_C_FLAGS_RELEASE)
RemoveNDEBUG(CMAKE_C_FLAGS_RELEASE_INIT)
RemoveNDEBUG(CMAKE_C_FLAGS_MINSIZEREL)
RemoveNDEBUG(CMAKE_C_FLAGS_MINSIZEREL_INIT)
RemoveNDEBUG(CMAKE_C_FLAGS_RELWITHDEBINFO)
RemoveNDEBUG(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT)

message(STATUS "CMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}")

project(aspell)

set(SRC
	common/cache.cpp common/string.cpp
	common/getdata.cpp common/itemize.cpp common/file_util.cpp
	common/string_map.cpp common/string_list.cpp common/config.cpp
	common/posib_err.cpp common/errors.cpp common/error.cpp
	common/fstream.cpp common/iostream.cpp common/info.cpp
	common/can_have_error.cpp common/convert.cpp
	common/tokenizer.cpp common/speller.cpp
	common/document_checker.cpp common/filter.cpp
	common/objstack.cpp common/strtonum.cpp
	common/gettext_init.cpp common/file_data_util.cpp
	modules/speller/default/readonly_ws.cpp
	modules/speller/default/suggest.cpp
	modules/speller/default/data.cpp
	modules/speller/default/multi_ws.cpp
	modules/speller/default/phonetic.cpp
	modules/speller/default/writable.cpp
	modules/speller/default/speller_impl.cpp
	modules/speller/default/phonet.cpp
	modules/speller/default/typo_editdist.cpp
	modules/speller/default/editdist.cpp
	modules/speller/default/primes.cpp
	modules/speller/default/language.cpp
	modules/speller/default/leditdist.cpp
	modules/speller/default/affix.cpp modules/tokenizer/basic.cpp
	lib/filter-c.cpp lib/word_list-c.cpp lib/info-c.cpp
	lib/mutable_container-c.cpp lib/error-c.cpp
	lib/document_checker-c.cpp lib/string_map-c.cpp
	lib/new_config.cpp lib/config-c.cpp
	lib/string_enumeration-c.cpp lib/can_have_error-c.cpp
	lib/dummy.cpp lib/new_filter.cpp lib/new_fmode.cpp
	lib/string_list-c.cpp lib/find_speller.cpp lib/speller-c.cpp
	lib/string_pair_enumeration-c.cpp lib/new_checker.cpp
	modules/filter/url.cpp modules/filter/email.cpp
	modules/filter/tex.cpp modules/filter/sgml.cpp
	modules/filter/context.cpp modules/filter/nroff.cpp
	modules/filter/texinfo.cpp
	prog/prezip.c
	)

if(WIN32)
	list(APPEND SRC
		win32/dirent.cpp
		win32/api-glue.cpp)
	file(COPY ${CMAKE_CURRENT_LIST_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

add_library(aspell SHARED ${SRC})
target_compile_definitions(aspell PRIVATE ASPELL060_EXPORTS)
target_include_directories(	aspell PUBLIC
							common
							interfaces/cc
							modules/speller/default)
if(WIN32)
	target_include_directories(aspell PUBLIC win32)
else()
	target_include_directories(aspell PUBLIC linux gen)
endif()
	
set_target_properties(aspell PROPERTIES 
	OUTPUT_NAME "aspell-dist-0.60"
	DEBUG_POSTFIX "d")
	

