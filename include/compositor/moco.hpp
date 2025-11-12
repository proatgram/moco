#pragma once

#include <wayland-server.hpp>
#include <wayland-server-protocol.hpp>

namespace moco::compositor {
    class Compositor {
        public:
            Compositor();       
            ~Compositor();

        private:
            wayland::server::display_t m_display;

            // Wayland globals

            std::string m_socket;
    };
} // namespace moco::compositor
