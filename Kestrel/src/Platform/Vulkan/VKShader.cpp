#include "VKShader.hpp"
#include "Core/File.hpp"

using namespace Kestrel;

const std::string Kestrel::to_string( ShaderType t ){
	switch( t ){
		case ShaderType::None:
			return "None";
#define SHADERTYPE( e )		case ShaderType::e: return #e;
		SHADERTYPES
#undef SHADERTYPE
		case ShaderType::Error:
			return "Error";
	}

	KST_CORE_ASSERT( false, "Invalid Shader type {}", static_cast<std::underlying_type_t<ShaderType>>( t ));
	return "Invalid type";
}

ShaderType ShaderType_from_string( const std::string& s ){
	if( s == "None" ){
		return ShaderType::None;
	}
#define SHADERTYPE( e )		else if( s == #e ){ return ShaderType::e; }
	SHADERTYPES
#undef SHADERTYPE
	else{
		return ShaderType::Error;
	}
}

vk::ShaderStageFlagBits Kestrel::flag_bits_from_stage( ShaderType t ){
	switch( t ){
		case ShaderType::None:
			return vk::ShaderStageFlagBits{};
		case ShaderType::Vertex:
			return vk::ShaderStageFlagBits::eVertex;
		case ShaderType::TessControl:
			return vk::ShaderStageFlagBits::eTessellationControl;
		case ShaderType::TessEvaluation:
			return vk::ShaderStageFlagBits::eTessellationEvaluation;
		case ShaderType::Geometry:
			return vk::ShaderStageFlagBits::eGeometry;
		case ShaderType::Fragment:
			return vk::ShaderStageFlagBits::eFragment;
		case ShaderType::Compute:
			return vk::ShaderStageFlagBits::eCompute;
		case ShaderType::RayGeneration:
			return vk::ShaderStageFlagBits::eRaygenKHR;
		case ShaderType::RayIntersection:
			return vk::ShaderStageFlagBits::eRaygenKHR;
		case ShaderType::RayAnyHit:
			return vk::ShaderStageFlagBits::eAnyHitKHR;
		case ShaderType::RayClosestHit:
			return vk::ShaderStageFlagBits::eClosestHitKHR;
		case ShaderType::RayMiss:
			return vk::ShaderStageFlagBits::eMissKHR;
		case ShaderType::RayCallable:
			return vk::ShaderStageFlagBits::eCallableKHR;
		default:
			KST_CORE_ASSERT( false, "Invalid Shader type {}", static_cast<std::underlying_type_t<ShaderType>>( t ));
	}

	return {};
}

Shader::Shader( vk::Device device, const std::string& path, ShaderType type ){
	readShader( device, path, type );
}

void Shader::readShader( vk::Device device, const std::string &path, ShaderType type ){
	auto data = Filereader::read_uint8t( path );

	vk::ShaderModuleCreateInfo cr_inf( {}, data.size(), reinterpret_cast<const uint32_t*>( data.data() ));

	module = device.createShaderModuleUnique( cr_inf );
	this->type = type;
}
