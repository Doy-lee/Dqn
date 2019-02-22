DQN_REFLECT enum struct OpenGLShader
{
    Invalid,
    Rect DQN_REFLECT_META(VertexShaderFilePath = "Rect.vert", FragmentShaderFilePath = "Rect.frag"),
    Text DQN_REFLECT_META(VertexShaderFilePath = "Text.vert", FragmentShaderFilePath = "Text.frag"),
};

#if 0
#define EXAMPLE_MACRO \
    X(EndOfStream, "End Of Stream") \
    X(Hash, "#")

#define MAXIMUM_MACRO(a, b) (a > b) ? (a) : (b)
#endif

struct V3
{
    float test;
};

struct V4
{
    float test;
};


DQN_REFLECT struct OpenGLState
{
// #if 0
// #endif
    // u32                           ebo, vbo, vao;
    // u32                           shaders[(int)OpenGLShader::Count];
    V4                            draw_color DQN_REFLECT_META(FriendlyName = "HelloWorld");
    V3                            lighting_ambient_coeff;
    // u8                          **bitmaps;
    // FixedArray<RendererLight, 32> lights;
    // FixedArray<Mat4, 32>          camera_matrixes;
    int                           draw_call_count;
};
