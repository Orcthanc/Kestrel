#pragma once

#include <kstpch.hpp>
#include <filesystem>

namespace Kestrel {
	struct Mesh {
		Mesh(): id( null ){}
		Mesh( uint32_t id ): id( id ){}

		inline Mesh operator++(){
			++id;
			return *this;
		}

		inline Mesh operator--(){
			--id;
			return *this;
		}

		static Mesh createMesh( const std::filesystem::path& p );


		inline auto operator <=>( const Mesh& ) const = default;

		uint32_t id;

		static constexpr uint32_t null = 0;
	};
}

namespace std {
	template<>
	struct hash<Kestrel::Mesh> {
		size_t operator()( const Kestrel::Mesh& m ) const noexcept {
			return std::hash<uint32_t>{}( m.id );
		}
	};
}
