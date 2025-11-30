#include "Keyboard.hpp"

#include <format>
#include <cstring>

#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <wayland-server-protocol.hpp>

using namespace moco::wayland::implementation;
using namespace wayland::server;

Keyboard::Keyboard(keyboard_t keyboard, Private) :
    Keyboard(keyboard) {}

Keyboard::Keyboard(keyboard_t keyboard) :
    ObjectImplementationBase(keyboard)
{
    on_release() = [this]() -> void {HandleRelease();};
    
    m_xkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

    m_xkbRuleNames = {
        .rules = getenv("XKB_DEFAULT_RULES") ?: "evdev",
        .model = getenv("XKB_DEFAULT_MODEL") ?: "",
        .layout = getenv("XKB_DEFAULT_LAYOUT") ?: "us",
        .variant = getenv("XKB_DEFAULT_VARIANT") ?: "",
        .options = getenv("XKB_DEFAULT_OPTIONS") ?: ""
    };

    m_xkbKeymap = xkb_keymap_new_from_names2(m_xkbContext, &m_xkbRuleNames, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
    char *keymapString = xkb_keymap_get_as_string2(m_xkbKeymap, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_SERIALIZE_NO_FLAGS);
    ssize_t keymapStringSize = std::strlen(keymapString) + 1;

    // Creates a memfd to send to clients to get the keymap
    // with a unique name.
    m_xkbKeymapFd = memfd_create(std::format("wl_keyboard@{}::xkb_keymap", std::to_string(reinterpret_cast<uintptr_t>(c_ptr()))).c_str(), MFD_CLOEXEC);

    ftruncate(m_xkbKeymapFd, keymapStringSize);

    // Write keymap string to memfd for clients to use
    write(m_xkbKeymapFd, keymapString, keymapStringSize);
    free(keymapString);

    // Seek to beginning for reading
    lseek(m_xkbKeymapFd, 0, SEEK_SET);

    keymap(keyboard_keymap_format::xkb_v1, m_xkbKeymapFd, keymapStringSize);

    // Change for Event system?
    m_keyboardEventSource = get_client().get_display().get_event_loop().add_idle([this]() -> void {});
}

auto Keyboard::HandleRelease() -> void {
    xkb_context_unref(m_xkbContext);
    xkb_keymap_unref(m_xkbKeymap);
    close(m_xkbKeymapFd);
}
