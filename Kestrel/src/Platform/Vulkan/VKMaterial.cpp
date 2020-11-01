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

static vk::UniqueRenderPass createRenderPass( KST_VK_DeviceSurface& device ){
	PROFILE_FUNCTION();
	vk::UniqueRenderPass renderpass;

	vk::AttachmentDescription attachment_description(
			{},
			device.swapchains[0].format.format, //TODO
			vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eLoad,
			vk::AttachmentStoreOp::eStore,
			vk::AttachmentLoadOp::eDontCare, //TODO add stencil
			vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::ePresentSrcKHR );

	std::array<vk::AttachmentReference, 1> attachment_references{
		vk::AttachmentReference( 0, vk::ImageLayout::eColorAttachmentOptimal ),
	};

	vk::SubpassDescription subpass_description(
			{},
			vk::PipelineBindPoint::eGraphics,
			{},
			attachment_references,
			{},
			{},
			{} );

	vk::SubpassDependency sub_dependency(
			VK_SUBPASS_EXTERNAL, 0,
			vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput,
			{}, vk::AccessFlagBits::eColorAttachmentWrite,
			{} );

	vk::RenderPassCreateInfo render_pass_info(
			{},
			1, &attachment_description,
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
	//TODO maybe delete Vertex3f3f

	std::array<vk::VertexInputAttributeDescription, 2> attrib_desc = {
		vk::VertexInputAttributeDescription( 0, 0, vk::Format::eR32G32B32Sfloat, 0 ),
		vk::VertexInputAttributeDescription( 1, 0, vk::Format::eR32G32B32Sfloat, 3 ),
	};

	vk::VertexInputBindingDescription binding_desc( 0, 6, vk::VertexInputRate::eVertex );

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

	vk::PipelineLayoutCreateInfo layout_info(
			{},
			{},
			{} ); //TODO

	newMat.layout = device->device->createPipelineLayoutUnique( layout_info );

	std::array<vk::DynamicState, 2> dynamic_states{
		vk::DynamicState::eViewport,
			vk::DynamicState::eScissor };

	vk::PipelineDynamicStateCreateInfo dynamic_state_info(
			{},
			dynamic_states );

	newMat.renderpass = createRenderPass( *device );

	vk::GraphicsPipelineCreateInfo pipeline_info(
			{},
			stage_infos,
			&vertex_input_info,
			&assembly_info,
			nullptr, //TODO
			&viewport_info,
			&rasterizer_info,
			&multisample_info,
			nullptr,
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
