#include "Sandbox.hpp"
#include "SandboxLayer.hpp"

#include "Scene/SceneFileScene.hpp"
#include "Scene/TerrainScene.hpp"
#include "Renderer/Terrain.hpp"

Sandbox::Sandbox(): Kestrel::Application(){
	//TODO lifetime management
	//current_scene = std::make_shared<Kestrel::SceneFileScene>( "../res/Sandbox/res/scenes/scene2.sce" );
	KST_CORE_INFO( "Creating terrain" );
	current_scene = std::make_shared<Kestrel::TerrainScene>( Kestrel::Terrain::createTerrain({ 3, 1024, 8, 64, 24, 4, 1024 }));

	addLayer( std::make_shared<SandboxLayer>( "Sandbox" ));

}

std::unique_ptr<Kestrel::Application> createApplication(){
	return std::make_unique<Sandbox>();
}
