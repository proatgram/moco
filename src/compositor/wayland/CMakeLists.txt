add_library(moco_wayland_compositor
    "${CMAKE_CURRENT_SOURCE_DIR}/Compositor.cpp"
)
add_library(moco::wayland::compositor ALIAS moco_wayland_compositor)

target_include_directories(moco_wayland_compositor
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/compositor/wayland>
        $<INSTALL_INTERFACE:include/compositor/wayland>
)

target_link_libraries(moco_wayland_compositor
    PUBLIC
        wayland-server++
        wayland-server-extra++
        PkgConfig::libdrm
        moco::wayland::Region
)

add_library(moco_wayland_SharedMemory
    "${CMAKE_CURRENT_SOURCE_DIR}/SharedMemory.cpp"
)
add_library(moco::wayland::SharedMemory ALIAS moco_wayland_SharedMemory)

target_include_directories(moco_wayland_SharedMemory
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/compositor/wayland>
        $<INSTALL_INTERFACE:include/compositor/wayland>
)

target_link_libraries(moco_wayland_SharedMemory
    PUBLIC
        moco::wayland::SharedMemoryPool
        wayland-server++
        wayland-server-extra++
)

add_library(moco_wayland_SharedMemoryPool
    "${CMAKE_CURRENT_SOURCE_DIR}/SharedMemoryPool.cpp"
)
add_library(moco::wayland::SharedMemoryPool ALIAS moco_wayland_SharedMemoryPool)

target_include_directories(moco_wayland_SharedMemoryPool
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/compositor/wayland>
        $<INSTALL_INTERFACE:include/compositor/wayland>
)

target_link_libraries(moco_wayland_SharedMemoryPool
    PUBLIC
        wayland-server++
        wayland-server-extra++
)

add_library(moco_wayland_Region
    "${CMAKE_CURRENT_SOURCE_DIR}/Region.cpp"
)
add_library(moco::wayland::Region ALIAS moco_wayland_Region)

target_include_directories(moco_wayland_Region
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/compositor/wayland>
        $<INSTALL_INTERFACE:include/compositor/wayland>
)

target_link_libraries(moco_wayland_Region
    PUBLIC
        wayland-server++
        wayland-server-extra++
        PkgConfig::pixman
)

add_library(moco_wayland_Surface
    "${CMAKE_CURRENT_SOURCE_DIR}/Surface.cpp"
)
add_library(moco::wayland::Surface ALIAS moco_wayland_Surface)

target_include_directories(moco_wayland_Surface
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/compositor/wayland>
        $<INSTALL_INTERFACE:include/compositor/wayland>
)

target_link_libraries(moco_wayland_Surface
    PUBLIC
        wayland-server++
        wayland-server-extra++
        PkgConfig::pixman
)

