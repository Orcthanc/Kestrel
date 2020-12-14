#pragma once

#include "Core/Log.hpp"
#include "Core/Layer.hpp"
#include "Core/Application.hpp"

struct SandboxLayer: public Kestrel::Layer {
	SandboxLayer( const std::string& s );

	virtual void onUpdate() override;
	virtual void onEvent( Kestrel::Event &e ) override;
};
