#version 330 core

in vec2 tex_coords;
in vec3 varyingNormal;
in vec3 varyingLightDir;
in vec3 varyingVertPos;
out vec4 frag_col;
// texture sampler
uniform sampler2D tex;
// material mode
uniform int material;

// white light properties
vec4 globalAmbient = vec4(0.1f, 0.1f, 0.2f, 1.0f);
vec4 light_ambient = vec4(0.1f, 0.1f, 0.2f, 1.0f);
vec4 light_diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
vec4 light_specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);

void main()
{
	vec4 material_ambient, material_diffuse, material_specular;
    float material_shininess;

 
        // SILVER material - ambient, diffuse, specular, and shininess
    material_ambient = vec4( 0.1923f, 0.1923f, 0.1923f, 1.0f);
    material_diffuse = vec4(0.5075f, 0.5075f, 0.5075f, 1.0f);
    material_specular = vec4( 0.5083f, 0.5083f, 0.5083f, 1.0f);
    material_shininess = 51.2f;
 
    // normalize the light, normal, and view vectors:
    vec3 L = normalize(varyingLightDir);
    vec3 N = normalize(varyingNormal);
    vec3 V = normalize(-varyingVertPos);
    // compute light reflection vector with respect to N:
    vec3 R = normalize(reflect(-L, N));
    // get the angle between the light and surface normal:
    //float cosTheta = dot(L,N);
    // angle between the view vector and reflected light:
    //float cosPhi = dot(V,R);
    // compute ADS contributions (per pixel), and combine to build output color:
    vec3 ambient = (globalAmbient * material_ambient + light_ambient * material_ambient).xyz;
    vec3 diffuse = light_diffuse.xyz * material_diffuse.xyz * max(dot(N, L),0.0);
    vec3 specular = light_specular.xyz * material_specular.xyz* pow(max(dot(R, V), 0.0), material_shininess);
	vec4 textureColor = texture(tex, tex_coords);
    frag_col = vec4((ambient+diffuse+specular) * textureColor.rgb, textureColor.a);
    //frag_col = textureColor;
}
