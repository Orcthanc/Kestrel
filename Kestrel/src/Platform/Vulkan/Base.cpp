#include "Platform/Vulkan/Base.hpp"

using namespace Kestrel;

KST_VK_BufferCreateInfo::KST_VK_BufferCreateInfo(
		KST_VK_DeviceSurface* device,
		size_t size,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags memory_properties,
		vk::SharingMode sharing_mode,
		std::vector<uint32_t> queue_families ):
	device( device ),
	size( size ),
	usage( usage ),
	memory_properties( memory_properties ),
	sharing_mode( sharing_mode ),
	queue_families( queue_families ){}

void KST_VK_Buffer::create( const KST_VK_BufferCreateInfo& cr_inf ){
	PROFILE_FUNCTION();

	//Create buffer
	vk::BufferCreateInfo buf_cr_inf(
			{},
			cr_inf.size,
			cr_inf.usage,
			cr_inf.sharing_mode,
			cr_inf.queue_families );

	buffer = cr_inf.device->device->createBufferUnique( buf_cr_inf );

	//Alloc memory
	auto memory_reqs = cr_inf.device->device->getBufferMemoryRequirements( *buffer );
	vk::MemoryAllocateInfo mem_inf(
			memory_reqs.size,
			cr_inf.device->find_memory_type(
				memory_reqs.memoryTypeBits,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent )
		);

	memory = cr_inf.device->device->allocateMemoryUnique( mem_inf );
	cr_inf.device->device->bindBufferMemory( *buffer, *memory, 0 );

	device = cr_inf.device;

	size = cr_inf.size;
}

void* KST_VK_Buffer::map(){
	PROFILE_FUNCTION();

	return data = device->device->mapMemory( *memory, 0, VK_WHOLE_SIZE );
}

void* KST_VK_Buffer::map( vk::DeviceSize offset, vk::DeviceSize size ){
	PROFILE_FUNCTION();

	KST_CORE_ASSERT( size == VK_WHOLE_SIZE || offset + size < this->size, "Error trying to map memory range {} - {} which is exceeding the buffer size {}", offset, offset + size, this->size );
	return data = device->device->mapMemory( *memory, offset, size );
}

void KST_VK_Buffer::unmap(){
	PROFILE_FUNCTION();

	device->device->unmapMemory( *memory );
	data = nullptr;
}

void VK_Utils::copy_to_buffer_device_local( const KST_VK_CopyInf& copy_inf, KST_VK_Buffer& target, vk::DeviceSize offset, void *data, size_t data_size ){
	PROFILE_FUNCTION();

	KST_VK_BufferCreateInfo buf_cr_inf(
		copy_inf.device,
		data_size,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible,
		vk::SharingMode::eExclusive );

	KST_VK_Buffer staging_buffer;
	staging_buffer.create( buf_cr_inf );

	staging_buffer.map( 0, VK_WHOLE_SIZE );

	memcpy( staging_buffer.data, data, data_size );

	std::array<vk::MappedMemoryRange, 1> mem_range{
		vk::MappedMemoryRange(
			*staging_buffer.memory,
			0,
			VK_WHOLE_SIZE )};

	copy_inf.device->device->flushMappedMemoryRanges( mem_range );
	staging_buffer.unmap();

	vk::CommandBufferBeginInfo beg_inf(
			vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
			nullptr );

	copy_inf.cmd_buf.begin( beg_inf );

	std::array<vk::BufferCopy, 1> copies{
		vk::BufferCopy( 0, offset, data_size )
	};

	copy_inf.cmd_buf.copyBuffer( *staging_buffer.buffer, *target.buffer, copies );

	copy_inf.cmd_buf.end();

	vk::FenceCreateInfo fence_cr_inf;

	vk::UniqueFence fence = copy_inf.device->device->createFenceUnique( fence_cr_inf );

	std::array<vk::CommandBuffer, 1> bufs{
		copy_inf.cmd_buf
	};

	std::array<vk::SubmitInfo, 1> sub_inf{ vk::SubmitInfo( {}, {}, bufs, {} )};

	copy_inf.queue.submit( sub_inf, *fence );

	KST_VERIFY( copy_inf.device->device->waitForFences( 1, &*fence, VK_TRUE, UINT64_MAX ) == vk::Result::eSuccess, "Wait for fence failed" );
}
