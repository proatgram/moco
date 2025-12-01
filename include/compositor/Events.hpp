#pragma once

#include <vector>
#include <string>
#include <any>
#include <functional>
#include <algorithm>
#include <memory>
#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <optional>

namespace moco::compositor {
    template <typename EventType>
    class EventSubscriber;

    template <typename EventType>
    using EventSubscriber_t = std::shared_ptr<EventSubscriber<EventType>>;

    /**
     * @brief Moco Event System
     * @details Provides a factory for event subscriptions
     * and a way to publish events with specific data.
     *
     */
    class Events {
        struct Private {explicit Private() = default;};
        template <class EventType>
        friend class EventSubscriber;
        public:
            using CallbackHandler_t = std::function<void(std::any)>;

            /**
             * @brief Subscribes to a specific event.
             * @details Subscribes to an event `event`, and allows
             * a callback to be added to the subscription during
             * subscribe time.
             *
             * @param `event`: A specific event to subscribe to and recieve signals to.
             * @param `callback`: [Optional] A function to call when the event is published.
             * The callback must be of `CallbackHandler_t`.
             *
             * @return `EventSubscriber_t<EventType>`: An object representing a subscription
             * to a specific event.
             *
             */
            template <typename EventType>
            static inline auto Subscribe(EventType event, std::optional<CallbackHandler_t> callback = std::nullopt) -> EventSubscriber_t<EventType> {
                std::shared_ptr<EventSubscriber<EventType>> subscriber = std::make_shared<EventSubscriber<EventType>>(Private(), event);
                s_subscribers[typeid(EventType)].push_back(std::make_any<EventSubscriber<EventType>*>(subscriber.get()));
                if (callback.has_value()) {
                    subscriber->AddHandler(callback.value());
                }

                return subscriber;
            }

            /**
             * @brief Publish to a specific event.
             * @details Publishes an event along with any
             * event data to any potential subscribers.
             *
             * @param `event`: A specific event to publish to.
             * @param `eventData`: [Optional] Type-erased data to
             * pass to the event subscribers callbacks.
             *
             * @return `size_t`: Amount of callbacks called due to the publication
             * of `event`.
             *
             */
            template <typename EventType>
            static inline auto Publish(EventType event, std::any eventData = {}) -> size_t {
                size_t numCalled{0};

                // If it does not contain the type, then there are def no subscribers.
                if (s_subscribers.contains(typeid(EventType))) {
                    for (const std::any &subscriber : s_subscribers.at(typeid(EventType))) {
                        EventSubscriber<EventType>* eventSubscriber = std::any_cast<EventSubscriber<EventType>*>(subscriber);
                        
                        // Should be valid, I hope?
                        if (eventSubscriber) {
                            if (eventSubscriber->GetSubscribedEvent() == event) {
                                numCalled += (*eventSubscriber)(eventData);
                            }
                        } else {
                            std::cerr << __PRETTY_FUNCTION__ << ": "
                                      << "Invalid pointer found. "
                                      << "This shouldn't happen and is a bug." << std::endl;
                        }
                    }
                }

                return numCalled;
            }

        private:
            template <typename EventType>
            static inline auto Unsubscribe(EventSubscriber<EventType> *subscription) -> bool {
                if (s_subscribers.contains(typeid(EventType))) {
                    std::vector<std::any> &eventSubscribers = s_subscribers.at(typeid(EventType));
                    size_t numErased = std::erase_if(eventSubscribers, [subscription](std::any subscriber) -> bool {
                        // Guarenteed to be of type EventSubscriber<EventType> due to the contains call above.
                        EventSubscriber<EventType> *eventSubscriber = std::any_cast<EventSubscriber<EventType>*>(subscriber);

                        return eventSubscriber == subscription;
                    });

                    if (eventSubscribers.empty()) {
                        // Remove if empty
                        s_subscribers.erase(typeid(EventType));
                    }

                    return numErased != 0;
                }

                return false;
            }

            // std::any -> EventSubscriber<*> *
            // Safety ensured by the pointer only existing while the object
            // exists, the destruction of the object removes its pointer from
            // this container before it fully destructs.
            inline static std::unordered_map<std::type_index, std::vector<std::any>> s_subscribers{};
    };

    /**
     * @brief Class that represents a subscription to an event.
     * @details An object created from this class represents a
     * subscription to a specific event, and can call its callbacks
     * when the event is published. Upon destruction, an object
     * unsubscribes from its event.
     *
     */
    template <typename EventType>
    class EventSubscriber {
        friend class Events;
        public:
            EventSubscriber(Events::Private, EventType event) :
                m_subscribedEvent(event) {}

            inline ~EventSubscriber() {
                if (!Events::Unsubscribe<EventType>(this)) {
                    std::cerr << __PRETTY_FUNCTION__ << ": "
                              << "Failed to unsubscribe from event."
                              << std::endl;
                }
            }

            /**
             * @brief Adds a callback to handle event publishes.
             * @details The callback is added to a container, thus
             * you can store multiple callbacks to be called when the
             * event gets published.
             *
             * @param `callbackHandler`: A function to call which follows
             * the type `Events::CallbackHandler_t`
             *
             */
            inline auto AddHandler(Events::CallbackHandler_t callbackHandler) -> void {
                m_handlers.push_back(callbackHandler);
            }

            /**
             * @brief Returns the event this object is subscribed to.
             *
             * @return `EventType`: Subscribed event.
             *
             */
            inline auto GetSubscribedEvent() const -> EventType {
                return m_subscribedEvent;
            }

        private:
            inline auto operator()(std::any callbackData) -> size_t {
                size_t numCalled{0};
                std::for_each(std::begin(m_handlers), std::end(m_handlers), [callbackData, &numCalled](Events::CallbackHandler_t callback) -> void {
                    callback(callbackData);
                    numCalled++;
                });

                return numCalled;
            }

            std::vector<Events::CallbackHandler_t> m_handlers;

            EventType m_subscribedEvent;
    };

}  // namespace moco::compositor
