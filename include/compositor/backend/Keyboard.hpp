#pragma once

#include "BackendBase.hpp"

#include <any>
#include <libinput.h>

namespace moco::backend {
    class Keyboard : public BackendBase<Keyboard> {
        public:
            Keyboard(Private, ::wayland::server::display_t display);

        private:
            auto EventKeyboardKey(std::any eventData) -> void;
    };
}  // namespace moco::backend
