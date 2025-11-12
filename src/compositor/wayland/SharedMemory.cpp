#include "SharedMemory.hpp"

#include <sys/mman.h>

using namespace moco::wayland::implementation;
using namespace wayland::server;

SharedMemory::SharedMemory(shm_t shm) :
    ObjectImplementationBase(shm)
{
    on_create_pool() = [this](shm_pool_t pool, int32_t fd, size_t size) -> void {HandleCreatePool(pool, fd, size);};
    on_destroy() = [this]() -> void {HandleDestroy();};
}

SharedMemory::SharedMemory(shm_t shm, Private) : SharedMemory(shm) {}

auto SharedMemory::HandleCreatePool(shm_pool_t pool, int32_t fd, size_t size) -> void {
    std::shared_ptr<SharedMemoryPool> sharedMemoryPool = SharedMemoryPool::Create(pool);

    // For now no excecution permissions, not sure if it's neccisary
    void *addr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        throw std::system_error(std::error_code(errno, std::system_category()));
        
    }

    sharedMemoryPool->Assign(std::span<uint8_t>(static_cast<uint8_t*>(addr), size));
}

auto SharedMemory::HandleDestroy() -> void {

}
