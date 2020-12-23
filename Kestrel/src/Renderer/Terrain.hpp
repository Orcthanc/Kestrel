#pragma once

#include <kstpch.hpp>

namespace Kestrel {
	struct Terrain {
		int high_res_tiles{ 2 };
		int high_res_res{ 128 };
		int med_res_tiles{ 4 };
		int med_res_res{ 32 };
		int low_res_tiles{ 8 };
		int low_res_res{ 8 };

		int tile_size{ 256 };
	};
}
