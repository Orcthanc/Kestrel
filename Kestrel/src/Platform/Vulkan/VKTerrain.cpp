#include "Platform/Vulkan/VKTerrain.hpp"

using namespace Kestrel;

std::map<Terrain, VK_Terrain_T> VK_TerrainRegistry::terrains;

KST_VK_DeviceSurface* VK_TerrainRegistry::device;
vk::UniqueCommandPool VK_TerrainRegistry::pool;

void VK_TerrainRegistry::init( KST_VK_DeviceSurface* dev ){
	terrains.clear();
	device = std::move( dev );

	vk::CommandPoolCreateInfo pool_inf( vk::CommandPoolCreateFlagBits::eTransient, device->queue_families.transfer.value());

	pool = device->device->createCommandPoolUnique( pool_inf );
}

Terrain VK_TerrainRegistry::createTerrain( const TerrainInfo& t ){
	size_t buffer_size = std::pow(( t.hi_tiles + t.mid_tiles + t.lo_tiles ) * 2 + 1, 2 );

	KST_VK_Buffer buf;

	buf.create( KST_VK_BufferCreateInfo( device, buffer_size * sizeof( float ) * 3, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal ));

	std::vector<float> data( buffer_size * 3 );

	size_t index = 0;

	int himilo = t.hi_tiles + t.mid_tiles + t.lo_tiles;
	int himi = t.hi_tiles + t.mid_tiles;

	for( int y = -himilo; y <= himilo; ++y ){
		for( int x = -himilo; x <= himilo; ++x ){
			data[index++] = x;
			data[index++] = y;
			if( std::max( std::abs( x ), std::abs( y )) > himi )
				data[index++] = 2;
			else
				// 1 if larger, else 0
				data[index++] = std::max( std::abs( x ), std::abs( y )) > t.hi_tiles;
		}
	}


	vk::CommandBufferAllocateInfo alloc_inf( *pool, vk::CommandBufferLevel::ePrimary, 1 );
	auto buffer = device->device->allocateCommandBuffersUnique( alloc_inf );

	auto queue = device->device->getQueue( device->queue_families.transfer.value(), 0 );

	VK_Utils::copy_to_buffer_device_local( KST_VK_CopyInf{ device, queue, *buffer[0] }, buf, 0, data.data(), buffer_size * 12 );

	Terrain ret;

	terrains.emplace( ret, VK_Terrain_T{ t, (uint32_t)buffer_size, std::move( buf )});

	return ret;
}

void VK_TerrainRegistry::destroy(){
	pool = {};
	terrains.clear();
}
