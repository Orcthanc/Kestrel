#include "Platform/Vulkan/VKTerrainRenderer.hpp"

#include "Platform/Vulkan/VKMesh.hpp"
#include "Platform/Vulkan/VKMaterial.hpp"
#include "Platform/Vulkan/VKCameraRenderer.hpp"

using namespace Kestrel;

KST_VK_TerrainRenderer KST_VK_TerrainRenderer::instance;

void KST_VK_TerrainRenderer::init(){
	PROFILE_FUNCTION();

	instance.terrain_mesh = Mesh::createMesh( "../res/Kestrel/res/models/Terrain4x4.obj" );
	instance.terrain_material = VK_Materials::getInstance().loadMaterial( "../res/Kestrel/shader/basic" );
}

void KST_VK_TerrainRenderer::drawTerrain( KST_VK_CameraRenderer *renderer, const TransformComponent &transform, const Terrain &terrain ){
	PROFILE_FUNCTION();
	//TODO Frustrum culling

	float tilesize = transform.scale.x * 2;
	//renderer->drawMesh( e, transform, terrain_mesh, terrain_material );

	//High resolution
	for( int z = -terrain.high_res_tiles; z <= terrain.high_res_tiles; ++z ){
		for( int x = -terrain.high_res_tiles; x <= terrain.high_res_tiles; ++x ){
			TransformComponent new_tran = transform;
			new_tran.loc.x += tilesize * x;
			new_tran.loc.z += tilesize * z;
			renderer->drawMesh( new_tran, terrain_mesh, terrain_material, terrain.high_res_res / 4, glm::vec3( 1, 0, 0 ));
		}
	}

	int med_size = terrain.med_res_tiles + terrain.high_res_tiles;

	//Medium resolution
	for( int z = -med_size; z <= med_size; ++z ){
		for( int x = -med_size; x <= med_size; ++x ){
			if( x >= -terrain.high_res_tiles && x <= terrain.high_res_tiles &&
					z >= -terrain.high_res_tiles && z <= terrain.high_res_tiles ){
				x += 2 * terrain.high_res_tiles;
				continue;
			}
			TransformComponent new_tran = transform;
			new_tran.loc.x += tilesize * x;
			new_tran.loc.z += tilesize * z;
			renderer->drawMesh( new_tran, terrain_mesh, terrain_material, terrain.med_res_res / 4, glm::vec3( 0, 1, 0 ));
		}
	}

	int low_size = med_size + terrain.low_res_tiles;

	//Low resolution
	for( int z = -low_size; z <= low_size; ++z ){
		for( int x = -low_size; x <= low_size; ++x ){
			if( x >= -med_size && x <= med_size &&
					z >= -med_size && z <= med_size ){
				x += 2 * med_size;
				continue;
			}
			TransformComponent new_tran = transform;
			new_tran.loc.x += tilesize * x;
			new_tran.loc.z += tilesize * z;
			renderer->drawMesh( new_tran, terrain_mesh, terrain_material, terrain.low_res_res / 4, glm::vec3( 0, 0, 1 ));
		}
	}
}
