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
			WindowCloseEvent();
			virtual ~WindowCloseEvent();

			virtual EventType getType() override;
			virtual EventDomain::EventDomain getDomain() override;
			virtual const char * getName() override;

			static constexpr EventType getStaticType(){
				return EventType::eWindowClose;
			}
	};
};
