#pragma once

#include <kstpch.hpp>

namespace Kestrel {
	struct Terrain {
		int high_res_tiles{ 2 };
		int high_res_res{ 1024 };
		int med_res_tiles{ 12 };
		int med_res_res{ 64 };
		int low_res_tiles{ 32 };
		int low_res_res{ 16 };
	};
}
