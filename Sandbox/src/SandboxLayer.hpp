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
};
