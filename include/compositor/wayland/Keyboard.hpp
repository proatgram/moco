#pragma once

#include "ObjectImplementationBase.hpp"
#include "Surface.hpp"
#include "Events.hpp"

#include <wayland-server-protocol.hpp>
#include <xkbcommon/xkbcommon.h>

#include <span>

namespace moco::wayland::implementation {
    class Keyboard : public ObjectImplementationBase<::wayland::server::keyboard_t, Keyboard> {
            using ObjectImplementationBase::on_release;
        public:
            Keyboard(::wayland::server::keyboard_t keyboard, Private);

            enum class Events {
                Keymap,
                Enter,
                Leave,
                Key,
                Modifier,
                RepeatInfo
            };
            using EventSubscriber_t = compositor::EventSubscriber_t<Events>;

            struct Keymap_EventData {
                ::wayland::server::keyboard_keymap_format Format;
                int Fd{-1};
                size_t Size{0};
            };

            struct Enter_EventData {
                uint32_t Serial{0};
                std::shared_ptr<::moco::wayland::implementation::Surface> Surface{nullptr};
                std::vector<uint32_t> Keys{};
            };

            struct Leave_EventData {
                uint32_t Serial{0};
                std::shared_ptr<::moco::wayland::implementation::Surface> Surface{nullptr};
            };

            struct Key_EventData {
                uint32_t Serial{0};
                uint32_t Time{0};
                uint32_t Key{0};
                ::wayland::server::keyboard_key_state KeyState{};
            };

            struct Modifier_EventData {
                uint32_t Serial{0};
                uint32_t ModsDepressed{0};
                uint32_t ModsLatched{0};
                uint32_t ModsLocked{0};
                uint32_t Group{0};
            };

            struct RepeatInfo_EventData {
                int32_t Rate{0};
                int32_t Delay{0};
            };

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

            EventSubscriber_t m_keymapEvent;
            EventSubscriber_t m_enterEvent;
            EventSubscriber_t m_leaveEvent;
            EventSubscriber_t m_keyEvent;
            EventSubscriber_t m_modifierEvent;
            EventSubscriber_t m_repeatInfoEvent;
    };
}  // namespace moco::wayland::implementation
