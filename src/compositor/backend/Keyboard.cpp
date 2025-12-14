#include "Keyboard.hpp"

#include "Events.hpp"
#include "LibInput.hpp"

using namespace moco::backend;
using namespace moco::compositor;
using namespace wayland::server;

Keyboard::Keyboard(Private, display_t display) :
    BackendBase(Private())
{
    LibInput::Initialize(display);

    Events::Subscribe(LibInput::Events::KeyboardKey, [this](std::any eventData) -> void {});
}

auto Keyboard::EventKeyboardKey(std::any eventData) -> void {
    try {
        libinput_event_keyboard *keyboardEvent = std::any_cast<libinput_event_keyboard*>(eventData);


    } catch (const std::bad_any_cast &err) {
        std::cerr << __PRETTY_FUNCTION__ << ": "
                  << "Event data error: Type mismatch."
                  << std::endl;
    }
}
