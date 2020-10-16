#include "Platform/Vulkan/VKMesh.hpp"

#include <fstream>

using namespace Kestrel;

void VK_Mesh::load_obj( const char *path ){
	PROFILE_FUNCTION();

	std::ifstream file( path, std::ios::binary );

	std::string temp;
	float x, y, z, w;

	while( !file.eof() ){
		file >> temp;
		if( temp[0] == '#' ){
			file.ignore( std::numeric_limits<std::streamsize>::max(), file.widen( '\n' ));
			continue;
		} else if( temp == "v" ){
			file >> x >> y >> z;
			if( file.failbit ){
				KST_CORE_ERROR( "Could not read file {}", path );
			}
			file >> w;
			verts.push_back({{ x, y, z }, { 0.9, 0.9, 0.9 }});
			KST_CORE_INFO( "Read vertex {}, {}, {}", x, y, z );
		} else if( temp == "f" ){
			file >> x >> y >> z;
			indices.push_back( x );
			indices.push_back( y );
			indices.push_back( z );
			for( ;; ){
				file >> w;
				if( file.failbit )
					break;
				KST_CORE_WARN( "Meshes with non-triangle faces are not supported" );
			}
		} else if( temp == "o" ){
			file >> temp;
			KST_CORE_INFO( "Loading object {}", temp );
		} else {
			KST_CORE_WARN( "Found unsupported option {} in obj file", temp );
			file.ignore( std::numeric_limits<std::streamsize>::max(), file.widen( '\n' ));
		}
	}

	KST_CORE_INFO( "Finished loading file" );
}

void VK_Mesh::unload(){
	verts.clear();
	indices.clear();
}

bool VK_Mesh::loaded(){
	return !indices.empty();
}
