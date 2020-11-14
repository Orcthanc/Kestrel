#pragma once

#include <kstpch.hpp>
#include "Renderer/Camera.hpp"

#include "Platform/Vulkan/VKContext.hpp"
#include "Renderer/Material.hpp"
#include "Platform/Vulkan/VKMaterial.hpp"
#include "Platform/Vulkan/VKVertex.hpp"

namespace Kestrel {

	struct KST_VK_Buffer {
		vk::UniqueBuffer buffer;
		vk::UniqueDeviceMemory memory;
		void* data = nullptr;
		size_t current_offset = 0;
	};

	struct KST_VK_RenderTarget {
		std::array<vk::UniqueImage, 2> color_depth;
		std::array<vk::UniqueDeviceMemory, 2> color_depth_mem;
		std::array<vk::UniqueImageView, 2> color_depth_view;

		vk::Extent2D size;

		vk::UniqueSemaphore render_ready_sema;
		vk::UniqueSemaphore render_done_sema;
		vk::UniqueFence render_done_fence;
	};

	template <size_t size>
	struct KST_VK_RenderChain {
		public:
			KST_VK_RenderTarget& startPass(){
				index = ( index + 1 ) % size;
				return targets[index];
			}

			auto begin() noexcept {
				return targets.begin();
			}

			auto end() noexcept {
				return targets.end();
			}

			const auto begin() const noexcept {
				return targets.begin();
			}

			const auto end() const noexcept {
				return targets.end();
			}

			const auto cbegin() const noexcept {
				return targets.cbegin();
			}

			const auto cend() const noexcept {
				return targets.cend();
			}

			auto& operator[]( size_t index ){
				return targets[index];
			}

			const auto& operator[]( size_t index ) const {
				return targets[index];
			}

			const size_t getIndex(){
				return index;
			}

		private:
			std::array<KST_VK_RenderTarget, size> targets;
			size_t index = 0;
	};

	struct RenderInfo {
		std::vector<vk::UniqueCommandBuffer> cmd_buffer;
		Material bound_mat = static_cast<Material>( -1 );
		size_t window_index = static_cast<size_t>( -1 );
		KST_VK_RenderTarget* target;
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
			void onSizeChange( bool resetSync );

			void createBuffers();
			void allocMemory();

			void createSynchronization();
			void createImages();
			void bindMat( VK_Material_T& mat );


			//TODO smart ptr
			KST_VK_DeviceSurface* device_surface = nullptr;

			vk::UniqueCommandPool render_cmd_pool;
			vk::UniqueCommandPool transfer_cmd_pool;

			KST_VK_Buffer vertex_buffer;
			KST_VK_Buffer index_buffer;
			KST_VK_Buffer uniform_buffer;

			vk::Queue graphics_queue;
			vk::Queue present_queue;
			vk::Queue transfer_queue;

			RenderInfo render_info;
			PresentSynchronization sync;

			static constexpr size_t vertex_size = sizeof( VK_Vertex );
			static constexpr size_t max_vertices = 10000; //TODO performance measuring
			static constexpr size_t vert_buf_size = vertex_size * max_vertices;

			static constexpr size_t max_draw_triangles = 10000;
			static constexpr size_t index_buf_size = 3 * sizeof( uint32_t ) * max_draw_triangles;

			static constexpr size_t frames = 2;

			KST_VK_RenderChain<frames> render_targets;
			size_t current_id = 0;
	};




}
