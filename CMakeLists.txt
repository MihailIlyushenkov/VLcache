cmake_minimum_required(VERSION 3.11)
project("LFU_Cache")

SET(SRCS 
    source/main.cpp; 
    #source/LFU_cache.cpp
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(COMPILE_FLAGS
    "-Wall"
)

add_library(LFUlib STATIC ${SRCS})
target_include_directories(LFUlib PUBLIC "include")
add_executable(LFU source/main.cpp)

target_compile_definitions(LFU PRIVATE
$<$<CONFIG:Debug>:TEST=1>
$<$<CONFIG:Release>:Release=1>
)

target_compile_options(LFU PRIVATE ${COMPILE_FLAGS})
target_link_libraries(LFU LFUlib)

#cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
#cmake --build build
#./build/LFU

##индивидуальная работа с таргетами
## allbraces, allperms - библиотеки
#set(APPS allbraces; allperms)
#
#foreach(APP ${APPS})
#    add_executable(${APP} ${APP}.cpp)
#    target_compile_features(${APP} PRIVATE cxx_std_20)
#    target_link_libraries(${APP} frame)
#    target_link_libraries(${APP} comblib)
#endforeach()