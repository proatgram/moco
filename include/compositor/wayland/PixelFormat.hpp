#pragma once

#include <wayland-server-protocol.hpp>

namespace moco::wayland::implementation::PixelFormats {
    template<::wayland::server::shm_format Format>
    struct PixelFormat;

    /**
     * @brief 32-bit ARGB format
     * @details [31:0] A:R:G:B 8:8:8:8 little endian
     *
     */
    template<>
    struct PixelFormat<::wayland::server::shm_format::argb8888> {
        using Size = uint32_t;
        union {
            Size Data;
            struct {
                uint8_t Blue;
                uint8_t Green;
                uint8_t Red;
                uint8_t Alpha;
            };
        };
    };

    /**
     * @brief 32-bit RGB format
     * @details [31:0] x:R:G:B 8:8:8:8 little endian
     *
     */
    template<>
    struct PixelFormat<::wayland::server::shm_format::xrgb8888> {
        using Size = uint32_t;
        union {
            Size Data;
            struct {
                uint8_t Blue;
                uint8_t Green;
                uint8_t Red;
            };
        };
    };

    // Easily declare sizes for different pixel shm formats
    template<::wayland::server::shm_format Format>
    using PixelFormatSize = PixelFormat<Format>::Size;

}  // namespace moco::wayland::implementation::PixelFormats
