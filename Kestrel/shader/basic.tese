#version 450

layout( triangles, fractional_even_spacing, cw ) in;

layout( location = 0 ) in VertData {
	vec3 position;
	vec3 color;
} indata[];

layout( push_constant ) uniform push_constants {
	mat4 model;
	vec3 color;
} u_push_constants;

layout( binding = 0 ) uniform vp {
	mat4 view;
	mat4 projection;
	mat4 view_projection;
} u_vp;

// layout( location = 0 ) out vec3 color;
layout( location = 0 ) out fragData {
	vec3 color;
	float z;
} u_frag;

const float C = 1;
const float Far = 1000;

void main() {
	volatile vec4 modelpos = u_push_constants.model * vec4(( indata[0].position * gl_TessCoord.x +
			indata[1].position * gl_TessCoord.y +
			indata[2].position * gl_TessCoord.z ), 1.0 );

	modelpos = u_vp.view * modelpos;
	gl_Position = u_vp.projection * modelpos;

	//gl_Position = u_vp.view_projection * modelpos;

	//W-buffer
	//gl_Position.z = gl_Position.w / Far;
	//gl_Position.z = u_frag.z * gl_Position.w;

	//Inverse w
	//gl_Position.z = 0.1;

	u_frag.color = u_push_constants.color;
}
