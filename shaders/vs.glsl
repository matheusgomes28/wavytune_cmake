#version 330
in vec3 aPos;
in vec3 aNor;
in vec4 aCol;

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
    colour = vec4(aCol.x + normal.x, aCol.y, aCol.z, aCol.w);
    gl_Position = proj*view*rotate*vec4(aPos.x + offset, aPos.y * height, aPos.z, 1.0);
}