#include "Platform/Vulkan/VKMaterial.hpp"

#include "Platform/Vulkan/VKShader.hpp"
#include "Platform/Vulkan/VKContext.hpp"
#include "Platform/Vulkan/VKVertex.hpp"
#include <filesystem>
#include "Core/Application.hpp"

using namespace Kestrel;

KST_VK_Framebufferset::KST_VK_Framebufferset(): buffer(), lastUpdateID( -1 ){}

KST_VK_Framebufferset::KST_VK_Framebufferset( size_t size ): buffer( size ), lastUpdateID( - 1){}

KST_VK_Framebufferset::operator std::vector<vk::UniqueFramebuffer>& (){
	return buffer;
}

KST_VK_Framebufferset::operator const std::vector<vk::UniqueFramebuffer>& () const {
	return buffer;
}

bool KST_VK_Framebufferset::dirty( size_t current_id ){
	return current_id == lastUpdateID;
}


static std::vector<std::pair<ShaderType, const char*>> stages{
	{ ShaderType::Vertex, ".vert.spv" },
	{ ShaderType::TessControl, ".tesc.spv" },
	{ ShaderType::TessEvaluation, ".tese.spv" },
	{ ShaderType::Geometry, ".geom.spv" },
	{ ShaderType::Fragment, ".frag.spv" },
};

static vk::UniqueRenderPass createRenderPass( KST_VK_DeviceSurface& device ){
	PROFILE_FUNCTION();
	vk::UniqueRenderPass renderpass;

	std::array<vk::AttachmentDescription, 2> attachment_descriptions {
		vk::AttachmentDescription(		//image
				{},
				device.swapchains[0].format.format, //TODO
				vk::SampleCountFlagBits::e1,
				vk::AttachmentLoadOp::eLoad,
				vk::AttachmentStoreOp::eStore,
				vk::AttachmentLoadOp::eDontCare, //TODO add stencil
				vk::AttachmentStoreOp::eDontCare,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eTransferSrcOptimal ),
		vk::AttachmentDescription( 		//depthbuffer
				{},
				vk::Format::eD32Sfloat, //TODO
				vk::SampleCountFlagBits::e1,
				vk::AttachmentLoadOp::eClear,
				vk::AttachmentStoreOp::eDontCare, //TODO maybe store for analysis
				vk::AttachmentLoadOp::eDontCare,
				vk::AttachmentStoreOp::eDontCare,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eDepthStencilAttachmentOptimal )
	};

	std::array<vk::AttachmentReference, 1> attachment_references{
		vk::AttachmentReference( 0, vk::ImageLayout::eColorAttachmentOptimal ),
	};

	vk::AttachmentReference depth_attachment_reference {
		vk::AttachmentReference( 1, vk::ImageLayout::eDepthStencilAttachmentOptimal )
	};

	vk::SubpassDescription subpass_description(
			{},
			vk::PipelineBindPoint::eGraphics,
			{},
			attachment_references,				//Color attachments
			{},									//Resolve attachments
			&depth_attachment_reference,		//Depth attachment
			{} );

	vk::SubpassDependency sub_dependency(
			VK_SUBPASS_EXTERNAL,
			0,
			vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
			vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
			{}, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
			{} );

	vk::RenderPassCreateInfo render_pass_info(
			{},
			2, attachment_descriptions.data(),
			1, &subpass_description,
			1, &sub_dependency );

	renderpass = device.device->createRenderPassUnique( render_pass_info );

	return renderpass;
}

Material VK_Materials::loadMaterial( const char* shader_name ){
	PROFILE_FUNCTION();
	VK_Material_T newMat;

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

	auto attrib_desc = VK_Vertex::getAttributeDescription();

	auto binding_desc = VK_Vertex::getBindingDescription();

	vk::PipelineVertexInputStateCreateInfo vertex_input_info( {},
			binding_desc,
			attrib_desc );

	vk::PipelineInputAssemblyStateCreateInfo assembly_info( {},
			vk::PrimitiveTopology::eTriangleList,
			VK_FALSE );

	vk::PipelineViewportStateCreateInfo viewport_info( {}, 1, nullptr, 1, nullptr );

	vk::PipelineRasterizationStateCreateInfo rasterizer_info(
			{},
			VK_FALSE,
			VK_FALSE,
			vk::PolygonMode::eLine,
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
		{ vk::ShaderStageFlagBits::eVertex, 0, sizeof( glm::mat4 ) },
	};

	std::vector<vk::DescriptorSetLayoutBinding> layout_binding{
		vk::DescriptorSetLayoutBinding( 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, {} )
		};

	vk::DescriptorSetLayoutCreateInfo layout_cr_inf(
		{}, layout_binding );

	newMat.desc_layout = device->device->createDescriptorSetLayoutUnique( layout_cr_inf );

	vk::PipelineLayoutCreateInfo layout_info(
			{},
			1, &*newMat.desc_layout,
			1, push_constant_ranges.data() );

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

	newMat.renderpass = createRenderPass( *device );

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

	vk::GraphicsPipelineCreateInfo pipeline_info(
			{},
			stage_infos,
			&vertex_input_info,
			&assembly_info,
			nullptr, //TODO tesselation
			&viewport_info,
			&rasterizer_info,
			&multisample_info,
			&depth_info,
			&blend_state_info,
			&dynamic_state_info,
			*newMat.layout,
			*newMat.renderpass,
			0,
			vk::Pipeline{},
			-1 );

	newMat.pipeline = device->device->createGraphicsPipelineUnique( {}, pipeline_info ).value;

	static Material id = 1;
	newMat.id = id;
	materials.emplace( id, std::move( newMat ));
	return id++;
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
