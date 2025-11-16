#include "Compositor.hpp"

#include "Region.hpp"

using namespace moco::wayland::implementation;
using namespace wayland::server;

Compositor::Compositor(compositor_t compositor, Private) :
    Compositor(compositor) {}

Compositor::Compositor(compositor_t compositor) :
    ObjectImplementationBase(compositor)
{
    on_create_surface() = [this](::wayland::server::surface_t surface) -> void {HandleCreateSurface(surface);};
    on_create_region() = [this](::wayland::server::region_t region) -> void {HandleCreateRegion(region);};
    on_destroy() = [this]() -> void {HandleDestroy();};
}

auto Compositor::HandleCreateSurface(surface_t surface) -> void {

}

auto Compositor::HandleCreateRegion(region_t region) -> void {
    Region::Create(region);
}

auto Compositor::HandleDestroy() -> void {
    /* Nothing to do (yet) */
}



GlobalCompositor::GlobalCompositor(display_t display) :
    global_compositor_t(display)
{
    on_bind() = HandleBind;
}

auto GlobalCompositor::HandleBind(client_t client, compositor_t compositor) -> void {
    Compositor::Create(compositor);
}
