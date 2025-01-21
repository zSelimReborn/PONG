#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform vec3 position;
uniform vec4 color;
uniform float scale = 5.0f;

void main()
{
    TexCoords = vertex.zw;
    ParticleColor = color;

    vec3 model = vec3(vertex.xy * scale, 0.0) + position;
    gl_Position = projection * vec4(model, 1.0);
}