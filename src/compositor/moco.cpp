#include "moco.hpp"

using namespace moco::compositor;
using namespace wayland::server;

#include <iostream>
#include <string>

Compositor::Compositor() :
    m_display()
{
    // Connect to socket named `wayland-1` just for testing purposes
    m_socket = m_display.add_socket("wayland-1");
}

Compositor::~Compositor() {
    m_display.terminate();
}
