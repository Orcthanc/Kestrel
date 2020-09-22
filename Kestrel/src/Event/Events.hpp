/*
 * =====================================================================================
 *
 *       Filename:  Events.hpp
 *
 *    Description:  Some common Events
 *
 *        Version:  1.0
 *        Created:  08/26/2020 06:55:25 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#include "kstpch.hpp"
#include "Event.hpp"

namespace Kestrel {
	class WindowCloseEvent: public Event {
		public:
			WindowCloseEvent() = default;
			virtual ~WindowCloseEvent() = default;

			virtual EventType getType() override;
			virtual EventDomain getDomain() override;
			virtual const char * getName() override;

			static constexpr EventType getStaticType(){
				return EventType::eWindowClose;
			}
	};

	class KeyEvent: public Event {
		public:
			KeyEvent() = default;
			KeyEvent( int keycode );
			virtual ~KeyEvent() = default;

			virtual const std::string getKeyName();

			int scancode;
	};

	class KeyPushEvent: public KeyEvent {
		public:
			KeyPushEvent() = default;
			KeyPushEvent( int keycode );
			virtual ~KeyPushEvent() = default;

			virtual EventType getType() override;
			virtual EventDomain getDomain() override;
			virtual const char * getName() override;

			static constexpr EventType getStaticType(){
				return EventType::eInputKeyPressed;
			}
	};

	class KeyReleaseEvent: public KeyEvent {
		public:
			KeyReleaseEvent() = default;
			KeyReleaseEvent( int keycode );
			virtual ~KeyReleaseEvent() = default;

			virtual EventType getType() override;
			virtual EventDomain getDomain() override;
			virtual const char * getName() override;

			static constexpr EventType getStaticType(){
				return EventType::eInputKeyReleased;
			}
	};

	class MouseButtonEvent: public Event {
		public:
			MouseButtonEvent() = default;
			MouseButtonEvent( int button );
			~MouseButtonEvent() = default;

			virtual std::string getButtonName();

			int button;
	};

	class MousePressedEvent: public MouseButtonEvent {
		public:
			MousePressedEvent() = default;
			MousePressedEvent( int button );
			~MousePressedEvent() = default;

			virtual EventType getType() override;
			virtual EventDomain getDomain() override;
			virtual const char* getName() override;

			static constexpr EventType getStaticType(){
				return EventType::eInputMousePressed;
			}
	};

	class MouseReleasedEvent: public MouseButtonEvent {
		public:
			MouseReleasedEvent() = default;
			MouseReleasedEvent( int button );
			~MouseReleasedEvent() = default;

			virtual EventType getType() override;
			virtual EventDomain getDomain() override;
			virtual const char* getName() override;

			static constexpr EventType getStaticType(){
				return EventType::eInputMouseReleased;
			}
	};

	class MouseMovedEvent: public Event {
		public:
			MouseMovedEvent() = default;
			MouseMovedEvent( float x, float y );
			~MouseMovedEvent() = default;

			virtual EventType getType() override;
			virtual EventDomain getDomain() override;
			virtual const char * getName() override;

			float x, y;

			static constexpr EventType getStaticType(){
				return EventType::eInputMouseMoved;
			}
	};

	class MouseScrollEvent: public Event {
		public:
			MouseScrollEvent() = default;
			MouseScrollEvent( float x, float y );
			~MouseScrollEvent() = default;

			virtual EventType getType() override;
			virtual EventDomain getDomain() override;
			virtual const char * getName() override;

			float x, y;

			static constexpr EventType getStaticType(){
				return EventType::eInputMouseScroll;
			}
	};
};
