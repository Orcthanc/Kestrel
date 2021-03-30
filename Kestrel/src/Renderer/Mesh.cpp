#include "Mesh.hpp"

#include "Platform/Vulkan/VKMesh.hpp"

using namespace Kestrel;

Mesh Mesh::createMesh( const std::filesystem::path &p ){
	switch( render_path ){
		case RenderPath::Vulkan:
		{
			return VK_MeshRegistry::requestResourceHandle( p );
		}
		case RenderPath::None:
		{
			return Mesh::null;
		}
	}

	KST_CORE_VERIFY( false, "Found invalid render_path {}", render_path );
}
