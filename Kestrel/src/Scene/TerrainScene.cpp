#include "Scene/TerrainScene.hpp"

#include "Scene/Components.hpp"
#include "Renderer/Terrain.hpp"

#include "imgui.h"

using namespace Kestrel;

TerrainScene::TerrainScene(){
	PROFILE_FUNCTION();

	Terrain terrain;
	auto temp = createEntity( "Terrain" );
	temp.addComponent<TransformComponent>( glm::vec3{ 0, -1, 0 }, glm::quat{ glm::vec3{ -1.5707963, 0, 0 }}, glm::vec3{ 100, 100, 100 });
	temp.addComponent<TerrainComponent>( terrain );
}

void TerrainScene::onUpdate(){
	PROFILE_FUNCTION();

	auto cams = getView<NameComponent, CameraComponent>();

	ImGui::Begin( "Cameras" );
	for( auto& comps: cams ){
		auto [name, cam] = cams.get<NameComponent, CameraComponent>( comps );
		if( ImGui::CollapsingHeader( name.name.c_str() )){

			cam.camera->onImgui();

			ImGui::Separator();
		}
	}

	ImGui::End();

	ImGui::ShowDemoWindow();
}