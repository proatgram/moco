#pragma once

#include <wayland-server-protocol.hpp>

#include <iostream>

namespace moco::wayland::implementation::PixelFormats {
    enum class Format {
        ARGB8888,
        XRGB8888,
        NOT_SUPPORTED
    };

    template<auto Format, typename Enable = void>
    struct PixelFormat;

    /**
     * @brief 32-bit ARGB format
     * @details [31:0] A:R:G:B 8:8:8:8 little endian
     *
     */
    template<>
    struct PixelFormat<Format::ARGB8888> {
        using Size = uint32_t;
        Format DataFormat = Format::ARGB8888;
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
    template<>
    struct PixelFormat<::wayland::server::shm_format::argb8888> : public PixelFormat<Format::ARGB8888> {};

    /**
     * @brief 32-bit RGB format
     * @details [31:0] x:R:G:B 8:8:8:8 little endian
     *
     */
    template<>
    struct PixelFormat<Format::XRGB8888> {
        using Size = uint32_t;
        Format DataFormat = Format::XRGB8888;
        union {
            Size Data;
            struct {
                uint8_t Blue;
                uint8_t Green;
                uint8_t Red;
            };
        };
    };
    template<>
    struct PixelFormat<::wayland::server::shm_format::xrgb8888> : public PixelFormat<Format::XRGB8888> {};

    // Easily declare sizes for different pixel shm formats
    template<auto Format>
    using PixelFormatSize = PixelFormat<Format>::Size;

    template<typename T>
    inline auto GetFormat(T format) -> Format {
        if constexpr (std::is_same_v<T, ::wayland::server::shm_format>) {
            switch (format) {
                case ::wayland::server::shm_format::argb8888:
                    return Format::ARGB8888;
                case ::wayland::server::shm_format::xrgb8888:
                    return Format::XRGB8888;
                default:
                    std::cerr << __PRETTY_FUNCTION__
                              << ": Unsupported compositor pixel format."
                              << std::endl;
                    return Format::NOT_SUPPORTED;
            }
        } else if constexpr (std::is_same_v<T, Format>) {
            return format;
        } else {
            std::cerr << __PRETTY_FUNCTION__
                      << ": Unsupported compositor pixel format."
                      << std::endl;
            return Format::NOT_SUPPORTED;
        }
    }
}  // namespace moco::wayland::implementation::PixelFormats
