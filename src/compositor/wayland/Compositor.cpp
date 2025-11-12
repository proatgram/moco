#include "Compositor.hpp"

using namespace moco::wayland::implementation;
using namespace wayland::server;

Compositor::Compositor(display_t display) :
    global_compositor_t(display)
{
    on_bind() = [this](client_t client, compositor_t compositor) -> void {HandleBind(client, compositor);};
}

auto Compositor::HandleBind(client_t client, compositor_t compositor) -> void {
    compositor.on_create_surface() = [this](surface_t surface) -> void {HandleCreateSurface(surface);};
    compositor.on_create_region() = [this](region_t region) -> void {HandleCreateRegion(region);};
}

auto Compositor::HandleCreateSurface(surface_t surface) -> void {

}

auto Compositor::HandleCreateRegion(region_t region) -> void {
    
}
