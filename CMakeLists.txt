cmake_minimum_required(VERSION 3.21)
project (lvgl)

# lvgl
#add_subdirectory(lvgl)
file(GLOB_RECURSE EX_SOURCES lvgl/*.c)
add_library(lvgl STATIC ${EX_SOURCES})

# lv_examples
file(GLOB_RECURSE EX_SOURCES lv_examples/*.c)
add_library(lv_examples STATIC ${EX_SOURCES})

# lv_drivers
file(GLOB_RECURSE DR_SOURCES lv_drivers/*.c)
add_library(lv_drivers STATIC ${DR_SOURCES})

if (0)
# quickjs
file(GLOB QU_SOURCES quickjs/quickjs-eval.c quickjs/quickjs.c quickjs/cutils.c quickjs/libregexp.c quickjs/libbf.c quickjs/libunicode.c)
add_library(quickjs STATIC ${QU_SOURCES})
set(QUICKJS_VERSION '"1.0.0"')
set_property(TARGET quickjs PROPERTY COMPILE_FLAGS -DCONFIG_VERSION=${QUICKJS_VERSION})
# get_target_property(FLAG quickjs COMPILE_FLAGS)
# message(STATUS "${FLAG}")

# quickjs lvgl binding
file(GLOB_RECURSE BI_SOURCES quickjs-lvgl-binding/*.c)
add_library(quickjs_lvgl_binding STATIC ${BI_SOURCES})
target_include_directories(quickjs_lvgl_binding PUBLIC
    "${PROJECT_SOURCE_DIR}/quickjs"
    "${PROJECT_SOURCE_DIR}/lvgl"
)
endif()

# jerryscript build
set(JERRY_DEBUGGER ON CACHE BOOL "debugger on")
set(JERRY_LINE_INFO ON CACHE BOOL "line info on")
set(JERRY_CPOINTER_32_BIT ON CACHE BOOL "32 bit pointer")
set(JERRY_GLOBAL_HEAP_SIZE 102400 CACHE STRING "heap size")
set(JERRY_SYSTEM_ALLOCATOR ON CACHE BOOL "system allocator")
set(JERRY_ERROR_MESSAGES ON CACHE BOOL "error messages")
set(JERRY_MEM_STATS ON CACHE BOOL "memory stats")
set(JERRY_LOGGING ON CACHE BOOL "logging")

add_subdirectory(jerryscript)

# jerry lvgl binding
file(GLOB JE_SOURCES jerry-lvgl-binding/*.c)
add_library(jerry_lvgl_biding STATIC ${JE_SOURCES})
target_include_directories(jerry_lvgl_biding PUBLIC
    "${PROJECT_SOURCE_DIR}/lvgl"
    "${PROJECT_SOURCE_DIR}/jerryscript/jerry-core/include"
    "${PROJECT_SOURCE_DIR}/jerryscript/jerry-ext/include"
)

if (0)
# jerry lib
add_library(jerry_core STATIC IMPORTED)
# find_library(jerry_lib NAMES jerry-core PATHS "${PROJECT_SOURCE_DIR}/jerry")
# message(STATUS "${jerry_lib}")
set_target_properties(jerry_core PROPERTIES
  IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/jerryscript/build/lib/libjerry-core.a"
)
add_library(jerry_ext STATIC IMPORTED)
set_target_properties(jerry_ext PROPERTIES
  IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/jerryscript/build/lib/libjerry-ext.a"
)
endif()

file(GLOB_RECURSE TESTS tests/*.c)
add_library(tests STATIC ${TESTS})
target_include_directories(tests PUBLIC
    "${PROJECT_SOURCE_DIR}/lvgl"
    "${PROJECT_SOURCE_DIR}/jerryscript/jerry-core/include"
    "${PROJECT_SOURCE_DIR}/jerryscript/jerry-ext/include"
    "${PROJECT_SOURCE_DIR}/jerry-lvgl-binding"
    "${PROJECT_SOURCE_DIR}"
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -s USE_SDL=2")

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/lvgl
    ${PROJECT_SOURCE_DIR}/quickjs
)

# file(GLOB_RECURSE INCLUDES "./*.h")
# file(GLOB_RECURSE LIB_SOURCES "lvgl/*.c" "lv_examples/*.c" "lv_drivers/*.c")
file(GLOB MY_SOURCES "./*.c")
list(FILTER MY_SOURCES EXCLUDE REGEX quickjs)
# set(SOURCES ${LIB_SOURCES} ${MY_SOURCES})

# add_executable(index ${MY_SOURCES})
add_executable(index ${MY_SOURCES})
message(STATUS ${MY_SOURCES})

target_include_directories(index PUBLIC 
    "${PROJECT_BINARY_DIR}" 
    "${PROJECT_SOURCE_DIR}/lvgl_examples" 
    "${PROJECT_SOURCE_DIR}/lvgl" 
    "${PROJECT_SOURCE_DIR}/lvgl_drivers"
    # "${PROJECT_SOURCE_DIR}/quickjs"
    # "${PROJECT_SOURCE_DIR}/quickjs-lvgl-binding"
    "${PROJECT_SOURCE_DIR}/jerry-lvgl-binding"
    "${PROJECT_SOURCE_DIR}/jerryscript/jerry-core/include"
)

target_link_libraries(index PUBLIC 
    lvgl lv_drivers lv_examples 
    # quickjs quickjs_lvgl_binding 
    jerry_lvgl_biding
    "jerry-core" "jerry-ext"
    # jerryscript
    tests
)

if(NOT LVGL_CHOSEN_DEMO)
    set(LVGL_CHOSEN_DEMO lv_demo_widgets)
endif()
set_source_files_properties(main.c PROPERTIES COMPILE_FLAGS -DCHOSEN_DEMO=${LVGL_CHOSEN_DEMO})
# set_property(TARGET index PROPERTY COMPILE_FLAGS "${COMPILE_FLAGS} -s EXPORTED_FUNCTIONS='[\"_eval\"]' -s EXPORTED_RUNTIME_METHODS='[\"ccall\",\"cwrap\"]'")

set(CMAKE_EXECUTABLE_SUFFIX ".html")
set(CMAKE_EXECUTABLE_SUFFIX ".js")
set_target_properties(index PROPERTIES LINK_FLAGS "-s ALLOW_MEMORY_GROWTH=1 --shell-file ${PROJECT_SOURCE_DIR}/lvgl_shell.html -s SINGLE_FILE=1 -s EXPORTED_FUNCTIONS=_eval,_run,_main,_socket,_bi_test_main -s EXPORTED_RUNTIME_METHODS=cwrap,ccall -lwebsocket.js -s ASYNCIFY")
# get_target_property(FLAG index LINK_FLAGS)
# message(STATUS "${FLAG}")
