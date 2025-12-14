add_library(moco_compositor
    "${CMAKE_CURRENT_SOURCE_DIR}/moco.cpp"
)
add_library(moco::compositor ALIAS moco_compositor)

target_include_directories(moco_compositor
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/compositor>
        $<INSTALL_INTERFACE:include/compositor>
)

target_link_libraries(moco_compositor
    PUBLIC
        wayland-server++
        wayland-server-extra++
)

add_library(moco_helper_Matrix INTERFACE)
add_library(moco::helper::Matrix ALIAS moco_helper_Matrix)

target_include_directories(moco_helper_Matrix
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/compositor/helper>
        $<INSTALL_INTERFACE:include/compositor/helper>
)

add_library(moco_Events INTERFACE)
add_library(moco::Events ALIAS moco_Events)

target_include_directories(moco_Events
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/compositor>
        $<INSTALL_INTERFACE:include/compositor>
)

add_subdirectory("wayland")
add_subdirectory("backend")
