#version 330 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 tex;
layout (location=2) in vec3 vertNormal;

out vec2 tex_coords;
out vec3 varyingNormal; // eye-space vertex normal
out vec3 varyingLightDir; // vector pointing to the light
out vec3 varyingVertPos; // vertex position in eye space;

// light position in view space
uniform vec3 light_position;

uniform mat4 norm_matrix; // model view matrix for transforming normals

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{	
	// output vertex position, light direction, and normal to the rasterizer for interpolation
    varyingVertPos= (view * model * vec4(vertPos,1.0)).xyz;
    varyingLightDir = light_position - varyingVertPos;
    varyingNormal= (norm_matrix * vec4(vertNormal,1.0)).xyz;
    
	// send the position to the fragment shader, as before
	gl_Position = projection * view *model* vec4(vertPos,1.0);
    
    tex_coords = tex;
}
