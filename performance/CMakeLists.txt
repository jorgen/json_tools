include_directories(${JSON_STRUCT_INCLUDE_DIR})

add_executable(benchmark 
    benchmark.cpp
    include/simdjson/simdjson.cpp)
target_compile_definitions(benchmark PRIVATE CATCH_CONFIG_ENABLE_BENCHMARKING)

add_library(catch_main_benchmark STATIC catch-main.cpp)
target_compile_definitions(catch_main_benchmark PRIVATE CATCH_CONFIG_ENABLE_BENCHMARKING)
target_link_libraries(benchmark catch_main_benchmark)
                
