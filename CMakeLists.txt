cmake_minimum_required(VERSION 3.11)
project("LFU_Cache")

SET(LFU_SRCS
    source/test_LFU.cpp; 
    source/cache_assistance.cpp
)

SET(Ideal_SRCS
    source/test_Ideal.cpp; 
    source/cache_assistance.cpp
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(COMPILE_FLAGS "-Wall" "-g")

add_library(LFUlib STATIC ${LFU_SRCS})
add_library(Ideallib STATIC ${Ideal_SRCS})

target_include_directories(LFUlib PUBLIC "include")
target_include_directories(Ideallib PUBLIC "include")

add_executable(LFU source/test_LFU.cpp)
add_executable(Ideal source/test_Ideal.cpp)

target_compile_definitions(LFU PRIVATE
    $<$<CONFIG:Debug>:Debug=1>
    $<$<CONFIG:Release>:Release=1>
)

target_compile_definitions(Ideal PRIVATE
    $<$<CONFIG:Debug>:Debug=1>
    $<$<CONFIG:Release>:Release=1>
)

target_compile_options(LFU PRIVATE ${COMPILE_FLAGS})
target_compile_options(Ideal PRIVATE ${COMPILE_FLAGS})

target_link_libraries(LFU LFUlib)
target_link_libraries(Ideal Ideallib)



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