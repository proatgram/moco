#pragma once

#include "ObjectImplementationBase.hpp"

#include <wayland-server-protocol.hpp>
#include <xkbcommon/xkbcommon.h>

#include <span>

namespace moco::wayland::implementation {
    class Keyboard : public ObjectImplementationBase<::wayland::server::keyboard_t, Keyboard> {
            using ObjectImplementationBase::on_release;
        public:
            Keyboard(::wayland::server::keyboard_t keyboard, Private);

        private:
            Keyboard(::wayland::server::keyboard_t keyboard);

            auto HandleRelease() -> void;

            std::span<uint8_t> m_data;

            int m_xkbKeymapFd;
            xkb_context *m_xkbContext;
            xkb_keymap *m_xkbKeymap;

            xkb_rule_names m_xkbRuleNames;
            // Use optional to get around default construction of event_source_t
            std::optional<::wayland::server::event_source_t> m_keyboardEventSource;
    };
}  // namespace moco::wayland::implementation
