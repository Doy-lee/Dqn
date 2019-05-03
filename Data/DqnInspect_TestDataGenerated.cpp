// This is an auto generated file using DqnInspect

 // NOTE: These macros are undefined at the end of the file so to not pollute namespace
#define ARRAY_COUNT(array) sizeof(array)/sizeof((array)[0])
#define CHAR_COUNT(str) (ARRAY_COUNT(str) - 1)
#define STR_AND_LEN(str) str, CHAR_COUNT(str)

enum struct DqnInspectMemberType
{
    SampleStruct,
    SampleStruct_a,
    SampleStruct_b,
    SampleStruct_c,
    SampleStruct_d,
    SampleStruct_e,
    SampleStruct_f,
    SampleStruct_g,
    SampleStruct_h,
    SampleStruct_i,
    SampleStruct_j,
    SampleStruct_k,
    SampleStruct_l,
    SampleStruct_m,
    EnumWithMetadata,
    SampleStruct_ebo,
    SampleStruct_vao,
    SampleStruct_vbo,
    SampleStruct_lights,
    SampleStruct_bitmaps,
    SampleStruct_shaders,
    EnumWithMetadata_Rect,
    EnumWithMetadata_Count,
    EnumWithMetadata_Square,
    SampleStruct_draw_color,
    SampleStruct_win32_handle,
    SampleStruct_camera_matrixes,
    SampleStruct_draw_call_count,
    SampleStruct_lighting_ambient_coeff,
};

enum struct DqnInspectDeclType
{
    NotAvailable_,
    V3_,
    V4_,
    int_,
    Array_,
    int_Ptr_,
    V3,_32_,
    V4,_32_,
    int_Ptr_Ptr_,
    void_Ptr_,
    char_Ptr_Ptr_,
    int_const_,
    int_Ptr_const_,
    int_const_Ptr_,
    SampleStruct_,
    char_const_Ptr_,
    Array_V3,_32_,
    Array_V4,_32_,
    int_Ptr_Ptr_Ptr_Ptr_const_,
    EnumWithMetadata_,
    int_const_Ptr_const_,
};

enum struct DqnInspectMetaType
{
    DisplayName,
    OpenGLVersion,
};

//
// ..\Data\DqnInspect_TestData.h
//

#ifndef DQN_INSPECT_DQNINSPECT_TESTDATA_H
#define DQN_INSPECT_DQNINSPECT_TESTDATA_H

char const * DqnInspectMetadata_SampleStruct_ebo_DisplayName = "Element Buffer Object";
char const * DqnInspectMetadata_SampleStruct_vao_DisplayName = "Vertex Array Object";
int DqnInspectMetadata_SampleStruct_vao_OpenGLVersion = 330;
char const * DqnInspectMetadata_SampleStruct_draw_color_DisplayName = "HelloWorld";

DqnInspectMetadata const DqnInspectMetadata_SampleStruct_ebo[] =
{
    { DqnInspectDeclType::char_const_Ptr_, DqnInspectMetaType::DisplayName, &DqnInspectMetadata_SampleStruct_ebo_DisplayName},
};

DqnInspectMetadata const DqnInspectMetadata_SampleStruct_vao[] =
{
    { DqnInspectDeclType::char_const_Ptr_, DqnInspectMetaType::DisplayName, &DqnInspectMetadata_SampleStruct_vao_DisplayName},
    { DqnInspectDeclType::int_, DqnInspectMetaType::OpenGLVersion, &DqnInspectMetadata_SampleStruct_vao_OpenGLVersion},
};

DqnInspectMetadata const DqnInspectMetadata_SampleStruct_draw_color[] =
{
    { DqnInspectDeclType::char_const_Ptr_, DqnInspectMetaType::DisplayName, &DqnInspectMetadata_SampleStruct_draw_color_DisplayName},
};

DqnInspectMember const DqnInspect_SampleStruct_Members[] =
{
    {
        DqnInspectMemberType::SampleStruct_ebo, STR_AND_LEN("ebo"),
        offsetof(SampleStruct, ebo),
        DqnInspectDeclType::int_, STR_AND_LEN("int"),
        sizeof(((SampleStruct *)0)->ebo), // full_decl_type_sizeof
        DqnInspectDeclType::int_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        0, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        DqnInspectMetadata_SampleStruct_ebo, ARRAY_COUNT(DqnInspectMetadata_SampleStruct_ebo), // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_vbo, STR_AND_LEN("vbo"),
        offsetof(SampleStruct, vbo),
        DqnInspectDeclType::int_, STR_AND_LEN("int"),
        sizeof(((SampleStruct *)0)->vbo), // full_decl_type_sizeof
        DqnInspectDeclType::int_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        0, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_vao, STR_AND_LEN("vao"),
        offsetof(SampleStruct, vao),
        DqnInspectDeclType::int_, STR_AND_LEN("int"),
        sizeof(((SampleStruct *)0)->vao), // full_decl_type_sizeof
        DqnInspectDeclType::int_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        0, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        DqnInspectMetadata_SampleStruct_vao, ARRAY_COUNT(DqnInspectMetadata_SampleStruct_vao), // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_a, STR_AND_LEN("a"),
        offsetof(SampleStruct, a),
        DqnInspectDeclType::int_Ptr_const_, STR_AND_LEN("int *const"),
        sizeof(((SampleStruct *)0)->a), // full_decl_type_sizeof
        DqnInspectDeclType::int_Ptr_const_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        1, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_b, STR_AND_LEN("b"),
        offsetof(SampleStruct, b),
        DqnInspectDeclType::int_const_Ptr_const_, STR_AND_LEN("int const *const"),
        sizeof(((SampleStruct *)0)->b), // full_decl_type_sizeof
        DqnInspectDeclType::int_const_Ptr_const_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        1, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_c, STR_AND_LEN("c"),
        offsetof(SampleStruct, c),
        DqnInspectDeclType::int_, STR_AND_LEN("int"),
        sizeof(((SampleStruct *)0)->c), // full_decl_type_sizeof
        DqnInspectDeclType::int_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        0, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_d, STR_AND_LEN("d"),
        offsetof(SampleStruct, d),
        DqnInspectDeclType::int_Ptr_, STR_AND_LEN("int *"),
        sizeof(((SampleStruct *)0)->d), // full_decl_type_sizeof
        DqnInspectDeclType::int_Ptr_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        1, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_e, STR_AND_LEN("e"),
        offsetof(SampleStruct, e),
        DqnInspectDeclType::int_Ptr_, STR_AND_LEN("int *"),
        sizeof(((SampleStruct *)0)->e), // full_decl_type_sizeof
        DqnInspectDeclType::int_Ptr_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        1, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_draw_color, STR_AND_LEN("draw_color"),
        offsetof(SampleStruct, draw_color),
        DqnInspectDeclType::V4_, STR_AND_LEN("V4"),
        sizeof(((SampleStruct *)0)->draw_color), // full_decl_type_sizeof
        DqnInspectDeclType::V4_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        0, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        DqnInspectMetadata_SampleStruct_draw_color, ARRAY_COUNT(DqnInspectMetadata_SampleStruct_draw_color), // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_lights, STR_AND_LEN("lights"),
        offsetof(SampleStruct, lights),
        DqnInspectDeclType::Array_V3,_32_, STR_AND_LEN("Array<V3, 32>"),
        sizeof(((SampleStruct *)0)->lights), // full_decl_type_sizeof
        DqnInspectDeclType::Array_, DqnInspectDeclType::V3,_32_,
        STR_AND_LEN("V3, 32"), // template_child_expr
        0, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_camera_matrixes, STR_AND_LEN("camera_matrixes"),
        offsetof(SampleStruct, camera_matrixes),
        DqnInspectDeclType::Array_V4,_32_, STR_AND_LEN("Array<V4, 32>"),
        sizeof(((SampleStruct *)0)->camera_matrixes), // full_decl_type_sizeof
        DqnInspectDeclType::Array_, DqnInspectDeclType::V4,_32_,
        STR_AND_LEN("V4, 32"), // template_child_expr
        0, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_bitmaps, STR_AND_LEN("bitmaps"),
        offsetof(SampleStruct, bitmaps),
        DqnInspectDeclType::char_Ptr_Ptr_, STR_AND_LEN("char **"),
        sizeof(((SampleStruct *)0)->bitmaps), // full_decl_type_sizeof
        DqnInspectDeclType::char_Ptr_Ptr_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        2, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_shaders, STR_AND_LEN("shaders"),
        offsetof(SampleStruct, shaders),
        DqnInspectDeclType::int_, STR_AND_LEN("int"),
        sizeof(((SampleStruct *)0)->shaders), // full_decl_type_sizeof
        DqnInspectDeclType::int_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        1, // array_dimensions
        {ARRAY_COUNT(((SampleStruct *)0)->shaders), 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_win32_handle, STR_AND_LEN("win32_handle"),
        offsetof(SampleStruct, win32_handle),
        DqnInspectDeclType::void_Ptr_, STR_AND_LEN("void *"),
        sizeof(((SampleStruct *)0)->win32_handle), // full_decl_type_sizeof
        DqnInspectDeclType::void_Ptr_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        1, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_lighting_ambient_coeff, STR_AND_LEN("lighting_ambient_coeff"),
        offsetof(SampleStruct, lighting_ambient_coeff),
        DqnInspectDeclType::V3_, STR_AND_LEN("V3"),
        sizeof(((SampleStruct *)0)->lighting_ambient_coeff), // full_decl_type_sizeof
        DqnInspectDeclType::V3_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        0, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_draw_call_count, STR_AND_LEN("draw_call_count"),
        offsetof(SampleStruct, draw_call_count),
        DqnInspectDeclType::int_, STR_AND_LEN("int"),
        sizeof(((SampleStruct *)0)->draw_call_count), // full_decl_type_sizeof
        DqnInspectDeclType::int_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        0, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_f, STR_AND_LEN("f"),
        offsetof(SampleStruct, f),
        DqnInspectDeclType::int_, STR_AND_LEN("int"),
        sizeof(((SampleStruct *)0)->f), // full_decl_type_sizeof
        DqnInspectDeclType::int_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        0, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_g, STR_AND_LEN("g"),
        offsetof(SampleStruct, g),
        DqnInspectDeclType::int_const_, STR_AND_LEN("int const"),
        sizeof(((SampleStruct *)0)->g), // full_decl_type_sizeof
        DqnInspectDeclType::int_const_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        0, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_h, STR_AND_LEN("h"),
        offsetof(SampleStruct, h),
        DqnInspectDeclType::int_Ptr_const_, STR_AND_LEN("int *const"),
        sizeof(((SampleStruct *)0)->h), // full_decl_type_sizeof
        DqnInspectDeclType::int_Ptr_const_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        1, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_i, STR_AND_LEN("i"),
        offsetof(SampleStruct, i),
        DqnInspectDeclType::int_const_Ptr_, STR_AND_LEN("int const*"),
        sizeof(((SampleStruct *)0)->i), // full_decl_type_sizeof
        DqnInspectDeclType::int_const_Ptr_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        1, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_j, STR_AND_LEN("j"),
        offsetof(SampleStruct, j),
        DqnInspectDeclType::int_Ptr_Ptr_Ptr_Ptr_const_, STR_AND_LEN("int ****const"),
        sizeof(((SampleStruct *)0)->j), // full_decl_type_sizeof
        DqnInspectDeclType::int_Ptr_Ptr_Ptr_Ptr_const_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        4, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_k, STR_AND_LEN("k"),
        offsetof(SampleStruct, k),
        DqnInspectDeclType::int_, STR_AND_LEN("int"),
        sizeof(((SampleStruct *)0)->k), // full_decl_type_sizeof
        DqnInspectDeclType::int_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        0, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_l, STR_AND_LEN("l"),
        offsetof(SampleStruct, l),
        DqnInspectDeclType::int_Ptr_Ptr_, STR_AND_LEN("int **"),
        sizeof(((SampleStruct *)0)->l), // full_decl_type_sizeof
        DqnInspectDeclType::int_Ptr_Ptr_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        2, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_m, STR_AND_LEN("m"),
        offsetof(SampleStruct, m),
        DqnInspectDeclType::int_Ptr_, STR_AND_LEN("int *"),
        sizeof(((SampleStruct *)0)->m), // full_decl_type_sizeof
        DqnInspectDeclType::int_Ptr_, DqnInspectDeclType::NotAvailable_,
        nullptr, 0, // template_child_expr and template_child_expr_len
        1, // array_dimensions
        {0, 0, 0, 0, 0, 0, 0, 0}, // array_compile_time_size 0, max 8 dimensions, 0 if unknown,
        nullptr, 0, // metadata array
    },
};

DqnInspectStruct const DqnInspect_SampleStruct_Struct =
{
    STR_AND_LEN("SampleStruct"),
    DqnInspect_SampleStruct_Members, // members
    ARRAY_COUNT(DqnInspect_SampleStruct_Members) // members_len
};

DqnInspectStruct const *DqnInspect_Struct(SampleStruct const *)
{
    DqnInspectStruct const *result = &DqnInspect_SampleStruct_Struct;
    return result;
}

char const *DqnInspect_EnumWithMetadata_Strings[] = {"Rect", "Square", "Count", };

char const *DqnInspectEnum_Stringify(EnumWithMetadata val, int *len = nullptr)
{
    if (val == EnumWithMetadata::Rect)   { if (len) *len = CHAR_COUNT("Rect"); return DqnInspect_EnumWithMetadata_Strings[0]; }
    if (val == EnumWithMetadata::Square) { if (len) *len = CHAR_COUNT("Square"); return DqnInspect_EnumWithMetadata_Strings[1]; }
    if (val == EnumWithMetadata::Count)  { if (len) *len = CHAR_COUNT("Count"); return DqnInspect_EnumWithMetadata_Strings[2]; }
    return nullptr;
}

int DqnInspectEnum_Count(EnumWithMetadata) { return 3; }

char const * DqnInspectMetadata_FilePath(EnumWithMetadata val)
{
    if (val == EnumWithMetadata::Rect) { return "Rect.vert"; }
    if (val == EnumWithMetadata::Square) { return "Square.vert"; }
    return nullptr;
}

bool DqnInspectMetadata_Coords(EnumWithMetadata val, V3 *value = nullptr)
{
    if (val == EnumWithMetadata::Rect) { *value = V3(1, 2, 3); return true; }
    return false;
}

void                               Function1(int a, float b = {}, char const * c = nullptr, bool e = false, int f = 1, char * g = "Hello world");
void *                             Function2(V3 foo = V3(10, 20, 50), V3 bar = {120, 150, 20}, ...);
Array<int const *, 3> const *const Function3(Array<int, 32> const * foobar);

#endif // DQN_INSPECT_DQNINSPECT_TESTDATA_H

DqnInspectStruct const *DqnInspect_Struct(DqnInspectDeclType type)
{
    (void)type;
#ifdef DQN_INSPECT_DQNINSPECT_TESTDATA_H
    if (type == DqnInspectDeclType::SampleStruct_) return &DqnInspect_SampleStruct_Struct;
#endif // DQN_INSPECT_DQNINSPECT_TESTDATA_H

    return nullptr;
}


#undef ARRAY_COUNT
#undef CHAR_COUNT
#undef STR_AND_LEN
