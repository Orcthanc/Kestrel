#pragma once

#include <kstpch.hpp>

#include <compare>

namespace Kestrel {
	struct TerrainInfo {
		int hi_tiles, hi_res, mid_tiles, mid_res, lo_tiles, lo_res, tilesize;
	};

	struct Terrain {
		public:
			Terrain();
			Terrain( size_t id );

			static Terrain createTerrain( const TerrainInfo& );

			auto operator<=>( const Terrain& t ) const noexcept {
				return id <=> t.id;
			}

			size_t id;
	};


	static const Terrain null_terrain{ 0 };
}
