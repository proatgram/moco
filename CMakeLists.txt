cmake_minimum_required(VERSION 3.31)
project(moco
    VERSION 0.1
    DESCRIPTION "A mobile oriented Wayland Compositor written in C++"
    HOMEPAGE_URL "https://github.com/proatgram/moco"
    LANGUAGES "CXX"
)

set(CMAKE_CXX_STANDARD 23)

find_package(PkgConfig REQUIRED)
find_package(waylandpp REQUIRED)

pkg_check_modules(libdrm REQUIRED IMPORTED_TARGET GLOBAL libdrm)
pkg_check_modules(pixman REQUIRED IMPORTED_TARGET GLOBAL pixman-1)
pkg_check_modules(xkbcommon REQUIRED IMPORTED_TARGET GLOBAL xkbcommon)

add_subdirectory("src")
