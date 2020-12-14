#pragma once

#include <kstpch.hpp>

#include <vulkan/vulkan.hpp>

#include "Platform/Vulkan/VKContext.hpp"

namespace Kestrel {

	struct KST_VK_BufferCreateInfo {
		KST_VK_BufferCreateInfo() = default;
		KST_VK_BufferCreateInfo( KST_VK_DeviceSurface* device, size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memory_properties, vk::SharingMode sharing_mode = vk::SharingMode::eExclusive, std::vector<uint32_t> queue_families = {} );

		KST_VK_DeviceSurface* device{};
		size_t size{};
		vk::BufferUsageFlags usage{};
		vk::MemoryPropertyFlags memory_properties{};
		vk::SharingMode sharing_mode{};
		std::vector<uint32_t> queue_families{};
	};

	struct KST_VK_CopyInf {
		KST_VK_DeviceSurface* device;
		vk::Queue queue;
		vk::CommandBuffer cmd_buf;
	};

	struct KST_VK_Buffer {
		KST_VK_Buffer() = default;
		~KST_VK_Buffer() = default;

		KST_VK_Buffer( const KST_VK_Buffer& ) = delete;
		KST_VK_Buffer( KST_VK_Buffer&& ) = default;

		KST_VK_Buffer& operator=( const KST_VK_Buffer& ) = delete;
		KST_VK_Buffer& operator=( KST_VK_Buffer&& ) = default;

		void create( const KST_VK_BufferCreateInfo& );

		void* map();
		void* map( vk::DeviceSize offset, vk::DeviceSize size );
		void unmap();

		KST_VK_DeviceSurface* device;
		vk::UniqueBuffer buffer;
		vk::UniqueDeviceMemory memory;
		void* data = nullptr;
		size_t current_offset = 0;
		size_t size = 0;
	};

	struct VK_Utils {
		VK_Utils() = delete;
		~VK_Utils() = delete;
		static void copy_to_buffer_device_local( const KST_VK_CopyInf&, KST_VK_Buffer& target, vk::DeviceSize offset, void* data, size_t data_size );
	};
}
