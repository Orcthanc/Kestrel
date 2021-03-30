#include "Platform/Vulkan/VKMesh.hpp"

#include <fstream>

using namespace Kestrel;

/*
template<>
std::map<std::filesystem::path, Mesh> VK_MeshRegistry::resource_locations;
template<>
std::unordered_map<Mesh, std::shared_ptr<VK_Mesh>> VK_MeshRegistry::resources;
template<>
SharedMeshResources VK_MeshRegistry::shared_resources;
KST_VK_DeviceSurface* VK_MeshRegistry::device;
VK_MeshRegistry::CopyInfo VK_MeshRegistry::copy_inf;
*/

template<>
std::shared_ptr<VK_Mesh> VK_MeshRegistry::load_resource( const std::filesystem::path& path ){
	PROFILE_FUNCTION();

	std::vector<VK_Vertex> verts;
	std::vector<uint32_t> indices;

	std::ifstream file( path, std::ios::binary );

	if( !file.is_open() ){
		KST_CORE_WARN( "Could not load file {}", path.generic_string() );
		return nullptr;
	}

	std::string temp;
	float x, y, z, w;

	while( !file.eof() ){
		file.clear();
		file >> temp;
		if( file.eof() )
			break;
		if( temp[0] == '#' ){
			file.ignore( std::numeric_limits<std::streamsize>::max(), file.widen( '\n' ));
		} else if( temp == "v" ){
			x = y = z = -12;
			file >> x >> y >> z;
			if( file.fail() ){
				KST_CORE_ERROR( "Could not read file {}", path.generic_string() );
				break;
			}
			file >> w;
			file.clear();
			//TODO color
			verts.push_back({{ x, y, z }, {( x + 1 ) *0.5, ( y + 1 ) * 0.5, ( z + 1 ) * 0.5 }});
		} else if( temp == "vt" ){
			KST_CORE_WARN( "Textures are currently not supported" );
			file.ignore( std::numeric_limits<std::streamsize>::max(), file.widen( '\n' ));
		} else if( temp == "vn" ){
			KST_CORE_INFO( "Ignoring normal" );
			file.ignore( std::numeric_limits<std::streamsize>::max(), file.widen( '\n' ));
		} else if( temp == "f" ){
			for( size_t i = 0; i < 3; ++i ){
				file >> x;
				indices.push_back( x - 1 );
				if( i != 2 )
					file.ignore( std::numeric_limits<std::streamsize>::max(), file.widen( ' ' ));
			}
			file.ignore( std::numeric_limits<std::streamsize>::max(), file.widen( '\n' ));
		} else if( temp == "mtllib" ){
			KST_CORE_WARN( "Materials are currently unsupported" );
			file.ignore( std::numeric_limits<std::streamsize>::max(), file.widen( '\n' ));
		} else if( temp == "o" ){
			file >> temp;
			KST_CORE_INFO( "Loading object {}", temp );
		} else if( temp == "usemtl" ){
			KST_CORE_WARN( "Materials are currently unsupported" );
			file.ignore( std::numeric_limits<std::streamsize>::max(), file.widen( '\n' ));
		} else if( temp == "s" ){
			file >> temp;
			if( temp == "on" ){
				KST_CORE_WARN( "Smooth shading is unsupported" );
			}
		} else {
			KST_CORE_WARN( "Found unsupported option {} in obj file", temp );
			file.ignore( std::numeric_limits<std::streamsize>::max(), file.widen( '\n' ));
		}
	}

	KST_CORE_INFO( "Finished loading file, {} verts, {} indices", verts.size(), indices.size() );
/*
	KST_CORE_INFO( "Verts:" );
	for( auto& v: verts ){
		KST_CORE_INFO( "{} {} {}, {} {} {}", v.pos.x, v.pos.y, v.pos.z, v.col.x, v.col.y, v.col.z );
	}

	KST_CORE_INFO( "Indices:" );
	for( auto& i: indices ){
		KST_CORE_INFO( "{}", i );
	}
	KST_CORE_INFO("");
*/

	vk::CommandBufferAllocateInfo alloc_inf(
			*shared_resources.copy_inf.pool,
			vk::CommandBufferLevel::ePrimary,
			2 );
	auto commandbuffers = shared_resources.device->device->allocateCommandBuffersUnique( alloc_inf );

	KST_VK_CopyInf cop_inf(
			shared_resources.device,
			shared_resources.copy_inf.queue,
			*commandbuffers[0] );

	static size_t vertex_amnt = 0;
	static size_t indic_amnt = 0;

	auto m = std::make_shared<VK_Mesh>();

	m->vertex_offset = vertex_amnt;
	m->vertex_size = verts.size();
	vertex_amnt += verts.size();

	m->index_offset = indic_amnt;
	m->index_amount = indices.size();
	indic_amnt += indices.size();

	VK_Utils::copy_to_buffer_device_local( cop_inf, shared_resources.mesh_data.vertex_buffer, shared_resources.mesh_data.vertex_buffer.current_offset, verts.data(), sizeof(verts[0]) * verts.size());
	shared_resources.mesh_data.vertex_buffer.current_offset += sizeof( verts[0] ) * verts.size();

	cop_inf.cmd_buf = *commandbuffers[1];

	VK_Utils::copy_to_buffer_device_local( cop_inf, shared_resources.mesh_data.index_buffer, shared_resources.mesh_data.index_buffer.current_offset, indices.data(), sizeof( indices[0] ) * indices.size());
	shared_resources.mesh_data.index_buffer.current_offset += sizeof( indices[0] ) * indices.size();

	return m;
}

/*
Mesh VK_MeshRegistry::requestMesh( const std::filesystem::path& p ){
	PROFILE_FUNCTION();

	KST_CORE_ASSERT( mesh_data.initialized, "MeshRegistry needs to be initialized first" );

	auto temp = meshes.find( p );

	if( temp == meshes.end())
		return Mesh::null;

	return meshes.at( p );
}

Mesh VK_MeshRegistry::requestOrLoadMesh( const std::filesystem::path & p ){
	PROFILE_FUNCTION();

	KST_CORE_ASSERT( mesh_data.initialized, "MeshRegistry needs to be initialized first" );

	auto temp = requestMesh( p );
	if( temp != Mesh::null ){
		return temp;
	}

	static Mesh mesh_id = 0;

	meshes.emplace( p, ++mesh_id );
	auto m = std::make_shared<VK_Mesh>();
	m->load_obj( p.string().c_str() );
	mesh_impls.emplace( mesh_id, m );
	return mesh_id;
}

std::shared_ptr<VK_Mesh> VK_MeshRegistry::getMeshImpl( Mesh m ){
	PROFILE_FUNCTION();

	return mesh_impls.at( m );
}
*/

template<>template<>
void VK_MeshRegistry::initialize(
		KST_VK_DeviceSurface* device,
		vk::Queue queue,
		vk::UniqueCommandPool&& pool,
		size_t vertex_hint,
		size_t index_hint ){
	KST_VK_BufferCreateInfo buf_cr_inf(
			device,
			vertex_hint,
			vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
			vk::MemoryPropertyFlagBits::eDeviceLocal );


	shared_resources.mesh_data.vertex_buffer.create( buf_cr_inf );

	buf_cr_inf.size = index_hint;
	buf_cr_inf.usage = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst;

	shared_resources.mesh_data.index_buffer.create( buf_cr_inf );

	VK_MeshRegistry::shared_resources.device = device;
	shared_resources.copy_inf.queue = queue;
	shared_resources.copy_inf.pool = std::move( pool );

	shared_resources.mesh_data.initialized = true;
}
