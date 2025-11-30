#include "Seat.hpp"

using namespace moco::wayland::implementation;
using namespace wayland::server;

Seat::Seat(seat_t seat, Private) :
    Seat(seat) {}

Seat::Seat(seat_t seat) :
    ObjectImplementationBase(seat)
{
    on_get_pointer() = [this](pointer_t pointer) -> void {HandleGetPointer(pointer);};
    on_get_keyboard() = [this](keyboard_t keyboard) -> void {HandleGetKeyboard(keyboard);};
    on_get_touch() = [this](touch_t touch) -> void {HandleGetTouch(touch);};
    on_release() = [this]() -> void {HandleRelease();};
}

auto Seat::HandleGetPointer(pointer_t pointer) -> void {

}

auto Seat::HandleGetKeyboard(keyboard_t keyboard) -> void {

}

auto Seat::HandleGetTouch(touch_t touch) -> void {

}

auto Seat::HandleRelease() -> void {

}

GlobalSeat::GlobalSeat(display_t display) :
    global_seat_t(display)
{
    on_bind() = OnBind;
}

auto GlobalSeat::OnBind(client_t client, seat_t seat) -> void {
    Seat::Create(seat);
}
