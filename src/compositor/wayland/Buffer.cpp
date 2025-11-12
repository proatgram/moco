#include "Buffer.hpp"

#include "SharedMemoryPool.hpp"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace moco::wayland::implementation;
using namespace wayland::server;

SharedMemoryBuffer::SharedMemoryBuffer(::wayland::server::buffer_t buffer) :
    ObjectImplementationBase(buffer)
{
    on_destroy() = [this]() -> void {HandleDestroy();};
}

auto SharedMemoryBuffer::HandleDestroy() -> void {
    /* Nothing to do (yet) */
}

auto SharedMemoryBuffer::AssignData(std::span<uint8_t> data, std::shared_ptr<SharedMemoryPool> parent) -> void {
    m_objectData->bufferData = data;
    m_objectData->parentPool = parent;
}

auto SharedMemoryBuffer::SetBufferFormat(size_t bufferHeight, size_t bufferWidth, size_t bufferStride, shm_format format) -> void {
    m_objectData->bufferHeight = bufferHeight;
    m_objectData->bufferWidth = bufferWidth;
    m_objectData->bufferStride = bufferStride;
    m_objectData->bufferFormat = format;
}

auto SharedMemoryBuffer::GetData() const -> std::span<uint8_t> {
    return m_objectData->bufferData;
}
