#include "Region.hpp"

using namespace moco::wayland::implementation;
using namespace wayland::server;

Region::Region(region_t region, Private) :
    Region(region) {}

Region::Region(region_t region) :
    ObjectImplementationBase(region)
{
    on_destroy() = [this]() -> void {HandleDestory();};
    on_add() = [this](int x, int y, int width, int height) -> void {HandleAdd(x, y, width, height);};
    on_subtract() = [this](int x, int y, int width, int height) -> void {HandleSubtract(x, y, width, height);};

    pixman_region32_init(&m_region);
}

Region::~Region() {
    pixman_region32_fini(&m_region);
}

auto Region::HandleDestory() -> void {
    /* Nothing to do (yet) */
}

auto Region::HandleAdd(int x, int y, unsigned int width, unsigned int height) -> void {
    if(!pixman_region32_union_rect(&m_region, &m_region, x, y, width, height)) {
        throw std::runtime_error("Failed to add rectangle to region.");
    }
}

auto Region::HandleSubtract(int x, int y, unsigned int width, unsigned int height) -> void {
    pixman_region32_t sub;
    pixman_region32_init_rect(&sub, x, y, width, height);
    if (!pixman_region32_subtract(&m_region, &m_region, &sub)) {
        pixman_region32_fini(&sub);
        throw std::runtime_error("Failed to subtract rectangle from region.");
    }

    pixman_region32_fini(&sub);
}
