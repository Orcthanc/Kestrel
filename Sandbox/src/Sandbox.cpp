#include "Sandbox.hpp"
#include "SandboxLayer.hpp"

#include "Scene/SceneFileScene.hpp"

Sandbox::Sandbox(): Kestrel::Application(){
	//TODO lifetime management
	current_scene = std::make_shared<Kestrel::SceneFileScene>( "../res/Sandbox/res/scenes/scene1.sce" );

	addLayer( std::make_shared<SandboxLayer>( "Sandbox" ));

}

std::unique_ptr<Kestrel::Application> createApplication(){
	return std::make_unique<Sandbox>();
}
