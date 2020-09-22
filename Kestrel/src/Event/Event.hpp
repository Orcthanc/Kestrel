/*
 * =====================================================================================
 *
 *       Filename:  Event.hpp
 *
 *    Description:  Basic event system
 *
 *        Version:  1.0
 *        Created:  08/25/2020 02:15:12 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#pragma once

#include "kstpch.hpp"

namespace Kestrel {
	enum class EventType {
		eNone = 0,
		eWindowResize, eWindowMove, eWindowMinimize, eWindowClose,
		eInputMouseMoved, eInputMousePressed, eInputMouseReleased, eInputMouseScroll,
		eInputKeyPressed, eInputKeyReleased,
		eInputControllerPressed, eInputControllerReleased,
		eAppUpdate, eAppRender
	};

	enum class EventDomain {
		eNone = 0,
		eApp = 1 << 0,
		eInput = 1 << 1,
		eKeyboard = 1 << 2,
		eMouse = 1 << 3,
		eController = 1 << 4
	};

	class Event {
		public:
			virtual ~Event() = default;

			bool handled = false;

			virtual EventType getType() = 0;
			virtual EventDomain getDomain() = 0;
			virtual const char* getName() = 0;

			bool isDomain( EventDomain e ){
				return ( getDomain() & e ) != EventDomain::eNone;
			}
	};

	class EventDispatcher {
		public:
			EventDispatcher( Event& e );

			template <typename T, typename F>
			bool dispatch( const F& func ){
				if( e.getType() == T::getStaticType() ){
					e.handled = func( static_cast<T&>( e ));
					return true;
				}
				return false;
			}

		private:
			Event& e;
	};
}
