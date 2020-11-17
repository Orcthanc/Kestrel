#version 450

layout( triangles, equal_spacing, cw ) in;

layout( constant_id = 0 ) const bool logarithmic = false;

layout( location = 0 ) in VertData {
	vec3 position;
	vec3 color;
} indata[];

layout( push_constant ) uniform push_constants {
	mat4 model;
} u_push_constants;

layout( binding = 0 ) uniform vp {
	mat4 view;
	mat4 projection;
	mat4 view_projection;
} u_vp;

layout( location = 0 ) out vec3 color;

const float C = 1;
const float Far = 10000;

void main() {
	gl_Position =
			u_vp.projection * u_vp.view * u_push_constants.model *
			vec4(( indata[0].position * gl_TessCoord.x +
			indata[1].position * gl_TessCoord.y +
			indata[2].position * gl_TessCoord.z ), 1.0 );
	
	if( logarithmic ){
		//gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * Fcoef - 1.0;
		gl_Position.z = log(C* gl_Position.w + 1) / log(C*Far + 1) * gl_Position.w;
	}

	color =
			indata[0].color * gl_TessCoord.x +
			indata[1].color * gl_TessCoord.y +
			indata[2].color * gl_TessCoord.z;
}