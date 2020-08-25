/*
 * =====================================================================================
 *
 *       Filename:  LayerStack.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/25/2020 04:41:00 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#include "LayerStack.hpp"

Kestrel::LayerStack::~LayerStack(){
	for( auto l: layers ){
		l->onDetach();
	}
}

void Kestrel::LayerStack::pushLayer( Layer* l ){
	l->onAttach();
	layers.push_back( l );
}

void Kestrel::LayerStack::PopLayer( Layer* l ){
	auto it = std::find( layers.begin(), layers.end(), l );
	if( it != layers.end() ){
		l->onDetach();
		layers.erase( it );
	}
}
