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

	class InputEvent: public Event {
		public:
			InputEvent() = default;
			InputEvent( int keycode );
			virtual ~InputEvent() = default;

			virtual const char* getKeyName();

			int scancode;
	};

	class KeyPushEvent: public InputEvent {
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

	class KeyReleaseEvent: public InputEvent {
		public:
			KeyReleaseEvent() = default;
			KeyReleaseEvent( int keycode );
			virtual ~KeyReleaseEvent() = default;

			virtual EventType getType() override;
			virtual EventDomain getDomain() override;
			virtual const char * getName() override;

			static constexpr EventType getStaticType(){
				return EventType::eInputKeyPressed;
			}
	};
};
