DQN_INSPECT enum struct OpenGLShader
{
    Invalid,
    Rect DQN_INSPECT_META(VertexShaderFilePath = "Rect.vert", FragmentShaderFilePath = "Rect.frag"),
    Text DQN_INSPECT_META(VertexShaderFilePath = "Text.vert", FragmentShaderFilePath = "Text.frag"),
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


DQN_INSPECT struct OpenGLState
{
// #if 0
// #endif
    void                         *win32_handle;
    int                           ebo DQN_INSPECT_META(DisplayName = "Element Buffer Object"), vbo, vao DQN_INSPECT_META(DisplayName = "Vertex Array Object", OpenGLVersion = "330");
    // u32                           shaders[(int)OpenGLShader::Count];
    V4                            draw_color DQN_INSPECT_META(DisplayName = "HelloWorld");
    V3                            lighting_ambient_coeff;
    char                        **bitmaps;
    // FixedArray<RendererLight, 32> lights;
    // FixedArray<Mat4, 32>          camera_matrixes;
    int                           draw_call_count;

    const int *const a;
    int const *const b, c, *d, *e;
    const int f;
    int const g;
    int *const h;
    int const* i;
    int *********const j, k, ******l, *m;
};
