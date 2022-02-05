/*
 * Razix Engine Shader File
 * Default Fragment Shader that can be used for rendering basic geometry with vertex colors and use a texture as well
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
     vec4 fragColor;
     vec2 fragTexCoord;
     vec3 fragNormal;
 }fs_in;

 //------------------------------------------------------------------------------
 // Fragment Shader Stage Uniforms
layout(set = 1, binding = 0) uniform sampler2D texSampler;
//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------

void main()
{
    outFragColor = vec4(fs_in.fragColor);
    //if(gl_FragCoord.x < 400)
    //    outFragColor = texture(texSampler, fs_in.fragTexCoord);
    //else
    //    outFragColor = texture(texSampler2, fs_in.fragTexCoord);

    outFragColor = texture(texSampler, fs_in.fragTexCoord);
    //outFragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
//------------------------------------------------------------------------------
