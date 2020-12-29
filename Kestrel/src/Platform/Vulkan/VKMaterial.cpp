#include "Platform/Vulkan/VKMaterial.hpp"

#include "Platform/Vulkan/VKShader.hpp"
#include "Platform/Vulkan/VKContext.hpp"
#include "Platform/Vulkan/VKVertex.hpp"
#include <filesystem>
#include "Core/Application.hpp"

using namespace Kestrel;

KST_VK_Framebufferset::KST_VK_Framebufferset(): buffer(){}

KST_VK_Framebufferset::KST_VK_Framebufferset( size_t size ): buffer( size ){}

KST_VK_Framebufferset::operator std::vector<vk::UniqueFramebuffer>& (){
	return buffer;
}

KST_VK_Framebufferset::operator const std::vector<vk::UniqueFramebuffer>& () const {
	return buffer;
}

bool KST_VK_Framebufferset::dirty( size_t current_id ){
	return current_id == lastUpdateID;
}

BindingInfo::BindingInfo(
		KST_VK_DeviceSurface& device,
		RendererID id,
		size_t dirty_check_id,
		std::vector<std::array<vk::ImageView, 2>>& imgs,
		vk::Extent2D img_size,
		size_t img_bind_index,
		vk::CommandBuffer cmd_buffer,
		vk::Buffer uniform_buffer,
		RenderModeFlags render_mode ):
	device( device ),
	id( id ),
	dirty_check_id( dirty_check_id ),
	img_views( imgs ),
	img_size( img_size ),
	img_bind_index( img_bind_index ),
	cmd_buffer( cmd_buffer ),
	uniform_buffer( uniform_buffer ),
	render_mode( render_mode ){}


static std::vector<std::pair<ShaderType, const char*>> stages{
	{ ShaderType::Vertex, ".vert.spv" },
	{ ShaderType::TessControl, ".tesc.spv" },
	{ ShaderType::TessEvaluation, ".tese.spv" },
	{ ShaderType::Geometry, ".geom.spv" },
	{ ShaderType::Fragment, ".frag.spv" },
};

Material VK_Materials::loadMaterial( const char* shader_name ){
	PROFILE_FUNCTION();

	if( material_names.contains( shader_name ))
		return material_names.at( shader_name );


	VK_Material_T newMat;

	KST_CORE_INFO( "Loading material {}", shader_name );
	std::vector<KST_VK_Shader> shaders;

	for( auto& stage: stages ){
		std::filesystem::path p{ shader_name + std::string( stage.second )};
		if( std::filesystem::exists( p )){
			KST_CORE_INFO( "Loading shader {}", p.string() );
			shaders.emplace_back( *device->device, p.string(), stage.first );
		}
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
		if( s.type == ShaderType::Vertex || s.type == ShaderType::Fragment || s.type == ShaderType::TessEvaluation ){
			stage_infos.push_back( vk::PipelineShaderStageCreateInfo( {}, flag_bits_from_stage( s.type ), *s.module, "main", &spec_info ));
			log_stage_infos.push_back( vk::PipelineShaderStageCreateInfo( {}, flag_bits_from_stage( s.type ), *s.module, "main", &spec_info_log ));
		} else {
			stage_infos.push_back({ {}, flag_bits_from_stage( s.type ), *s.module, "main", {} });
			log_stage_infos.push_back({ {}, flag_bits_from_stage( s.type ), *s.module, "main", {} });
		}
	}

	auto attrib_desc = VK_Vertex::getAttributeDescription();

	auto binding_desc = VK_Vertex::getBindingDescription();

	vk::PipelineVertexInputStateCreateInfo vertex_input_info( {},
			binding_desc,
			attrib_desc );

	vk::PipelineInputAssemblyStateCreateInfo assembly_info( {},
			vk::PrimitiveTopology::ePatchList,
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

	std::vector<vk::PushConstantRange> push_constant_ranges{
		vk::PushConstantRange( vk::ShaderStageFlagBits::eTessellationControl | vk::ShaderStageFlagBits::eTessellationEvaluation, 0, sizeof( VK_UniformBufferObj )),
	};

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

	newMat.desc_layout = device->device->createDescriptorSetLayoutUnique( layout_cr_inf );

	vk::PipelineLayoutCreateInfo layout_info(
			{},
			1, &*newMat.desc_layout,
			static_cast<uint32_t>( push_constant_ranges.size()), push_constant_ranges.data() );

	newMat.layout = device->device->createPipelineLayoutUnique( layout_info );

	std::array<vk::DescriptorPoolSize, 1> desc_pool_size{
		vk::DescriptorPoolSize( vk::DescriptorType::eUniformBuffer, 1 )
	};

	vk::DescriptorPoolCreateInfo desc_pool_inf(
			vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
			1,
			desc_pool_size );

	newMat.desc_pool = device->device->createDescriptorPoolUnique( desc_pool_inf );

	std::array<vk::DynamicState, 2> dynamic_states{
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor };

	vk::PipelineDynamicStateCreateInfo dynamic_state_info(
			{},
			dynamic_states );

	newMat.renderpass = *KST_VK_Context::get().device.renderpass;

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
			&tess_state_inf,
			&viewport_info,
			&rasterizer_info,
			&multisample_info,
			&depth_info,
			&blend_state_info,
			&dynamic_state_info,
			*newMat.layout,
			newMat.renderpass,
			0,
			{},
			-1 );

	using flag_integral = std::underlying_type_t<RenderModeFlags>;

	newMat.pipelines.reserve( static_cast<flag_integral>( RenderModeFlags::eAllFlags ));

	for( flag_integral i = 0; i <= static_cast<flag_integral>( RenderModeFlags::eAllFlags ); ++i ){
		if( any_flag( RenderModeFlags::eInverse & i )){
			depth_info.depthCompareOp = vk::CompareOp::eGreaterOrEqual;
		} else {
			depth_info.depthCompareOp = vk::CompareOp::eLessOrEqual;
		}

		if( any_flag( RenderModeFlags::eLogarithmic & i )){
			pipeline_info.pStages = log_stage_infos.data();
		} else {
			pipeline_info.pStages = stage_infos.data();
		}

		if( any_flag( RenderModeFlags::eIntegerDepth & i )){
			//TODO
		} else {
			//TODO
		}

		if( any_flag( RenderModeFlags::eWireframe & i )){
			rasterizer_info.polygonMode = vk::PolygonMode::eLine;
		} else {
			rasterizer_info.polygonMode = vk::PolygonMode::eFill;
		}

		newMat.pipelines.emplace_back( std::move( device->device->createGraphicsPipelineUnique( {}, pipeline_info ).value ));
		pipeline_info.basePipelineHandle = *newMat.pipelines[0];
	}
	static Material id = 1;
	newMat.id = id;
	materials.emplace( id, std::move( newMat ));
	material_names[shader_name] = id;
	return id++;
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

	vk::Framebuffer buf;

	// Creates if it does not exist and checks if up to date
	if( !framebuffers[ bind_inf.id ].dirty( bind_inf.dirty_check_id )){
		std::vector<vk::UniqueFramebuffer>& framebufs = framebuffers[ bind_inf.id ].buffer;

		framebufs.resize( bind_inf.img_views.size() );

		for( size_t i = 0; i < framebufs.size(); ++i ){

			std::vector<vk::ImageView> attachments{
				bind_inf.img_views[i][0],
				bind_inf.img_views[i][1] };

			vk::FramebufferCreateInfo framebuf_inf(
					{},
					renderpass,
					attachments,
					bind_inf.img_size.width,
					bind_inf.img_size.height,
					1
				);

			framebufs[i] = bind_inf.device.device->createFramebufferUnique( framebuf_inf );

		}

		buf = *framebufs[ bind_inf.img_bind_index ];
	} else {
		buf = *framebuffers.at( bind_inf.id ).buffer[ bind_inf.img_bind_index ];
	}

	std::array<vk::ClearValue, 2> clear_values{
		vk::ClearColorValue(std::array<float, 4>{ 0.0, 0.0, 0.0, 0.0 }),
		vk::ClearDepthStencilValue( 1.0f, 0 )
	};

	if( any_flag( bind_inf.render_mode & RenderModeFlags::eInverse )){
		clear_values[1] = vk::ClearDepthStencilValue( 0.0f, 0 );
	}

	vk::RenderPassBeginInfo beg_inf(
			renderpass,
			buf,
			{{ 0, 0 }, bind_inf.img_size },
			clear_values
		);

	bind_inf.cmd_buffer.beginRenderPass( beg_inf, vk::SubpassContents::eInline );

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

VK_Materials& VK_Materials::getInstance(){
	static VK_Materials mats;
	return mats;
}

VK_Material_T& VK_Materials::operator[]( Material mat ){
	PROFILE_FUNCTION();

	KST_CORE_ASSERT( materials.contains( mat ), "Could not load material {}", mat );
	return materials.at( mat );
}

const VK_Material_T& VK_Materials::operator[]( Material mat ) const {
	PROFILE_FUNCTION();

	KST_CORE_ASSERT( materials.contains( mat ), "Could not load material {}", mat );
	return materials.at( mat );
}
