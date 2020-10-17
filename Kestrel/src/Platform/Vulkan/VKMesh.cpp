#include "Platform/Vulkan/VKMesh.hpp"

#include <fstream>

using namespace Kestrel;

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
			verts.push_back({{ x, y, z }, { 0.9, 0.9, 0.9 }});
		} else if( temp == "vt" ){
			KST_CORE_WARN( "Textures are currently not supported" );
			file.ignore( std::numeric_limits<std::streamsize>::max(), file.widen( '\n' ));
		} else if( temp == "vn" ){
			KST_CORE_INFO( "Ignoring normal" );
			file.ignore( std::numeric_limits<std::streamsize>::max(), file.widen( '\n' ));
		} else if( temp == "f" ){
/*			file >> x >> y >> z;
			indices.push_back( x );
			indices.push_back( y );
			indices.push_back( z );
			*/
			for( size_t i = 0; i < 3; ++i ){
				file >> x;
				indices.push_back( x );
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

	KST_CORE_INFO( "Finished loading file" );
}

void VK_Mesh::unload(){
	verts.clear();
	indices.clear();
}

bool VK_Mesh::loaded(){
	return !indices.empty();
}
