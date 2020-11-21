#include "Platform/Vulkan/VKMesh.hpp"

#include <fstream>

using namespace Kestrel;

std::map<const std::filesystem::path, std::shared_ptr<VK_Mesh>> VK_MeshRegistry::meshes;

std::shared_ptr<MeshImpl> VK_Mesh::create( const std::filesystem::path& p ){
	PROFILE_FUNCTION();

	auto temp = std::make_shared<VK_Mesh>();
	VK_MeshRegistry::register_mesh( p, temp );
	temp->load_obj( p.c_str() );
	return temp;
}

std::shared_ptr<MeshImpl> VK_Mesh::getMesh( const std::filesystem::path& p ){
	PROFILE_FUNCTION();

	return VK_MeshRegistry::getMesh( p );
}

void VK_Mesh::load_obj( const char *path ){
	PROFILE_FUNCTION();

	std::ifstream file( path, std::ios::binary );

	if( !file.is_open() ){
		KST_CORE_WARN( "Could not load file {}", path );
		return;
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
				KST_CORE_ERROR( "Could not read file {}", path );
				break;
			}
			file >> w;
			file.clear();
			//TODO color
			verts.push_back({{ x, y, z }, { ( x + 1 ) *0.5, ( y + 1 ) * 0.5, ( z + 1 ) * 0.5 }});
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
}

const BufferView<float> VK_Mesh::getVertices(){
	PROFILE_FUNCTION();
	return { reinterpret_cast<float*>( verts.data()), verts.size() * sizeof( verts[0] ), sizeof( verts[0] )}; //TODO hope padding doesn't eat this
}

const BufferView<uint32_t> VK_Mesh::getIndices(){
	PROFILE_FUNCTION();
	return { indices.data(), indices.size() * sizeof( indices[0] ), sizeof( indices[0] )};
}

std::shared_ptr<VK_Mesh> VK_MeshRegistry::getMesh( const std::filesystem::path& p ){
	PROFILE_FUNCTION();
	return meshes[p];
}

void VK_MeshRegistry::register_mesh( const std::filesystem::path& p, std::shared_ptr<VK_Mesh> mesh ){
	PROFILE_FUNCTION();
	meshes.emplace( p, std::move( mesh ));
}
