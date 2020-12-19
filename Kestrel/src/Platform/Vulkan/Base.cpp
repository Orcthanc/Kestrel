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

KST_VK_ImageCreateInfo::KST_VK_ImageCreateInfo(
			KST_VK_DeviceSurface* device,
			vk::Format format,
			vk::ImageSubresourceRange subresource_range,
			vk::ImageUsageFlags usage,
			vk::Extent3D extent,
			uint32_t miplevels,
			uint32_t array_layers,
			vk::ImageType image_type,
			vk::ImageTiling tiling,
			vk::Bool32 preinitialized ):
	device( device ),
	format( format ),
	subresource_range( subresource_range ),
	usage( usage ),
	extent( extent ),
	miplevels( miplevels ),
	array_layers( array_layers ),
	image_type( image_type ),
	tiling( tiling ),
	preinitialized( preinitialized ){}


void KST_VK_Image_DeviceLocal::create( const KST_VK_ImageCreateInfo & img_inf ){
	device = img_inf.device;

	{
		vk::ImageCreateInfo img_cr_inf(
				{},
				img_inf.image_type,
				img_inf.format,
				img_inf.extent,
				img_inf.miplevels,
				img_inf.array_layers,
				vk::SampleCountFlagBits::e1,
				img_inf.tiling,
				img_inf.usage,
				vk::SharingMode::eExclusive,
				{},
				img_inf.preinitialized ? vk::ImageLayout::ePreinitialized : vk::ImageLayout::eUndefined );

		image = device->device->createImageUnique( img_cr_inf );
	}
	{
		auto memreqs = device->device->getImageMemoryRequirements( *image );

		vk::MemoryAllocateInfo mem_inf(
				memreqs.size,
				device->find_memory_type(
					memreqs.memoryTypeBits,
					vk::MemoryPropertyFlagBits::eDeviceLocal )
			);

		memory = device->device->allocateMemoryUnique( mem_inf );
	}
	{
		vk::ImageViewCreateInfo view_cr_inf(
				{},
				*image,
				vk::ImageViewType::e1D,
				img_inf.format,
				vk::ComponentMapping(),
				img_inf.subresource_range );

		//TODO arrays
		if( img_inf.image_type == vk::ImageType::e1D ){
			view_cr_inf.viewType = vk::ImageViewType::e1D;
		} else if( img_inf.image_type == vk::ImageType::e2D ){
			view_cr_inf.viewType = vk::ImageViewType::e2D;
		} else {
			view_cr_inf.viewType = vk::ImageViewType::e3D;
		}

		view = device->device->createImageViewUnique( view_cr_inf );
	}

	//TODO sampler
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


void VK_Utils::changeImageLayout(
		vk::CommandBuffer buffer,
		vk::Image image,
		vk::ImageLayout oldLayout,
		vk::ImageLayout newLayout,
		vk::ImageSubresourceRange range,
		vk::PipelineStageFlags srcStages,
		vk::PipelineStageFlags dstStages )
{
	std::array<vk::ImageMemoryBarrier, 1> barrier{
		vk::ImageMemoryBarrier(
			{},
			{},
			oldLayout,
			newLayout,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED,
			image,
			range
		)};

	switch( oldLayout ){
		case vk::ImageLayout::eUndefined:
		{
			break;
		}
		case vk::ImageLayout::eColorAttachmentOptimal:
		{
			barrier[0].srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
			break;
		}
		case vk::ImageLayout::eDepthAttachmentOptimal:
		case vk::ImageLayout::eDepthStencilAttachmentOptimal:
		{
			barrier[0].srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
			break;
		}
		case vk::ImageLayout::ePresentSrcKHR:
		{
			barrier[0].srcAccessMask = vk::AccessFlagBits::eMemoryRead;
			break;
		}
		case vk::ImageLayout::eShaderReadOnlyOptimal:
		{
			barrier[0].srcAccessMask = vk::AccessFlagBits::eShaderRead;
			break;
		}
		case vk::ImageLayout::eTransferSrcOptimal:
		{
			barrier[0].srcAccessMask = vk::AccessFlagBits::eTransferRead;
			break;
		}
		case vk::ImageLayout::eTransferDstOptimal:
		{
			barrier[0].srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			break;
		}
		default:
		{
			KST_CORE_ERROR( "Invalid old layout {}", vk::to_string( oldLayout ));
		}
	}

	switch( newLayout ){
		case vk::ImageLayout::eUndefined:
		{
			break;
		}
		case vk::ImageLayout::eColorAttachmentOptimal:
		{
			barrier[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
			break;
		}
		case vk::ImageLayout::eDepthAttachmentOptimal:
		case vk::ImageLayout::eDepthStencilAttachmentOptimal:
		{
			barrier[0].dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
			break;
		}
		case vk::ImageLayout::ePresentSrcKHR:
		{
			barrier[0].dstAccessMask = vk::AccessFlagBits::eMemoryRead;
			break;
		}
		case vk::ImageLayout::eShaderReadOnlyOptimal:
		{
			barrier[0].dstAccessMask = vk::AccessFlagBits::eShaderRead;
			break;
		}
		case vk::ImageLayout::eTransferSrcOptimal:
		{
			barrier[0].dstAccessMask = vk::AccessFlagBits::eTransferRead;
			break;
		}
		case vk::ImageLayout::eTransferDstOptimal:
		{
			barrier[0].dstAccessMask = vk::AccessFlagBits::eTransferWrite;
			break;
		}
		default:
		{
			KST_CORE_ERROR( "Invalid new layout {}", vk::to_string( oldLayout ));
		}
	}

	buffer.pipelineBarrier(
			srcStages,
			dstStages,
			{},
			{},
			{},
			barrier );
}


