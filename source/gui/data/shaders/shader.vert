#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 modelPose;
uniform mat4 cameraView;
uniform mat4 cameraProjection;

void main()
{
    FragPos = vec3(modelPose * vec4(aPos, 1.0));
    Normal = vec3(transpose(inverse(modelPose)) * vec4(aNormal, 0));
    TexCoords = aTexCoords;

    gl_Position = cameraProjection * cameraView * vec4(FragPos, 1.0);
}
