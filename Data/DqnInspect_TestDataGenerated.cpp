// This is an auto generated file using Dqn_Inspect

//
// ..\Data\DqnInspect_TestData.h
//

#ifndef DQN_INSPECT_DQNINSPECT_TESTDATA_H
#define DQN_INSPECT_DQNINSPECT_TESTDATA_H

char const *DqnInspect_EnumWithMetadata_Strings[] = {"Rect", "Count", };

char const *DqnInspect_EnumString(EnumWithMetadata val)
{
    if (val == EnumWithMetadata::Rect)  return DqnInspect_EnumWithMetadata_Strings[0]; // "Rect"
    if (val == EnumWithMetadata::Count) return DqnInspect_EnumWithMetadata_Strings[1]; // "Count"
    return nullptr;
}

char const *DqnInspect_FilePathMetadata(EnumWithMetadata val)
{
    if (val == EnumWithMetadata::Rect) return "Rect.vert";
    return nullptr;
}

char const *DqnInspect_FilePath2Metadata(EnumWithMetadata val)
{
    if (val == EnumWithMetadata::Rect) return "Rect.frag";
    return nullptr;
}

DqnInspect_StructMemberMetadata const DqnInspect_SampleStruct_ebo_StructMemberMetadata[] =
{
    {
        DqnInspect_StructMemberMetadataType::String,
        STR_AND_LEN("DisplayName"), STR_AND_LEN("Element Buffer Object"),
    },
};

DqnInspect_StructMemberMetadata const DqnInspect_SampleStruct_vao_StructMemberMetadata[] =
{
    {
        DqnInspect_StructMemberMetadataType::String,
        STR_AND_LEN("DisplayName"), STR_AND_LEN("Vertex Array Object"),
    },
    {
        DqnInspect_StructMemberMetadataType::String,
        STR_AND_LEN("OpenGLVersion"), STR_AND_LEN("330"),
    },
};

DqnInspect_StructMemberMetadata const DqnInspect_SampleStruct_draw_color_StructMemberMetadata[] =
{
    {
        DqnInspect_StructMemberMetadataType::String,
        STR_AND_LEN("DisplayName"), STR_AND_LEN("HelloWorld"),
    },
};

DqnInspect_StructMember const DqnInspect_SampleStruct_StructMembers[] =
{
    {
        STR_AND_LEN("Array"), STR_AND_LEN("lights"),
        STR_AND_LEN("V3, 32"), // template_expr
        nullptr, 0, // metadata and metadata_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("Array"), STR_AND_LEN("camera_matrixes"),
        STR_AND_LEN("V4, 32"), // template_expr
        nullptr, 0, // metadata and metadata_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("char"), STR_AND_LEN("bitmaps"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        2 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("shaders"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        1 // array_dimensions
    },
    {
        STR_AND_LEN("void"), STR_AND_LEN("win32_handle"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        1 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("ebo"),
        nullptr, 0, // template_expr and template_expr_len
        DqnInspect_SampleStruct_ebo_StructMemberMetadata, 1,
        0 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("vbo"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("vao"),
        nullptr, 0, // template_expr and template_expr_len
        DqnInspect_SampleStruct_vao_StructMemberMetadata, 2,
        0 // array_dimensions
    },
    {
        STR_AND_LEN("V4"), STR_AND_LEN("draw_color"),
        nullptr, 0, // template_expr and template_expr_len
        DqnInspect_SampleStruct_draw_color_StructMemberMetadata, 1,
        0 // array_dimensions
    },
    {
        STR_AND_LEN("V3"), STR_AND_LEN("lighting_ambient_coeff"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("draw_call_count"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("const"), STR_AND_LEN("int"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("const"), STR_AND_LEN("a"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("b"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        1 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("c"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("d"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        1 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("e"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        1 // array_dimensions
    },
    {
        STR_AND_LEN("const"), STR_AND_LEN("int"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("g"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("h"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        1 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("i"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        1 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("j"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        9 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("k"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("l"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        6 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("m"),
        nullptr, 0, // template_expr and template_expr_len
        nullptr, 0, // metadata and metadata_len
        1 // array_dimensions
    },
};

DqnInspect_Struct const DqnInspect_SampleStruct_Struct =
{
    STR_AND_LEN("SampleStruct"),
    DqnInspect_SampleStruct_StructMembers, // members
    ARRAY_COUNT(DqnInspect_SampleStruct_StructMembers) // members_len
};

DqnInspect_Struct const *DqnInspect_GetStruct(SampleStruct const *val)
{
    (void)val;
    DqnInspect_Struct const *result = &DqnInspect_SampleStruct_Struct;
    return result;
}

void Function1(int a, float b = {}, char const *c = nullptr, bool e = false, int f = 1, char *g = "Hello world");
void *Function2();
Array<int const *, 3> const *const Function3(Array<int, 32> const *foobar);

#endif // DQN_INSPECT_DQNINSPECT_TESTDATA_H

