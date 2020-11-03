#pragma once

#include <kstpch.hpp>

namespace Kestrel {

	template <typename T>
	struct BufferView {
		T* data;
		size_t size;
		size_t elem_size;
	};

	struct MeshImpl {
		virtual ~MeshImpl() = default;
		virtual void load_obj( const char* path ) = 0;
		virtual void unload() = 0;
		virtual bool loaded() = 0;


		virtual const BufferView<float> getVertices() = 0;
		virtual const BufferView<uint32_t> getIndices() = 0;
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

		const BufferView<float> getVertices();
		const BufferView<uint32_t> getIndices();

		std::unique_ptr<MeshImpl> impl;
	};
}
