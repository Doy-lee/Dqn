struct V3 {
  V3(float a, float b, float c) { (void)a; (void)b; (void)c; }
  float a, b, c;
};
struct V4 { float test; };

template <typename T, int Size>
struct Array { T data[Size]; };

enum struct OpenGLShader { Vertex, Count, };

DQN_INSPECT struct SampleStruct
{
    int             ebo                                 DQN_INSPECT_META(char const *DisplayName = "Element Buffer Object"), vbo, vao DQN_INSPECT_META(char const *DisplayName = "Vertex Array Object", int OpenGLVersion = 330);
    const int *const a;
    int const *const b, c, *d, *e;
    V4              draw_color                          DQN_INSPECT_META(char const *DisplayName = "HelloWorld");
// #if 0
// #endif
    Array<V3, 32>   lights;
    Array<V4, 32>   camera_matrixes;
    char          **bitmaps;
    int             shaders[(int)OpenGLShader::Count];
    void           *win32_handle;
    V3              lighting_ambient_coeff;
    int             draw_call_count;

    const int f;
    int const g;
    int *const h;
    int const* i;
    int ****const j, k, **l, *m;

};

DQN_INSPECT enum struct EnumWithMetadata
{
    Rect   DQN_INSPECT_META(char const *FilePath = "Rect.vert", V3 Coords = V3(1, 2, 3)),
    Square DQN_INSPECT_META(char const *FilePath = "Square.vert"),
    Count,
};

DQN_INSPECT_GENERATE_PROTOTYPE(b = {}, c = nullptr, e = false, f = 1, g = "Hello world")
void Function1(int a, float b, char const *c, bool e, int f, char *g)
{
    (void)a; (void)b; (void)c; (void)e; (void)f; (void)g;
}

DQN_INSPECT_GENERATE_PROTOTYPE(foo = V3(10, 20, 50), bar = {120, 150, 20})
void *Function2(V3 foo, V3 bar, ...) { (void)foo; (void)bar; return nullptr; }

DQN_INSPECT_GENERATE_PROTOTYPE()
Array<int const *, 3> const *const Function3(Array<int, 32> const *foobar) { (void)foobar; return {}; }
