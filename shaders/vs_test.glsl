#version 430
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNor;
layout(location = 2) in vec4 aCol;

out vec3 normal;
out vec4 colour;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 rotate;
uniform float height;
uniform float offset;

void main()
{
    normal = aNor.yzx;
    colour = vec4(aNor.x, aCol.x, 0.9f, 1.0);
    gl_Position = proj*view*rotate*vec4(aPos.x + offset, aPos.y * height, aPos.z, 1.0);
}