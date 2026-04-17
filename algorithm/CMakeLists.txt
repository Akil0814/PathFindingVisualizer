option(PATHFINDER_USE_CUSTOM_IMPLEMENTATIONS "Compile algorithm/*.cpp learning implementations instead of algorithm/impl/*.cpp." OFF)

set(ALG
    path_finder.cpp
)

set(PATHFINDER_ALGORITHMS
    a_star
    bfs
    custom
    dijkstra
    greedy
)

foreach(ALG_NAME IN LISTS PATHFINDER_ALGORITHMS)
    set(CUSTOM_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/${ALG_NAME}_pathfinder.cpp")
    set(IMPL_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/impl/${ALG_NAME}_pathfinder_impl.cpp")

    if(PATHFINDER_USE_CUSTOM_IMPLEMENTATIONS OR NOT EXISTS "${IMPL_SOURCE}")
        list(APPEND ALG "${CUSTOM_SOURCE}")
    else()
        list(APPEND ALG "${IMPL_SOURCE}")
    endif()
endforeach()

add_library(algorithmLib ${ALG})

if(PATHFINDER_USE_CUSTOM_IMPLEMENTATIONS)
    target_compile_definitions(algorithmLib PUBLIC PATHFINDER_USE_CUSTOM_IMPLEMENTATIONS)
endif()
