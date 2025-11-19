#include "Surface.hpp"

using namespace moco::wayland::implementation;
using namespace wayland::server;

Surface::Surface(surface_t surface, Private) :
    Surface(surface)
{

}

Surface::Surface(surface_t surface) :
    ObjectImplementationBase(surface)
{

}
