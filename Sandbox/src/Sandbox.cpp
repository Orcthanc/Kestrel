#include "Sandbox.hpp"
#include "SandboxLayer.hpp"

Sandbox::Sandbox(): Kestrel::Application(){
	//TODO lifetime management
	addLayer( new SandboxLayer( "Sandbox" ));
}

Kestrel::Application* createApplication(){
	return new Sandbox();
}
