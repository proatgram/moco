#include "Buffer.hpp"

#include "SharedMemoryPool.hpp"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace moco::wayland::implementation;
using namespace wayland::server;

SharedMemoryBuffer::SharedMemoryBuffer(buffer_t buffer, Private) :
    SharedMemoryBuffer(buffer) {}

SharedMemoryBuffer::SharedMemoryBuffer(buffer_t buffer) :
    ObjectImplementationBase(buffer)
{
    on_destroy() = [this]() -> void {HandleDestroy();};
}

auto SharedMemoryBuffer::HandleDestroy() -> void {
    /* Nothing to do (yet) */
}

auto SharedMemoryBuffer::AssignData(std::span<uint8_t> data, std::shared_ptr<SharedMemoryPool> parentPool) -> void {
    bufferData = data;
    parentPool = parentPool;
}

auto SharedMemoryBuffer::SetBufferFormat(size_t bufferHeight, size_t bufferWidth, size_t bufferStride, shm_format format) -> void {
    bufferHeight = bufferHeight;
    bufferWidth = bufferWidth;
    bufferStride = bufferStride;
    bufferFormat = format;
}

auto SharedMemoryBuffer::GetData() const -> std::span<uint8_t> {
    return bufferData;
}
