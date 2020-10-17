#pragma once

#include <kstpch.hpp>

namespace Kestrel {
	struct MeshImpl {
		virtual ~MeshImpl() = default;
		virtual void load_obj( const char* path ) = 0;
		virtual void unload() = 0;
		virtual bool loaded() = 0;
	};

	struct Mesh {
		template<typename T>
		void load_obj( const char* path ){
			if( !impl )
				impl = std::make_unique<T>();
			impl->load_obj( path );
		}
		void unload();
		bool loaded();

		std::unique_ptr<MeshImpl> impl;
	};
}
