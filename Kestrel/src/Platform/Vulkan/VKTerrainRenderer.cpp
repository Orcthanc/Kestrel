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

void KST_VK_TerrainRenderer::drawTerrain( Entity e, KST_VK_CameraRenderer *renderer, const TransformComponent &transform, const Terrain &terrain ){
	PROFILE_FUNCTION();
	//TODO Frustrum culling
	//
	KST_CORE_INFO( "Terrain start {} {} {}", terrain.high_res_tiles, terrain.med_res_tiles, terrain.low_res_tiles );

	//TODO remove test
	float tilesize = transform.scale.x;
	renderer->drawMesh( e, transform, terrain_mesh, terrain_material );

	//High resolution
	for( int y = -terrain.high_res_tiles; y <= terrain.high_res_tiles; ++y ){
		for( int x = -terrain.high_res_tiles; x <= terrain.high_res_tiles; ++x ){
			TransformComponent new_tran = transform;
			new_tran.loc.x += tilesize * x;
			new_tran.loc.y += tilesize * y;
			renderer->drawMesh( e, new_tran, terrain_mesh, terrain_material, terrain.high_res_res / 4 );
			//TODO draw stuff
			//KST_CORE_INFO( "High: {} {}", x, y );
		}
	}

	int med_size = terrain.med_res_tiles + terrain.high_res_tiles;

	//Medium resolution
	for( int y = -med_size; y <= med_size; ++y ){
		for( int x = -med_size; x <= med_size; ++x ){
			if( x >= -terrain.high_res_tiles && x <= terrain.high_res_tiles &&
					y >= -terrain.high_res_tiles && y <= terrain.high_res_tiles ){
				x += 2 * terrain.high_res_tiles;
				continue;
			}
			//KST_CORE_INFO( "Med: {} {}", x, y );
		}
	}

	int low_size = med_size + terrain.low_res_tiles;

	//Low resolution
	for(
			int y = -low_size;
			y <= low_size;
			++y ){
		for(
				int x = -low_size;
				x <= low_size;
				++x ){
			if( x >= -med_size && x <= med_size &&
					y >= -med_size && y <= med_size ){
				x += 2 * med_size;
				continue;
			}
			//KST_CORE_INFO( "Low: {} {}", x, y );
		}
	}
}
