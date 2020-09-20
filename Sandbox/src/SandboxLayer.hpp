/*
 * =====================================================================================
 *
 *       Filename:  SandboxLayer.hpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/25/2020 05:14:57 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#pragma once

#include "Core/Log.hpp"
#include "Core/Layer.hpp"
#include "Core/Application.hpp"

struct SandboxLayer: public Kestrel::Layer {
	SandboxLayer( const std::string& s ): Layer( s ){}

	virtual void onUpdate() override {
		static int calls = 0;
		if( !( ++calls % 1000000 ))
			KST_INFO( "{}", calls );
	}

	virtual void onEvent( Kestrel::Event& e ) override {
		Kestrel::EventDispatcher d{ e };

		d.dispatch<Kestrel::KeyPushEvent>( []( Kestrel::KeyPushEvent& e ){
				KST_INFO( "Key {} pushed", e.getKeyName() );
				return true;
			});

		d.dispatch<Kestrel::KeyReleaseEvent>( []( Kestrel::KeyReleaseEvent& e ){
				KST_INFO( "Key {} released", e.getKeyName() );
				return true;
			});
	}
	
};
