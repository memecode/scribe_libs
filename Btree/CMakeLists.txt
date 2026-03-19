include(../common.cmake)

add_library(btree SHARED
    btree.c
    btree_delete.c
    btree_header.c
    btree_insert.c
    btree_lock.c
    btree_node.c
    btree_search.c
    btree_traverse.c
    config.c
    db.c
    db_blocklist.c
    db_blocks.c
    db_cache.c
    db_header.c
    db_lock.c
    db_utils.c
    gbtree.cpp
    hashtable.c
    offsetlist.c)

target_compile_definitions(btree
    PRIVATE
        STDC_HEADERS)

if (WIN32)
    set_target_properties(btree PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

install(TARGETS btree
    RUNTIME
        DESTINATION ${CMAKE_BINARY_DIR}/lib
    LIBRARY
        DESTINATION ${CMAKE_BINARY_DIR}/lib
    ARCHIVE
		DESTINATION ${CMAKE_BINARY_DIR}/lib)