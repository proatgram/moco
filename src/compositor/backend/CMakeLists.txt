add_library(moco_backend_Keyboard
    "${CMAKE_CURRENT_SOURCE_DIR}/Keyboard.cpp"
)
add_library(moco::backend::Keyboard ALIAS moco_backend_Keyboard)

target_include_directories(moco_backend_Keyboard
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/compositor/backend>
        $<INSTALL_INTERFACE:include/compositor/backend>
)

target_link_libraries(moco_backend_Keyboard
    PUBLIC
        wayland-server++
        wayland-server-extra++
        PkgConfig::libinput
        moco::Events
)

add_library(moco_backend_LibInput
    "${CMAKE_CURRENT_SOURCE_DIR}/LibInput.cpp"
)
add_library(moco::backend::LibInput ALIAS moco_backend_LibInput)

target_include_directories(moco_backend_LibInput
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/compositor/backend>
        $<INSTALL_INTERFACE:include/compositor/backend>
)

target_link_libraries(moco_backend_LibInput
    PUBLIC
        wayland-server++
        wayland-server-extra++
        PkgConfig::libinput
        moco::Events
)
