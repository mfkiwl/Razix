/*
 * Razix Engine Shader File
 * Fragment shader to render a skybox as a cubemap
 */
 #version 450
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable
 //------------------------------------------------------------------------------
 // Bindless Textures
//#define ENABLE_BINDLESS 1
//#include <Common/ShaderInclude.Builtin.BindlessResources.glsl>
 //------------------------------------------------------------------------------
 // Vertex Input
 layout(location = 0) in VSOutput
 {
     vec3 fragLocalPos;
     float time;
 }fs_in;
 //------------------------------------------------------------------------------
 // Fragment Shader Stage Uniforms
layout(set = 1, binding = 0) uniform samplerCube environmentMap;
//layout (push_constant) uniform EnvMap
//{
//    uint idx;
//}tex;

//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outSceneColor;
//------------------------------------------------------------------------------
// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
vec3 ACES(vec3 x) {
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
void main()
{
    //vec3 envColor = texture(global_textures_cubemap[nonuniformEXT(tex.idx)], fs_in.fragLocalPos).rgb;  
    vec3 envColor = texture(environmentMap, fs_in.fragLocalPos).rgb;  
    envColor = ACES(envColor);
    // gamma correction (already done in swapchain)
    //envColor = pow(envColor, vec3(1.0/2.2)); 
    
    outSceneColor = vec4(envColor, 1.0);
}
//------------------------------------------------------------------------------
