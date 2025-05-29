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

layout( location = 0 ) out vec3 color;
layout( location = 1 ) out float z_out;

const float C = 1;
const float Far = 1000000;

void main() {
	volatile vec4 modelpos = u_push_constants.model * vec4(( indata[0].position * gl_TessCoord.x +
			indata[1].position * gl_TessCoord.y +
			indata[2].position * gl_TessCoord.z ), 1.0 );

	float distance = sqrt( modelpos.x * modelpos.x + modelpos.z * modelpos.z );
	modelpos.y += sin(distance / 512) * 256;
	float col_fac = cos(distance / 512) * 0.4 + 0.6;

	//modelpos = u_vp.view * modelpos;
	//gl_Position = u_vp.projection * modelpos;

	gl_Position = u_vp.view_projection * modelpos;

	color =
			indata[0].color * gl_TessCoord.x +
			indata[1].color * gl_TessCoord.y +
			indata[2].color * gl_TessCoord.z;

	//color = u_push_constants.color;
	color.x *= max( 0, 1 - max( 0.0000001, distance / 16000 )) * col_fac;
	color.y *= max( 0, 1 - max( 0.0000001, ( distance - 8000 ) / 40000 )) * col_fac;
	color.z *= max( 0, 1 - max( 0.0000001, ( distance - 12000 ) / 60000 )) * col_fac;

	//color = vec3( distance, distance, distance );
}
