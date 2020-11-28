#include "Scene/SceneFileScene.hpp"

#include "Scene/Components.hpp"

#include "Platform/Vulkan/VKMesh.hpp" //TODO
#include "Platform/Vulkan/VKMaterial.hpp" //TODO

#include "SyntaxTree.h"
#include "Lexer.hpp"
#include "test.h"

#include "VM/VM.hpp"

#include "imgui.h"

#include <fstream>

using namespace Kestrel;

SceneFileScene::SceneFileScene( const std::filesystem::path& p ){
	PROFILE_FUNCTION();

	load( p );
}

void SceneFileScene::load( const std::filesystem::path& path ){
	PROFILE_FUNCTION();

	char* file;
	{
		std::ifstream istream( path, std::ios::ate | std::ios::in );
		KST_CORE_ASSERT( istream.is_open(), "Could not open file {}", path.string());
		auto size = istream.tellg();
		istream.seekg( 0 );
		file = (char*)malloc( size.operator long() + 1 );
		istream.read( file, size );
		file[size] = 0;
	}


	Lexer::Lexer lex;

	std::map<Lexer::token_id, int> convert_tok;

	convert_tok[0] = 0;
	convert_tok[lex.push_rule( "void", "void" )] = VOID;
	convert_tok[lex.push_rule( "Identifier", "\\w+\\d*" )] = IDENTIFIER;
	convert_tok[lex.push_rule( "Plus", "\\+" )] = PLUS;
	convert_tok[lex.push_rule( "Minus", "-" )] = MINUS;
	convert_tok[lex.push_rule( "Times", "\\*" )] = TIMES;
	convert_tok[lex.push_rule( "Divide", "/" )] = DIVIDE;
	convert_tok[lex.push_rule( "Modulo", "%" )] = MODULO;
	convert_tok[lex.push_rule( "Float", "\\d*\\.\\d+" )] = FLOAT;
	convert_tok[lex.push_rule( "Assignment", "=" )] = ASSIGN;
	convert_tok[lex.push_rule( "CurlyBracketOpen", "{" )] = CURLY_BRACKET_LEFT;
	convert_tok[lex.push_rule( "CurlyBracketClose", "}" )] = CURLY_BRACKET_RIGHT;
	convert_tok[lex.push_rule( "ParenthesisOpen", "(" )] = PARENTHESIS_LEFT;
	convert_tok[lex.push_rule( "ParenthesisClose", ")" )] = PARENTHESIS_RIGHT;
	convert_tok[lex.push_rule( "BracketOpen", "[" )] = LEFT_SQUARE_BRACKET;
	convert_tok[lex.push_rule( "BracketClose", "]" )] = RIGHT_SQUARE_BRACKET;
	convert_tok[lex.push_rule( "Comma", "," )] = COMMA;
	convert_tok[lex.push_rule( "Dot", "\\." )] = DOT;
	convert_tok[lex.push_rule( "Semicolon", ";" )] = SEMICOLON;
	convert_tok[lex.push_rule( "String", "\".*\"" )] = STRING;

	auto ws = lex.push_rule( "Whitespace", "\\s" );

	auto parser = ParseAlloc( malloc );

	lex.start( file );

	Lexer::Token t;
	ast_node* tree;
#ifndef NDEBUG
	FILE* f = fopen( "parser_debug.txt", "w" );
	char prefix[2] = { '>', ' ' };
	ParseTrace( f, prefix );
#endif
	std::unordered_set<std::string> strings; //TODO
	do {
		lex >> t;
		//std::cout << t << std::endl;
		if( t.id != ws ){
			if( convert_tok[ t.id ] == IDENTIFIER || convert_tok[ t.id ] == FLOAT || convert_tok[ t.id ] == STRING ){
				Parse( parser, convert_tok.at( t.id ), strings.emplace( std::move( t.match_text )).first->c_str(), &tree );
			} else
				Parse( parser, convert_tok.at( t.id ), t.match_text.c_str(), &tree );
		}
	} while( t.id != 0 );

#ifndef NDEBUG
	fflush( f );
	fclose( f );
#endif

	free( file );

	ast_node_print_tree( tree );

	ParseFree( parser, free );

	auto decls = tree->val.program.decls;
	if( decls ){
		auto temp = decls;

		while( temp ){
			std::string name = temp->val.list.val->val.decl.identifier->val.identifier.name;
			KST_CORE_INFO( "Entity: {}", name );
			Entity entity{ this, entitys.emplace( name, createEntity( name.c_str()).entity ).first->second };
			auto comps = temp->val.list.val->val.decl.components;
			auto temp2 = comps;

			while( temp2 ){
				std::string comp = temp2->val.list.val->val.comp.name->val.identifier.name;

				if( comp == "Transform" ){
					KST_CORE_INFO( "Adding TransformComponent" );
					components[entity.entity] |= SceneComponentTypes::eTransform;
						entity.addComponent<TransformComponent>( glm::vec3{}, glm::quat{ glm::vec3{ 0, 3.14, 0 }}, glm::vec3{} ); //TODO args
				} else if( comp == "Color" ){
					KST_CORE_ASSERT( temp2->val.list.val->val.comp.value->type == AST_NODE_float3, "FIXME:: Color component has to be a float3" );
					components[entity.entity] |= SceneComponentTypes::eColor;
					auto temp = temp2->val.list.val->val.comp.value->val.float3;
					entity.addComponent<ColorComponent>(glm::vec3( temp.x, temp.y, temp.z ));
					KST_CORE_INFO( "Adding color component: ({} {} {})", temp.x, temp.y, temp.z );
				} else if( comp == "Mesh" ){
					auto mesh = std::make_shared<Mesh>();
					KST_CORE_ASSERT( temp2->val.list.val->val.comp.value->type == AST_NODE_string, "FIXME:: Mesh component has to be a string" );
					components[entity.entity] |= SceneComponentTypes::eMesh;
					const char* path = temp2->val.list.val->val.comp.value->val.identifier.name;
					KST_CORE_INFO( "Adding mesh component: {}", path );
					mesh->load_obj<VK_Mesh>( path );
					entity.addComponent<MeshComponent>( mesh );
				} else if( comp == "Mat" ){
					KST_CORE_ASSERT( temp2->val.list.val->val.comp.value->type == AST_NODE_string, "FIXME:: Mat component has to be a string" );
					components[entity.entity] |= SceneComponentTypes::eMat;
					const char* path = temp2->val.list.val->val.comp.value->val.identifier.name;
					KST_CORE_INFO( "Adding material component: {}", path );
					auto mat = VK_Materials::getInstance().loadMaterial( path );
					entity.addComponent<MaterialComponent>( mat );
				} //TODO Camera

				temp2 = temp2->val.list.next;
			}

			temp = temp->val.list.next;
		}
	}

	functions = tree->val.program.functions;
	tree->val.program.functions = nullptr;

	ast_node_free( tree );
}

SceneFileScene::~SceneFileScene(){
	ast_node_free( functions );
}

void SceneFileScene::callFunctions(){
	PROFILE_FUNCTION();

	ast_node* func = functions;

	static size_t frame = 0;
	frame++;

	while( func ){
		//TODO
		auto name = func->val.list.val->val.function.name->val.identifier.name;
		auto entity = entitys.at( std::string( name ));
		Entity s_ent = { this, entity };

		KST_CORE_INFO( "Updating entity {}", name );

		VM vm;
		VariableRegistry reg;
		VariableHeap heap;

		auto& frame_var = reg.variable["frame"];
		frame_var.type.type = VM_Type::float1;
		frame_var.type.size = sizeof( float );
		frame_var.offset = heap.alloc_size( sizeof( float ));
		heap.accessVal<float>( frame_var ) = frame;

		if( any_flag( components[entity] & SceneComponentTypes::eTransform )){
			VariableRegistry& temp = *( reg.variable["Transform"].type.members = std::make_unique<VariableRegistry>());
			auto& loc = temp.variable["loc"];
			loc.type.type = VM_Type::float3;
			loc.type.size = sizeof( glm::vec3 );
			loc.offset = heap.alloc_size( sizeof( glm::vec3 ));
			heap.accessVal<glm::vec3>( loc ) = s_ent.getComponent<TransformComponent>().loc;
/*
			auto& rot = temp.variable["rot"];
			rot.type.type = VM_Type::float4;
			rot.type.size = sizeof( glm::vec4 );
			rot.offset = heap.alloc_size( sizeof( glm::vec4 ));
			auto rot_val = s_ent.getComponent<TransformComponent>().rot;
			heap.accessVal<glm::vec4>( rot ) = { rot_val.x, rot_val.y, rot_val.z, rot_val.w };
*/
			auto& scale = temp.variable["scale"];
			scale.type.type = VM_Type::float3;
			scale.type.size = sizeof( glm::vec3 );
			scale.offset = heap.alloc_size( sizeof( glm::vec3 ));
			heap.accessVal<glm::vec3>( scale ) = s_ent.getComponent<TransformComponent>().scale;
		} else {
			//TODO
		}

		vm.execute( func->val.list.val->val.function.statements, reg, heap );

		if( any_flag( components[entity] & SceneComponentTypes::eTransform )){
			VariableRegistry& temp = *reg.variable["Transform"].type.members;

			KST_CORE_INFO( "{} {} {}", heap.accessVal<glm::vec3>( temp.variable["loc"] ).x, heap.accessVal<glm::vec3>( temp.variable["loc"] ).y, heap.accessVal<glm::vec3>( temp.variable["loc"] ).z );

			s_ent.getComponent<TransformComponent>().loc = heap.accessVal<glm::vec3>( temp.variable["loc"] );
			KST_CORE_INFO( "{} {} {}", s_ent.getComponent<TransformComponent>().loc.x, s_ent.getComponent<TransformComponent>().loc.y, s_ent.getComponent<TransformComponent>().loc.z );

/*
			auto rot_val = heap.accessVal<glm::vec4>( temp.variable["rot"] );
			s_ent.getComponent<TransformComponent>().rot = { rot_val.x, rot_val.y, rot_val.z, rot_val.w };
*/
			s_ent.getComponent<TransformComponent>().scale = heap.accessVal<glm::vec3>( temp.variable["scale"] );
		} else {
			//TODO
		}


		func = func->val.list.next;
	}
}

void SceneFileScene::onUpdate(){
	PROFILE_FUNCTION();

	callFunctions();
#ifndef NDEBUG
	static bool firstFrame = true;
	if( firstFrame ){
		firstFrame = false;
		return;
	}

	auto cams = getView<NameComponent, CameraComponent>();

	ImGui::Begin( "Cameras" );
	for( auto& comps: cams ){
		auto [name, cam] = cams.get<NameComponent, CameraComponent>( comps );
		if( ImGui::CollapsingHeader( name.name.c_str() )){

			static bool inverse = false, logarithmic = false, interger = false, wireframe = false;
			ImGui::Checkbox( "Inverse depth buffer (unimplemented)", &inverse );
			ImGui::Checkbox( "Logarithmic depth buffer", &logarithmic );
			ImGui::Checkbox( "Integer depth buffer (unimplemented)", &interger );
			ImGui::Checkbox( "Wireframe", &wireframe );

			RenderModeFlags res{ RenderModeFlags::eNone };
			res |= inverse ? RenderModeFlags::eInverse : RenderModeFlags::eNone;
			res |= logarithmic ? RenderModeFlags::eLogarithmic : RenderModeFlags::eNone;
			res |= interger ? RenderModeFlags::eIntegerDepth : RenderModeFlags::eNone;
			res |= wireframe ? RenderModeFlags::eWireframe : RenderModeFlags::eNone;

			cam.camera->camera_render_mode = res;

			ImGui::Separator();
		}
	}

	ImGui::End();

	ImGui::ShowDemoWindow();
#endif
}
