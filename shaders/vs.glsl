#version 330
in vec3 aPos;
in vec3 aNor;
in vec4 aCol;

out vec3 normal;
out vec4 colour;

// Uniform for all of them
uniform mat4 proj;
uniform mat4 view;
uniform mat4 rotate;

// Instance based
uniform vec3 offsets[100];
uniform float heights[100];

void main()
{
    vec3 offset = offsets[gl_InstanceID];
    float height = heights[gl_InstanceID];

    normal = aNor.yzx;
    colour = vec4(aCol.x + normal.x, aCol.y, aCol.z, aCol.w);
    gl_Position = proj*view*rotate*vec4(aPos.x + offset.x, aPos.y * height + offset.y, aPos.z + offset.z, 1.0);
}
