add_executable(moco)
target_sources(moco
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

target_link_libraries(moco
    PRIVATE
        moco::compositor
)

add_subdirectory("compositor")
