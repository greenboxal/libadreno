set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/Debug/libadreno)

add_library (adreno
    ail/ailc.c
    ail/lex.ail_.c
    ail/y.tab.c
    crypt/blowfish.c
    memory.c
    utils/array.c
    utils/bitarray.c
    utils/hashtable.c
    utils/memorypool.c
    utils/memorystream.c
    vm/emit.c
    vm/script.c
    vm/stack.c
    vm/stdlib.c
    vm/value.c
    vm/vm.c
)
