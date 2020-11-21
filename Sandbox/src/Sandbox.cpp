#include "Sandbox.hpp"
#include "SandboxLayer.hpp"

Sandbox::Sandbox(): Kestrel::Application(){
	//TODO lifetime management
	addLayer( std::make_shared<SandboxLayer>( "Sandbox" ));
}

std::unique_ptr<Kestrel::Application> createApplication(){
	return std::make_unique<Sandbox>();
}
