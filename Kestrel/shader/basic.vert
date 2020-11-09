#version 450

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


void main() {
    gl_Position = u_vp.projection * u_vp.view * u_push_constants.model * vec4( position, 1.0 );
	fragColor = inColor;
}
