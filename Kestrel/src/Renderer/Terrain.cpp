#include "Terrain.hpp"

#include "Platform/Vulkan/VKTerrain.hpp"

using namespace Kestrel;

Terrain::Terrain(){
	static size_t id = 0;
	this->id = ++id;
}

Terrain::Terrain( size_t id ){
	this->id = id;
}

Terrain Terrain::createTerrain( const TerrainInfo& t ){
	switch( render_path ){
		case RenderPath::Vulkan:
			return VK_TerrainRegistry::createTerrain( t );
		default:
			KST_CORE_VERIFY( false, "Unsupported rendering path {}", (int)render_path );
	}
}
