#include "Platform/Vulkan/VKTerrainRenderer.hpp"

#include "Platform/Vulkan/VKMesh.hpp"
#include "Platform/Vulkan/VKMaterial.hpp"
#include "Platform/Vulkan/VKCameraRenderer.hpp"
#include "Platform/Vulkan/VKTerrain.hpp"

using namespace Kestrel;

KST_VK_TerrainRenderer KST_VK_TerrainRenderer::instance;

void KST_VK_TerrainRenderer::init(){
	PROFILE_FUNCTION();

	instance.terrain_mesh = Mesh::createMesh( "../res/Kestrel/res/models/Terrain4x4.obj" );
	instance.terrain_material = VK_Materials::requestResourceHandle( "../res/Kestrel/shader/terrain" );
}

void KST_VK_TerrainRenderer::drawTerrain( KST_VK_CameraRenderer *renderer, const TransformComponent &transform, const Terrain &terrain ){
	PROFILE_FUNCTION();

	VK_Terrain_T& ter = VK_TerrainRegistry::terrains.at( terrain );

	//Transform
	auto model = glm::scale( glm::translate( glm::identity<glm::mat4>(), transform.loc ) * glm::mat4_cast( transform.rot ), transform.scale );

	VK_UniformBufferObj mod_col{ model, glm::vec3( 0, 0, 1 )};
	renderer->render_info.cmd_buffer[0]->pushConstants(
			*VK_Materials::requestResource( terrain_material )->layout,
			vk::ShaderStageFlagBits::eTessellationControl | vk::ShaderStageFlagBits::eTessellationEvaluation,
			0,
			sizeof( mod_col ),
			&mod_col );

	float tilesize = transform.scale.x;

	glm::vec4 pos{ 0, 0, 0, 1 };

	auto temp = glm::inverse( renderer->view_proj.view );
	pos = temp * pos;

	int xoffset = (int)pos.x / (int)tilesize;
	int zoffset = -(int)pos.z / (int)tilesize;

	std::array<float, 2> offset{ (float)xoffset, (float)zoffset };

	renderer->render_info.cmd_buffer[0]->pushConstants(
			*VK_Materials::requestResource( terrain_material )->layout,
			vk::ShaderStageFlagBits::eVertex,
			80,
			8,
			offset.data() );

	//Mat
	if( terrain_material != renderer->render_info.bound_mat ){
		std::vector<std::array<vk::ImageView, 2>> views;

		views.reserve( renderer->render_targets.size );

		for( auto& t: renderer->render_targets ){
			views.push_back(std::array<vk::ImageView, 2>{
					*t.color_depth_view[0],
					*t.color_depth_view[1] });
		}

		BindingInfo bind_inf(
				*renderer->device_surface,
				renderer,
				renderer->render_targets.begin()->size,
				renderer->render_targets.getIndex(),
				*renderer->render_info.cmd_buffer[0],
				*renderer->uniform_buffer.buffer,
				renderer->render_info.render_mode );

		VK_Materials::requestResource( terrain_material )->bind( bind_inf );

		renderer->render_info.bound_mat = terrain_material;
	}

	auto mimp = VK_MeshRegistry::requestResource( terrain_mesh );

	vk::DeviceSize size = 0;

	renderer->render_info.cmd_buffer[0]->bindVertexBuffers( 1, 1, &*ter.instance_offsets.buffer, &size );

	// TODO clean up (maybe amount instead of size)
	renderer->render_info.cmd_buffer[0]->drawIndexed(
			static_cast<uint32_t>( mimp->index_amount ),
			ter.size,
			static_cast<uint32_t>( mimp->index_offset ),
			static_cast<int32_t>( mimp->vertex_offset ), 0 );

	/*
	//TODO Improve frustrum culling

	float tilesize = transform.scale.x * 2;

	glm::vec4 pos{ 0, 0, 0, 1 };
	glm::vec4 dir{ 0, 0, 1, 0 };

	auto temp = glm::inverse( renderer->view_proj.view );
	pos = temp * pos;
	dir = temp * dir;

	int xoffset = (int)pos.x / (int)tilesize;
	int zoffset = (int)pos.z / (int)tilesize;

	glm::vec2 cullmin{ dir.x < 0 ? 0 : 1, dir.z < 0 ? 0 : 1 };
	glm::vec2 cullmax{ dir.x < 0 ? 1 : 0, dir.z < 0 ? 1 : 0 };

	if( std::abs( dir.x ) < std::abs( dir.z )){
		cullmin.x = 1;
		cullmax.x = 1;
	} else {
		cullmin.y = 1;
		cullmax.y = 1;
	}

	KST_CORE_INFO( "{}", glm::to_string( pos ));

	//High resolution
	for( int z = -terrain.high_res_tiles * cullmin.y; z <= terrain.high_res_tiles * cullmax.y; ++z ){
		for( int x = -terrain.high_res_tiles * cullmin.x; x <= terrain.high_res_tiles * cullmax.x; ++x ){
			TransformComponent new_tran = transform;
			new_tran.loc.x += tilesize * ( x + xoffset );
			new_tran.loc.z += tilesize * ( z + zoffset );
			renderer->drawMesh( new_tran, terrain_mesh, terrain_material, terrain.high_res_res / 4.0, glm::vec3( 1, 0, 0 ));
		}
	}

	int med_size = terrain.med_res_tiles + terrain.high_res_tiles;

	//Medium resolution
	for( int z = -med_size * cullmin.y; z <= med_size * cullmax.y; ++z ){
		for( int x = -med_size * cullmin.x; x <= med_size * cullmax.x; ++x ){
			if( x >= -terrain.high_res_tiles && x <= terrain.high_res_tiles &&
					z >= -terrain.high_res_tiles && z <= terrain.high_res_tiles ){
				x = terrain.high_res_tiles;
				continue;
			}
			TransformComponent new_tran = transform;
			new_tran.loc.x += tilesize * ( x + xoffset );
			new_tran.loc.z += tilesize * ( z + zoffset );
			renderer->drawMesh( new_tran, terrain_mesh, terrain_material, terrain.med_res_res / 4.0, glm::vec3( 0, 1, 0 ));
		}
	}

	int low_size = med_size + terrain.low_res_tiles;

	//Low resolution
	for( int z = -low_size * cullmin.y; z <= low_size * cullmax.y; ++z ){
		for( int x = -low_size * cullmin.x; x <= low_size * cullmax.x; ++x ){
			if( x >= -med_size && x <= med_size &&
					z >= -med_size && z <= med_size ){
				x = med_size;
				continue;
			}
			TransformComponent new_tran = transform;
			new_tran.loc.x += tilesize * ( x + xoffset );
			new_tran.loc.z += tilesize * ( z + zoffset );
			renderer->drawMesh( new_tran, terrain_mesh, terrain_material, terrain.low_res_res / 4.0, glm::vec3( 0, 0, 1 ));
		}
	}
	*/
}
