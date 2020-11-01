#pragma once

#include <kstpch.hpp>
#include "Renderer/Camera.hpp"

#include "Platform/Vulkan/VKContext.hpp"
#include "Renderer/Material.hpp"
#include "Platform/Vulkan/VKMaterial.hpp"

namespace Kestrel {
	struct RenderInfo {
		std::vector<vk::UniqueCommandBuffer> cmd_buffer;
		vk::Framebuffer* framebuffer; //TODO
		Material bound_mat = -1;
	};


	struct KST_VK_CameraRenderer: public CameraRenderer {
		public:
			KST_VK_CameraRenderer() = default;
			KST_VK_CameraRenderer( std::shared_ptr<KST_VK_DeviceSurface> surface );

			void setDeviceSurface( std::shared_ptr<KST_VK_DeviceSurface> surface );

			virtual void begin_scene(Camera &) override;
			virtual void draw(Entity e) override;
			virtual void endScene() override;

		private:
			void bindMat( VK_Material_T& mat );
			std::shared_ptr<KST_VK_DeviceSurface> device_surface;
			vk::UniqueCommandPool render_cmd_pool;
			RenderInfo render_info;
	};
}
