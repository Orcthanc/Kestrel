/*
 * =====================================================================================
 *
 *       Filename:  Event.cpp
 *
 *    Description:  Implementation of Event.hpp
 *
 *        Version:  1.0
 *        Created:  08/25/2020 02:57:47 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#include "Event.hpp"

Kestrel::EventDispatcher::EventDispatcher( Event& e ): e{ e }{
}
