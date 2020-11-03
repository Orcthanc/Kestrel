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
		size_t window_index = -1;
	};

	struct KST_VK_Buffer {
		vk::UniqueBuffer buffer;
		vk::UniqueDeviceMemory memory;
		void* data;
		size_t current_offset;
	};

	struct PresentSynchronization {
		vk::UniqueSemaphore start_rendering;
		vk::UniqueSemaphore image_presentable;
	};

	struct KST_VK_CameraRenderer: public CameraRenderer {
		public:
			KST_VK_CameraRenderer() = default;
			KST_VK_CameraRenderer( KST_VK_DeviceSurface* surface );

			virtual ~KST_VK_CameraRenderer() = default;

			void setDeviceSurface( KST_VK_DeviceSurface* surface );

			virtual void begin_scene( Camera &, size_t window_index ) override;
			virtual void draw( Entity e ) override;
			virtual void endScene() override;

		private:
			void bindMat( VK_Material_T& mat );
			//TODO smart ptr
			KST_VK_DeviceSurface* device_surface;
			vk::UniqueCommandPool render_cmd_pool;
			RenderInfo render_info;
			KST_VK_Buffer vertex_buffer;
			KST_VK_Buffer index_buffer;
			vk::Queue graphics_queue;
			vk::Queue present_queue;
			PresentSynchronization sync;

			static constexpr size_t vertex_size = 6 * sizeof( float ); //TODO textures
			static constexpr size_t max_vertices = 10000; //TODO performance measuring
			static constexpr size_t vert_buf_size = vertex_size * max_vertices;

			static constexpr size_t max_draw_triangles = 10000;
			static constexpr size_t index_buf_size = 3 * sizeof( uint32_t ) * max_draw_triangles;
	};
}
