#pragma once

#include "ObjectImplementationBase.hpp"

#include "wayland-server-protocol.hpp"

namespace moco::wayland::implementation {
    class Seat : public ObjectImplementationBase<::wayland::server::seat_t, Seat> {
            using ObjectImplementationBase::on_destroy;
            using ObjectImplementationBase::on_get_keyboard;
            using ObjectImplementationBase::on_get_pointer;
            using ObjectImplementationBase::on_get_touch;
            using ObjectImplementationBase::on_release;

        public:
            Seat(::wayland::server::seat_t, Private);

        private:
            Seat(::wayland::server::seat_t seat);

            auto HandleGetPointer(::wayland::server::pointer_t pointer) -> void;
            auto HandleGetKeyboard(::wayland::server::keyboard_t keyboard) -> void;
            auto HandleGetTouch(::wayland::server::touch_t touch) -> void;
            auto HandleRelease() -> void;
    };

    class GlobalSeat : public ::wayland::server::global_seat_t {
            using ::wayland::server::global_seat_t::on_bind;
        public:
            GlobalSeat(::wayland::server::display_t display);

        private:
            static auto OnBind(::wayland::server::client_t client, ::wayland::server::seat_t seat) -> void;

    };
}  // namespace moco::wayland::implementation
