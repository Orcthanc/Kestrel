#include "Platform/Vulkan/VKMaterial.hpp"

#include "Platform/Vulkan/VKShader.hpp"
#include "Platform/Vulkan/VKContext.hpp"
#include <filesystem>

using namespace Kestrel;

static std::vector<std::pair<ShaderType, const char*>> stages{
	{ ShaderType::Vertex, ".vert.spv" },
	{ ShaderType::TessControl, ".tesc.spv" },
	{ ShaderType::TessEvaluation, ".tese.spv" },
	{ ShaderType::Geometry, ".geom.spv" },
	{ ShaderType::Fragment, ".frag.spv" },
};

Material VK_Materials::loadMaterial( const char* shader_name ){
	KST_CORE_INFO( "Loading material {}", shader_name );
	std::vector<Shader> shaders;

	for( auto& stage: stages ){
		std::filesystem::path p{ shader_name + std::string( stage.second )};
		if( std::filesystem::exists( p )){
			KST_CORE_INFO( "Loading shader {}", p.string() );
			shaders.emplace_back( *device->device, p.string(), stage.first );
		}
	}

	std::vector<vk::PipelineShaderStageCreateInfo> stage_infos;

	for( auto& s: shaders ){
		stage_infos.push_back({ {}, flag_bits_from_stage( s.type ), *s.module, "main", {} });
	}

	return 1;
}

VK_Materials& VK_Materials::getInstance(){
	static VK_Materials mats;
	return mats;
}
