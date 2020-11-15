#pragma once

#include <kstpch.hpp>

#include <vulkan/vulkan.hpp>

namespace Kestrel {

#ifndef SHADERTYPES

#define SHADERTYPES			\
SHADERTYPE( Vertex )		\
SHADERTYPE( TessControl )		\
SHADERTYPE( TessEvaluation )		\
SHADERTYPE( Geometry )		\
SHADERTYPE( Fragment )		\
SHADERTYPE( Compute )		\
SHADERTYPE( RayGeneration )		\
SHADERTYPE( RayIntersection )		\
SHADERTYPE( RayAnyHit )		\
SHADERTYPE( RayClosestHit )		\
SHADERTYPE( RayMiss )		\
SHADERTYPE( RayCallable )

#endif


	enum class ShaderType {
		None = 0,
#define SHADERTYPE( e ) e,
		SHADERTYPES
#undef SHADERTYPE
		Error = 0xffff,
	};

	extern const std::string to_string( ShaderType t );
	extern ShaderType ShaderType_from_string( const std::string& s );
	extern vk::ShaderStageFlagBits flag_bits_from_stage( ShaderType t );

	struct KST_VK_Shader {
		KST_VK_Shader() = default;
		KST_VK_Shader( vk::Device device, const std::string& path, ShaderType type );

		KST_VK_Shader( const KST_VK_Shader& ) = delete;
		KST_VK_Shader( KST_VK_Shader&& ) = default;
		KST_VK_Shader& operator=( const KST_VK_Shader& ) = delete;
		KST_VK_Shader& operator=( KST_VK_Shader&& ) = default;

		void readShader( vk::Device device, const std::string& path, ShaderType type );

		ShaderType type;
		vk::UniqueShaderModule module;
	};
}
