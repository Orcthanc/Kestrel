#include "Scene/Scene.hpp"
#include "Scene/Entity.hpp"

using namespace Kestrel;

void Scene::onUpdate(){

}

Entity Scene::createEntity(){
	return { this, entt_reg.create() };
}

void Scene::destroyEntity( Entity entity ){
	entt_reg.destroy( entity );
}
