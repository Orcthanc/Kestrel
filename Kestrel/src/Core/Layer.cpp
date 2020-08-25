/*
 * =====================================================================================
 *
 *       Filename:  Layer.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/25/2020 04:30:23 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#include "Layer.hpp"

Kestrel::Layer::Layer( const std::string& name ): debug_name( name ){}
void Kestrel::Layer::onAttach(){}
void Kestrel::Layer::onDetach(){}
void Kestrel::Layer::onUpdate(){}
void Kestrel::Layer::onGui(){}
void Kestrel::Layer::onEvent(Event &e){}
const std::string Kestrel::Layer::getName(){ return debug_name; }
