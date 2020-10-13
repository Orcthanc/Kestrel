#include "Core/File.hpp"

#include <fstream>

using namespace Kestrel;

std::vector<uint8_t> Filereader::read_uint8t( const std::string& filename ){
	std::ifstream file( filename, std::ios::ate | std::ios::binary | std::ios::in );

	std::vector<uint8_t> res( file.tellg() );
	file.seekg( 0 );

	file.read( reinterpret_cast<char*>( res.data()), res.size() );

	return res;
}

std::vector<int8_t> Filereader::read_int8t( const std::string& filename ){
	std::ifstream file( filename, std::ios::ate | std::ios::binary | std::ios::in );

	std::vector<int8_t> res( file.tellg() );
	file.seekg( 0 );

	file.read( reinterpret_cast<char*>( res.data()), res.size() );

	return res;
}

std::string Filereader::read_string( const std::string& filepath ){
	auto temp = read_int8t( filepath );

	return { temp.begin(), temp.end() };
}
