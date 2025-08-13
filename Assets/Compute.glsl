#type compute
#version 430 core

layout(local_size_x = 256) in;

layout(std430, binding = 0) buffer InputBuffer
{
    float data[];
} inputBuffer;

layout(std430, binding = 1) buffer OutputBuffer
{
    float data[];
} outputBuffer;

uniform float u_Scale;
uniform float u_Offset;

void main()
{
    uint index = gl_GlobalInvocationID.x;
    float inputValue = inputBuffer.data[index];
    outputBuffer.data[index] = inputValue * u_Scale + u_Offset;
}
