////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   $$$$$$$$\ $$\     $$\ $$$$$$$\  $$$$$$$$\       $$$$$$\ $$\   $$\ $$$$$$$$\  $$$$$$\
//   \__$$  __|\$$\   $$  |$$  __$$\ $$  _____|      \_$$  _|$$$\  $$ |$$  _____|$$  __$$\
//      $$ |    \$$\ $$  / $$ |  $$ |$$ |              $$ |  $$$$\ $$ |$$ |      $$ /  $$ |
//      $$ |     \$$$$  /  $$$$$$$  |$$$$$\            $$ |  $$ $$\$$ |$$$$$\    $$ |  $$ |
//      $$ |      \$$  /   $$  ____/ $$  __|           $$ |  $$ \$$$$ |$$  __|   $$ |  $$ |
//      $$ |       $$ |    $$ |      $$ |              $$ |  $$ |\$$$ |$$ |      $$ |  $$ |
//      $$ |       $$ |    $$ |      $$$$$$$$\       $$$$$$\ $$ | \$$ |$$ |       $$$$$$  |
//      \__|       \__|    \__|      \________|      \______|\__|  \__|\__|       \______/
//
//    dqn_type_info.h -- C++ type introspection
//
////////////////////////////////////////////////////////////////////////////////////////////////////

struct Dqn_TypeEnumField
{
    uint16_t  index;
    Dqn_Str8  name;
    Dqn_isize value;
};

struct Dqn_TypeStructField
{
    uint16_t                   index;
    Dqn_Str8                   name;
    struct Dqn_TypeInfo const *type;
    bool                       is_pointer;
    Dqn_TypeStructField const *array_count;
    uint16_t                   array_static_count;
};

enum Dqn_TypeInfoKind
{
    Dqn_TypeInfoKind_Basic,
    Dqn_TypeInfoKind_Enum,
    Dqn_TypeInfoKind_Struct,
};

struct Dqn_TypeInfo
{
    Dqn_Str8                   name;
    Dqn_TypeInfoKind           kind;
    Dqn_TypeStructField const *struct_field;
    uint16_t                   struct_field_count;
    Dqn_TypeEnumField const   *enum_field;
    uint16_t                   enum_field_count;
    Dqn_isize                  enum_min;
    Dqn_isize                  enum_max;
};
