#pragma once

#include "Core/Log.hpp"
#include "Core/Layer.hpp"
#include "Core/Application.hpp"

#include "Renderer/NaiveCamera.hpp"

struct SandboxLayer: public Kestrel::Layer {
	SandboxLayer( const std::string& s );

	virtual void onUpdate() override;
	virtual void onEvent( Kestrel::Event &e ) override;

	std::shared_ptr<Kestrel::NaiveCamera> camera;

	Kestrel::Entity plane1, plane2;
#ifdef KST_COLOR_STATS
	std::ofstream color_stat_file{ "color_stats.txt" };
#endif
};
