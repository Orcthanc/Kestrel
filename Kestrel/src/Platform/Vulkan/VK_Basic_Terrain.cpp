#include "VK_Basic_Terrain.hpp"
#include "VKMesh.hpp"

using namespace Kestrel;

VK_BasicTerrain::VK_BasicTerrain(){
	mesh.load_obj<VK_Mesh>( "../res/Kestrel/res/models/Terrain4x4.obj" );
}

void VK_BasicTerrain::draw(){

}
