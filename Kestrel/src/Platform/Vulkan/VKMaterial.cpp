#include "Platform/Vulkan/VKMaterial.hpp"

#include "Platform/Vulkan/VKShader.hpp"
#include "Platform/Vulkan/VKContext.hpp"
#include "Platform/Vulkan/VKVertex.hpp"

#include <filesystem>

#include "Core/Application.hpp"

#include "nlohmann/json.hpp"

using namespace Kestrel;

using json = nlohmann::json;

BindingInfo::BindingInfo(
		KST_VK_DeviceSurface& device,
		RendererID id,
		vk::Extent2D img_size,
		size_t img_bind_index,
		vk::CommandBuffer cmd_buffer,
		vk::Buffer uniform_buffer,
		RenderModeFlags render_mode ):
	device( device ),
	id( id ),
	img_size( img_size ),
	img_bind_index( img_bind_index ),
	cmd_buffer( cmd_buffer ),
	uniform_buffer( uniform_buffer ),
	render_mode( render_mode ){}


static std::map<std::string, ShaderType> stages{
	{ "vert", ShaderType::Vertex },
	{ "tesc", ShaderType::TessControl },
	{ "tese", ShaderType::TessEvaluation },
	{ "geom", ShaderType::Geometry },
	{ "frag", ShaderType::Fragment },
};

template<>
std::shared_ptr<VK_Material_T> VK_Materials::load_resource( const std::filesystem::path& shader_name ){
	PROFILE_FUNCTION();

	std::filesystem::path path( shader_name );
	path += std::string( ".json" );
	std::ifstream in( path );
	json j;
	in >> j;

	path.remove_filename();

	auto newMat = std::make_shared<VK_Material_T>();

	KST_CORE_INFO( "Loading material {}", shader_name.generic_string() );
	std::vector<KST_VK_Shader> shaders;

	bool tessellation = false;

	for( auto& [name, stage]: j["stages"].items() ){
		if( name == "tesc" )
			tessellation = true;
		shaders.push_back( KST_VK_Shader( *shared_resources.device->device, (path / stage["file"].get<std::string>()).string(), stages[name] ));
		KST_CORE_INFO( "Loading file {}", (path / stage["file"].get<std::string>()).string() );
	}

	std::vector<vk::PipelineShaderStageCreateInfo> stage_infos;
	std::vector<vk::PipelineShaderStageCreateInfo> log_stage_infos;
	
	uint8_t log_off = 0;
	uint8_t log_on = 1;
	vk::SpecializationMapEntry log_entry( 0, 0, 1 );
	vk::SpecializationInfo spec_info(
			1, &log_entry,
			1, &log_off );
	
	vk::SpecializationInfo spec_info_log(
			1, &log_entry,
			1, &log_on );
	
	for( auto& s: shaders ){
		stage_infos.push_back( vk::PipelineShaderStageCreateInfo( {}, flag_bits_from_stage( s.type ), *s.module, "main", nullptr ));
		log_stage_infos.push_back( vk::PipelineShaderStageCreateInfo( {}, flag_bits_from_stage( s.type ), *s.module, "main", nullptr ));
	}

	std::vector<vk::VertexInputAttributeDescription> attrib_desc;
	std::vector<vk::VertexInputBindingDescription> binding_desc;

	vk::Format format;

	for( auto& attribute: j["attribute_descriptions"] ){
		switch( attribute["size"].get<int>() ){
			case 1:
				format = vk::Format::eR32Sfloat;
				break;
			case 2:
				format = vk::Format::eR32G32Sfloat;
				break;
			case 3:
				format = vk::Format::eR32G32B32Sfloat;
				break;
			case 4:
				format = vk::Format::eR32G32B32A32Sfloat;
				break;
			default:
				KST_CORE_VERIFY( false, "Shader attribute can not have a vector with more than 4 floats (has {})", attribute["byte"].get<int>() );
		}

		attrib_desc.emplace_back(
				attribute["location"].get<uint32_t>(),
				attribute["binding"].get<uint32_t>(),
				format,
				attribute["offset"].get<uint32_t>()
			);
	}

	for( auto& binding: j["binding_descriptions"] ){
		vk::VertexInputRate inputrate;
		if( binding["inputrate"] == "v" ){
			inputrate = vk::VertexInputRate::eVertex;
		} else if( binding["inputrate"] == "i" ){
			inputrate = vk::VertexInputRate::eInstance;
		} else {
			KST_CORE_VERIFY( false, "Invalid vertex input rate {}", binding["inputrate"] );
		}

		binding_desc.emplace_back(
				binding["binding"].get<uint32_t>(),
				binding["stride"].get<uint32_t>(),
				inputrate );
	}

	vk::PipelineVertexInputStateCreateInfo vertex_input_info( {},
			binding_desc,
			attrib_desc );

	vk::PipelineInputAssemblyStateCreateInfo assembly_info( {},
			tessellation ? vk::PrimitiveTopology::ePatchList : vk::PrimitiveTopology::eTriangleList,
			VK_FALSE );

	vk::PipelineViewportStateCreateInfo viewport_info( {}, 1, nullptr, 1, nullptr );

	vk::PipelineRasterizationStateCreateInfo rasterizer_info(
			{},
			VK_FALSE,
			VK_FALSE,
			vk::PolygonMode::eFill,
			vk::CullModeFlagBits::eBack,
			vk::FrontFace::eClockwise,
			VK_FALSE,
			0,
			0,
			0,
			1 );

	vk::PipelineMultisampleStateCreateInfo multisample_info(
			{},
			vk::SampleCountFlagBits::e1,
			VK_FALSE,
			1,
			nullptr,
			VK_FALSE,
			VK_FALSE );

	vk::PipelineColorBlendAttachmentState color_blend_attachment(
			VK_FALSE,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eZero,
			vk::BlendOp::eAdd,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eZero,
			vk::BlendOp::eAdd,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA );

	std::array<vk::PipelineColorBlendAttachmentState, 1> color_blend_attachments{ color_blend_attachment };
	std::array<float, 4> blend_constants{ 0, 0, 0, 0 };
	vk::PipelineColorBlendStateCreateInfo blend_state_info(
			{},
			VK_FALSE,
			vk::LogicOp::eCopy,
			color_blend_attachments,
			blend_constants);

	std::vector<vk::PushConstantRange> push_constant_ranges;

	for( auto& range: j["push_constants"] ){
		vk::ShaderStageFlags stages;
		for( auto& stage: range["stages"] ){
			if( stage == "vert" )
				stages |= vk::ShaderStageFlagBits::eVertex;
			else if( stage == "geom" )
				stages |= vk::ShaderStageFlagBits::eGeometry;
			else if( stage == "tesc" )
				stages |= vk::ShaderStageFlagBits::eTessellationControl;
			else if( stage == "tese" )
				stages |= vk::ShaderStageFlagBits::eTessellationEvaluation;
			else if( stage == "frag" )
				stages |= vk::ShaderStageFlagBits::eFragment;
			else
				KST_CORE_ERROR( "Ignoring invalid stage: {}", stage.get<std::string>());
		}

		push_constant_ranges.emplace_back( stages, range["offset"], range["size"] );
	}

	std::vector<vk::DescriptorSetLayoutBinding> layout_binding{
		vk::DescriptorSetLayoutBinding(
				0,
				vk::DescriptorType::eUniformBuffer,
				1,
				vk::ShaderStageFlagBits::eTessellationEvaluation | vk::ShaderStageFlagBits::eTessellationControl,
				{} )
		};

	vk::DescriptorSetLayoutCreateInfo layout_cr_inf(
		{}, layout_binding );

	newMat->desc_layout = shared_resources.device->device->createDescriptorSetLayoutUnique( layout_cr_inf );

	vk::PipelineLayoutCreateInfo layout_info(
			{},
			1, &*newMat->desc_layout,
			static_cast<uint32_t>( push_constant_ranges.size()), push_constant_ranges.data() );

	newMat->layout = shared_resources.device->device->createPipelineLayoutUnique( layout_info );

	std::array<vk::DescriptorPoolSize, 1> desc_pool_size{
		vk::DescriptorPoolSize( vk::DescriptorType::eUniformBuffer, 1 )
	};

	vk::DescriptorPoolCreateInfo desc_pool_inf(
			vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
			1,
			desc_pool_size );

	newMat->desc_pool = shared_resources.device->device->createDescriptorPoolUnique( desc_pool_inf );

	std::array<vk::DynamicState, 2> dynamic_states{
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor };

	vk::PipelineDynamicStateCreateInfo dynamic_state_info(
			{},
			dynamic_states );

	vk::PipelineDepthStencilStateCreateInfo depth_info (
			{},
			VK_TRUE,							//Depth test
			VK_TRUE,							//Depth write
			vk::CompareOp::eLessOrEqual,		//Depth op
			VK_FALSE,							//Depth bounds test
			VK_FALSE,							//Stencil test
			{},									//Stencil front
			{},									//Stencil back
			0,									//Min depth
			1 );								//Max depth

	vk::PipelineTessellationStateCreateInfo tess_state_inf(
			{},
			3 );

	vk::GraphicsPipelineCreateInfo pipeline_info(
			{},
			stage_infos,
			&vertex_input_info,
			&assembly_info,
			tessellation ? &tess_state_inf : nullptr,
			&viewport_info,
			&rasterizer_info,
			&multisample_info,
			&depth_info,
			&blend_state_info,
			&dynamic_state_info,
			*newMat->layout,
			*KST_VK_Context::get().device.renderpass,
			0,
			{},
			-1 );

	using flag_integral = std::underlying_type_t<RenderModeFlags>;

	newMat->pipelines.reserve( static_cast<flag_integral>( RenderModeFlags::eAllFlags ));

	for( flag_integral i = 0; i <= static_cast<flag_integral>( RenderModeFlags::eAllFlags ); ++i ){
		if( any_flag( RenderModeFlags::eInverse & i )){
			depth_info.depthCompareOp = vk::CompareOp::eGreater;
		} else {
			depth_info.depthCompareOp = vk::CompareOp::eLess;
		}

		if( any_flag( RenderModeFlags::eLogarithmic & i )){
			pipeline_info.pStages = log_stage_infos.data();
		} else {
			pipeline_info.pStages = stage_infos.data();
		}

		if( any_flag( RenderModeFlags::eWireframe & i )){
			rasterizer_info.polygonMode = vk::PolygonMode::eLine;
		} else {
			rasterizer_info.polygonMode = vk::PolygonMode::eFill;
		}

		newMat->pipelines.emplace_back( std::move( shared_resources.device->device->createGraphicsPipelineUnique( {}, pipeline_info ).value ));
		pipeline_info.basePipelineHandle = *newMat->pipelines[0];
	}

	static uint32_t id = 0;

	newMat->id = ++id;

	return newMat;
}

void VK_Material_T::bind( const BindingInfo& bind_inf ){
	PROFILE_FUNCTION();

	vk::DescriptorSetAllocateInfo desc_set_alloc_inf(
			*desc_pool,
			1,
			&*desc_layout );

	if( !desc_sets.contains( bind_inf.id ))
		desc_sets.emplace( bind_inf.id, bind_inf.device.device->allocateDescriptorSetsUnique( desc_set_alloc_inf ));

	auto& desc_set = desc_sets.at( bind_inf.id );

	std::array<vk::DescriptorBufferInfo, 1> desc_buf_info{
		vk::DescriptorBufferInfo( bind_inf.uniform_buffer,
			0,
			sizeof( VK_ViewProj ))
	};

	vk::WriteDescriptorSet write_desc(
			*desc_set[0],
			0, 0,
			vk::DescriptorType::eUniformBuffer,
			{},
			desc_buf_info,
			{}
		);

	bind_inf.device.device->updateDescriptorSets( 1, &write_desc, 0, nullptr );

	bind_inf.cmd_buffer.bindPipeline(
			vk::PipelineBindPoint::eGraphics,
			*pipelines[ static_cast<std::underlying_type_t<RenderModeFlags>>( bind_inf.render_mode )] );

	vk::Viewport viewport(
			0, 0,
			static_cast<float>( bind_inf.img_size.width ), static_cast<float>( bind_inf.img_size.height ),
			0, 1 );

	bind_inf.cmd_buffer.setViewport( 0, 1, &viewport );

	vk::Rect2D scissor(
			{ 0, 0 },
			{ bind_inf.img_size.width, bind_inf.img_size.height });

	bind_inf.cmd_buffer.setScissor( 0, 1, &scissor );

	bind_inf.cmd_buffer.bindDescriptorSets( vk::PipelineBindPoint::eGraphics, *layout, 0, *desc_set[0], {} );
}

template<>template<>
void VK_Materials::initialize( KST_VK_DeviceSurface* device ){
	shared_resources.device = device;
}
