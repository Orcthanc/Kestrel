#version 450

layout( constant_id = 0 ) const bool logarithmic = false;

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec3 inColor;

layout( push_constant ) uniform push_constants {
	mat4 model;
} u_push_constants;

layout( binding = 0 ) uniform vp {
	mat4 view;
	mat4 projection;
	mat4 view_projection;
} u_vp;

layout( location = 0 ) out vec3 fragColor;

//const float Fcoef = 2.0 / log2( 500 + 1 );
const float C = 1;
const float Far = 10000;

void main() {
	gl_Position = u_vp.projection * u_vp.view * u_push_constants.model * vec4( position, 1.0 );
	if( logarithmic ){
		//gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * Fcoef - 1.0;
		gl_Position.z = log(C* gl_Position.w + 1) / log(C*Far + 1) * gl_Position.w;
	}
	fragColor = inColor;
}
