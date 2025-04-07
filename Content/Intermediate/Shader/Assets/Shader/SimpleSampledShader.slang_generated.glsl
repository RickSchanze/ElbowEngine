Vertex: 
#version 460
layout(column_major) uniform;
layout(column_major) buffer;

#line 1312 0
struct _MatrixStorage_float4x4std140_0
{
    vec4  data_0[4];
};


#line 37 1
struct Camera_std140_0
{
    _MatrixStorage_float4x4std140_0 view_0;
    _MatrixStorage_float4x4std140_0 projection_0;
    _MatrixStorage_float4x4std140_0 ortho_0;
    _MatrixStorage_float4x4std140_0 data2_0;
};


#line 9 2
layout(binding = 0)
layout(std140) uniform block_Camera_std140_0
{
    _MatrixStorage_float4x4std140_0 view_0;
    _MatrixStorage_float4x4std140_0 projection_0;
    _MatrixStorage_float4x4std140_0 ortho_0;
    _MatrixStorage_float4x4std140_0 data2_0;
}camera_0;

#line 9
mat4x4 unpackStorage_0(_MatrixStorage_float4x4std140_0 _S1)
{

#line 9
    return mat4x4(_S1.data_0[0][0], _S1.data_0[0][1], _S1.data_0[0][2], _S1.data_0[0][3], _S1.data_0[1][0], _S1.data_0[1][1], _S1.data_0[1][2], _S1.data_0[1][3], _S1.data_0[2][0], _S1.data_0[2][1], _S1.data_0[2][2], _S1.data_0[2][3], _S1.data_0[3][0], _S1.data_0[3][1], _S1.data_0[3][2], _S1.data_0[3][3]);
}


#line 18 3
mat4x4 GetFloat4x4Identity_0()
{

#line 19
    return mat4x4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
}


#line 27
mat4x4 RotateX_0(float angle_0, mat4x4 m_0)
{

#line 28
    float cx_0 = cos(angle_0);
    float sx_0 = sin(angle_0);

#line 36
    return (((mat4x4(1.0, 0.0, 0.0, 0.0, 0.0, cx_0, - sx_0, 0.0, 0.0, sx_0, cx_0, 0.0, 0.0, 0.0, 0.0, 1.0)) * (m_0)));
}

mat4x4 RotateY_0(float angle_1, mat4x4 m_1)
{

#line 40
    float cy_0 = cos(angle_1);
    float sy_0 = sin(angle_1);

#line 48
    return (((mat4x4(cy_0, 0.0, sy_0, 0.0, 0.0, 1.0, 0.0, 0.0, - sy_0, 0.0, cy_0, 0.0, 0.0, 0.0, 0.0, 1.0)) * (m_1)));
}

mat4x4 RotateZ_0(float angle_2, mat4x4 m_2)
{

#line 52
    float cz_0 = cos(angle_2);
    float sz_0 = sin(angle_2);

#line 60
    return (((mat4x4(cz_0, - sz_0, 0.0, 0.0, sz_0, cz_0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0)) * (m_2)));
}

mat4x4 Translate_0(float x_0, float y_0, float z_0, mat4x4 m_3)
{

#line 70
    return (((mat4x4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, x_0, y_0, - z_0, 1.0)) * (m_3)));
}

mat4x4 Translate_1(vec3 translation_0, mat4x4 m_4)
{

#line 74
    return Translate_0(translation_0.x, translation_0.y, translation_0.z, m_4);
}


#line 5 1
struct Vertex_0
{
    vec3 position_0;
    vec3 normal_0;
    vec2 uv0_0;
    vec3 location_0;
    vec3 rotation_0;
    vec3 scale_0;
};

mat4x4 Vertex_GetModelFloat4x4_0(Vertex_0 this_0)
{
    mat4x4 rot_0 = GetFloat4x4Identity_0();

#line 23
    return ((((((Translate_1(this_0.location_0, rot_0)) * (RotateZ_0(this_0.rotation_0[2], RotateY_0(this_0.rotation_0[1], RotateX_0(this_0.rotation_0[0], rot_0))))))) * (mat4x4(this_0.scale_0.x, 0.0, 0.0, 0.0, 0.0, this_0.scale_0.y, 0.0, 0.0, 0.0, 0.0, this_0.scale_0.z, 0.0, 0.0, 0.0, 0.0, 1.0))));
}


#line 3 3
vec4 PerformMVP_0(vec4 pos_0, mat4x4 m_5, mat4x4 v_0, mat4x4 p_0)
{


    return (((p_0) * ((((v_0) * ((((m_5) * (pos_0)))))))));
}


#line 7
layout(location = 0)
out vec2 entryPointParam_vert_uv_0;


#line 7
layout(location = 0)
in vec3 vert_position_0;


#line 7
layout(location = 1)
in vec3 vert_normal_0;


#line 7
layout(location = 2)
in vec2 vert_uv0_0;


#line 7
layout(location = 3)
in vec3 vert_location_0;


#line 7
layout(location = 4)
in vec3 vert_rotation_0;


#line 7
layout(location = 5)
in vec3 vert_scale_0;


#line 14 2
struct VertexOutput_0
{
    vec4 position_1;
    vec2 uv_0;
};


#line 1314 0
mat4x4 Camera_GetView_0()
{

#line 39 1
    return unpackStorage_0(camera_0.view_0);
}


#line 39
mat4x4 Camera_GetProjection_0()
{
    return unpackStorage_0(camera_0.projection_0);
}


#line 24 2
void main()
{

#line 24
    Vertex_0 _S2 = { vert_position_0, vert_normal_0, vert_uv0_0, vert_location_0, vert_rotation_0, vert_scale_0 };
    VertexOutput_0 output_0;

    output_0.position_1 = PerformMVP_0(vec4(vert_position_0, 1.0), Vertex_GetModelFloat4x4_0(_S2), Camera_GetView_0(), Camera_GetProjection_0());
    output_0.uv_0 = vert_uv0_0;
    VertexOutput_0 _S3 = output_0;

#line 29
    gl_Position = output_0.position_1;

#line 29
    entryPointParam_vert_uv_0 = _S3.uv_0;

#line 29
    return;
}


Fragment: 
#version 460
layout(column_major) uniform;
layout(column_major) buffer;

#line 11 0
layout(binding = 1)
uniform texture2D tex_0;


#line 12
layout(binding = 2)
uniform sampler common_sampler_0;


#line 5371 1
layout(location = 0)
out vec4 entryPointParam_frag_color_0;


#line 5371
layout(location = 0)
in vec2 input_uv_0;


#line 19 0
struct FragOutput_0
{
    vec4 color_0;
};


#line 33
void main()
{

#line 34
    FragOutput_0 output_0;
    output_0.color_0 = (texture(sampler2D(tex_0,common_sampler_0), (input_uv_0)));

#line 35
    entryPointParam_frag_color_0 = output_0.color_0;

#line 35
    return;
}

