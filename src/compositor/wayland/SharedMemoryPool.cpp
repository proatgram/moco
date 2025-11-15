#include "SharedMemoryPool.hpp"

#include <format>
#include <iostream>

#include <sys/mman.h>

using namespace moco::wayland::implementation;
using namespace wayland::server;

SharedMemoryPool::SharedMemoryPool(shm_pool_t shm_pool) :
    ObjectImplementationBase(shm_pool)
{
    on_destroy() = [this]() -> void {HandleDestroy();};
    on_create_buffer() = [this](buffer_t buffer, int offset, int width, int height, int stride, shm_format format) -> void {HandleCreateBuffer(buffer, offset, width, height, stride, format);};
    on_resize() = [this](int size) -> void {HandleResize(size);};
}

SharedMemoryPool::SharedMemoryPool(shm_pool_t shm_pool, SharedMemoryPool::Private) : SharedMemoryPool(shm_pool) {}

// Only destructed when all buffers have been destroyed and HandleDestroy
// has been called because of the shared_ptr ownership
SharedMemoryPool::~SharedMemoryPool() {
    int result = munmap(memorySpace.data(), memorySpace.size());
    if (result == -1) {
        std::cerr << __PRETTY_FUNCTION__ << ":"
                  << std::system_error(std::error_code(errno, std::system_category())).what() 
                  << std::endl;
            
    }
}

/* Request Handlers */

auto SharedMemoryPool::HandleDestroy() -> void {
    /* Nothing to do (yet) */
}

auto SharedMemoryPool::HandleCreateBuffer(buffer_t buffer, int offset, int width, int height, int stride, shm_format format) -> void {
    switch (format) {
        case shm_format::argb8888:
            SharedMemoryBuffer<shm_format::argb8888>::Create(buffer)->AssignData(memorySpace.subspan(offset, height * stride), shared_from_this())->SetBufferFormat(height, width, stride);
            break;

        case shm_format::xrgb8888:
            SharedMemoryBuffer<shm_format::xrgb8888>::Create(buffer)->AssignData(memorySpace.subspan(offset, height * stride), shared_from_this())->SetBufferFormat(height, width, stride);
        default:
            std::cerr << __PRETTY_FUNCTION__
                      << ": Unsupported compositor pixel format."
                      << std::endl;
            break;
    }
}

auto SharedMemoryPool::HandleResize(size_t newSize) -> void {
    // Per Wayland spec the new size for the memory mapped space must be
    // larger, not smaller than what it currently is.
    // https://wayland.app/protocols/wayland#wl_shm_pool:request:resize
    if (newSize < memorySpace.size()) {
        throw std::length_error(std::format("{}: newSize ({}) is smaller than current size ({}). Resize must be an extension to the size, not a reduction.", 
                __PRETTY_FUNCTION__,
                newSize,
                memorySpace.size()));
    }

    // MREMAP_MAYMOVE -> Potentially move all data if there exists a mapping in the space
    // where we would extend into.
    // Using this would also cause us to need to update each buffer created with a new address
    // which becomes annoying
    void *addr = mremap(memorySpace.data(), memorySpace.size(), newSize, 0 /* MREMAP_MAYMOVE */);
    if (addr == MAP_FAILED) {
        throw std::system_error(std::error_code(errno, std::system_category()));
    }

    memorySpace = std::span<uint8_t>(static_cast<uint8_t*>(addr), newSize);
}

/* Implementation */

auto SharedMemoryPool::Assign(std::span<uint8_t> data) -> void {
    memorySpace = data;
}
