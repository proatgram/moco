#pragma once

#include "BackendBase.hpp"

#include <libinput.h>

namespace moco::backend {
    class LibInput : public BackendBase<LibInput> {
        public:
            enum class Events {
                KeyboardKey
            };

            LibInput(Private, ::wayland::server::display_t display);
            ~LibInput();

        private:
            static const libinput_interface s_libinputInterface;
            static auto OpenRestricted(const char *path, int flags, void *userData) -> int;
            static auto CloseRestricted(int fd, void *userData) -> void;

            auto BackendLoop() -> void final;
            auto HandleKeyboardEvent(libinput_event_keyboard *event) -> void;

            libinput *m_libinputHandle;
            udev *m_udevHandle;
    };
}  // namespace moco::backend
