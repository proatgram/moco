#include "LibInput.hpp"

#include <cerrno>
#include <fcntl.h>
#include <unistd.h>

#include "Events.hpp"

using namespace moco::backend;
using namespace wayland::server;

auto LibInput::OpenRestricted(const char *path, int flags, void *userData) -> int {
    int fd = open(path, flags);
    return fd < 0 ? -errno : fd;
}

auto LibInput::CloseRestricted(int fd, void *userData) -> void {
    close(fd);
}

const libinput_interface LibInput::s_libinputInterface = {
    .open_restricted = OpenRestricted,
    .close_restricted = CloseRestricted
};

LibInput::LibInput(Private, display_t display) :
    BackendBase(Private()),
    m_udevHandle(udev_new())
{
    m_libinputHandle = libinput_udev_create_context(&s_libinputInterface, nullptr, m_udevHandle);

    display.get_event_loop().add_idle([this]() -> void {BackendLoop();});
}

LibInput::~LibInput() {
    libinput_unref(m_libinputHandle);
    udev_unref(m_udevHandle);
}

auto LibInput::BackendLoop() -> void {
    libinput_event *event;
    while ((event = libinput_get_event(m_libinputHandle)) != nullptr) {
        /* Handle each libinput event */
        libinput_device *device = libinput_event_get_device(event);
        
        switch (libinput_event_get_type(event)) {

            case LIBINPUT_EVENT_NONE:
            case LIBINPUT_EVENT_DEVICE_ADDED:
            case LIBINPUT_EVENT_DEVICE_REMOVED:
            case LIBINPUT_EVENT_KEYBOARD_KEY:
                HandleKeyboardEvent(libinput_event_get_keyboard_event(event));
                break;
            case LIBINPUT_EVENT_POINTER_MOTION:
            case LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE:
            case LIBINPUT_EVENT_POINTER_BUTTON:
            case LIBINPUT_EVENT_POINTER_AXIS:
            case LIBINPUT_EVENT_POINTER_SCROLL_WHEEL:
            case LIBINPUT_EVENT_POINTER_SCROLL_FINGER:
            case LIBINPUT_EVENT_POINTER_SCROLL_CONTINUOUS:
            case LIBINPUT_EVENT_TOUCH_DOWN:
            case LIBINPUT_EVENT_TOUCH_UP:
            case LIBINPUT_EVENT_TOUCH_MOTION:
            case LIBINPUT_EVENT_TOUCH_CANCEL:
            case LIBINPUT_EVENT_TOUCH_FRAME:
            case LIBINPUT_EVENT_TABLET_TOOL_AXIS:
            case LIBINPUT_EVENT_TABLET_TOOL_PROXIMITY:
            case LIBINPUT_EVENT_TABLET_TOOL_TIP:
            case LIBINPUT_EVENT_TABLET_TOOL_BUTTON:
            case LIBINPUT_EVENT_TABLET_PAD_BUTTON:
            case LIBINPUT_EVENT_TABLET_PAD_RING:
            case LIBINPUT_EVENT_TABLET_PAD_STRIP:
            case LIBINPUT_EVENT_TABLET_PAD_KEY:
            case LIBINPUT_EVENT_TABLET_PAD_DIAL:
            case LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN:
            case LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE:
            case LIBINPUT_EVENT_GESTURE_SWIPE_END:
            case LIBINPUT_EVENT_GESTURE_PINCH_BEGIN:
            case LIBINPUT_EVENT_GESTURE_PINCH_UPDATE:
            case LIBINPUT_EVENT_GESTURE_PINCH_END:
            case LIBINPUT_EVENT_GESTURE_HOLD_BEGIN:
            case LIBINPUT_EVENT_GESTURE_HOLD_END:
            case LIBINPUT_EVENT_SWITCH_TOGGLE:
                break;
        }

        libinput_event_destroy(event);
        libinput_dispatch(m_libinputHandle);
    }
}

auto LibInput::HandleKeyboardEvent(libinput_event_keyboard *event) -> void {
    // Since we aren't multithreaded, it will be safe to pass by pointer.
    // It only gets destroyed after this function finishes.
    compositor::Events::Publish(Events::KeyboardKey, event);
}
