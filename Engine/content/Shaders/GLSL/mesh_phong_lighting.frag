/*
 * Razix Engine Shader File
 * Renders a mesh using phong shading using light maps (albedo, roughness, metallic, specular, normal etc.)
 */
 #version 450
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable

 //------------------------------------------------------------------------------
 // Vertex Input
layout(location = 0) in VSOutput
{
    vec3 fragPos;
    vec4 fragColor;
    vec2 fragTexCoord;
    vec3 fragNormal;
    vec3 fragTangent;
}fs_in;
 //------------------------------------------------------------------------------
 // Fragment Shader Stage Uniforms
layout(set = 1, binding = 0) uniform sampler2D albedoMap;
layout(set = 1, binding = 1) uniform sampler2D specularMap;
//------------------------------------------------------------------------------
// Uniforms and Push Constants
// Lighting information
layout(set = 1, binding = 2) uniform DirectionalLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
    vec3 viewPos;
} directional_light;
//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------

void main()
{
    // ambient
    vec3 ambient = directional_light.ambient * texture(albedoMap, fs_in.fragTexCoord).rgb;

    // diffuse
    vec3 norm = normalize(fs_in.fragNormal);
    vec3 lightDir = normalize(directional_light.position - fs_in.fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = directional_light.diffuse * diff * texture(albedoMap, fs_in.fragTexCoord).rgb;

    // specular
    vec3 viewDir = normalize(directional_light.viewPos - fs_in.fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
    float spec_map = 1.0f - texture(specularMap, fs_in.fragTexCoord).g;
    vec3 specular = vec3(spec_map, spec_map, spec_map) * spec;

    vec3 result = ambient + diffuse + specular;
    outFragColor = vec4(result, 1.0);
}
//------------------------------------------------------------------------------
