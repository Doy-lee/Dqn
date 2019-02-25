DQN_INSPECT enum struct OpenGLShader
{
    Invalid,
    Rect DQN_INSPECT_META(VertexShaderFilePath = "Rect.vert", FragmentShaderFilePath = "Rect.frag"),
    Text DQN_INSPECT_META(VertexShaderFilePath = "Text.vert", FragmentShaderFilePath = "Text.frag"),
    Count,
};

#if 0
#define EXAMPLE_MACRO \
    X(EndOfStream, "End Of Stream") \
    X(Hash, "#")

#define MAXIMUM_MACRO(a, b) (a > b) ? (a) : (b)
#endif

struct V3 { float test; };
struct V4 { float test; };

template <typename T, int Size>
struct Array
{
    T data[Size];
};

DQN_INSPECT struct OpenGLState
{
// #if 0
// #endif
    Array<V3, 32>   lights;
    Array<V4, 32>   camera_matrixes;
    char          **bitmaps;
    int             shaders[(int)OpenGLShader::Count];
    void           *win32_handle;
    int             ebo DQN_INSPECT_META(DisplayName = "Element Buffer Object"), vbo, vao DQN_INSPECT_META(DisplayName = "Vertex Array Object", OpenGLVersion = "330");
    V4              draw_color DQN_INSPECT_META(DisplayName = "HelloWorld");
    V3              lighting_ambient_coeff;
    int             draw_call_count;

    const int *const a;
    int const *const b, c, *d, *e;
    const int f;
    int const g;
    int *const h;
    int const* i;
    int *********const j, k, ******l, *m;
};

DQN_INSPECT_GENERATE_PROTOTYPE(texture = nullptr, size = {}, depth_test = false, type = 1, user_msg = "Hello world")
void RenderRect(V3 pos, V2 size, char const *texture, bool depth_test, int type, char *user_msg)
{
    (void)pos; void(size); (void)texture; (void)depth_test;
}
