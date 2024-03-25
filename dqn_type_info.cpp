/*
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
//    dqn_type_info.cpp
//
////////////////////////////////////////////////////////////////////////////////////////////////////
*/

Dqn_TypeGetField Dqn_Type_GetField(Dqn_TypeInfo const *type_info, Dqn_Str8 name)
{
    Dqn_TypeGetField result = {};
    for (Dqn_usize index = 0; index < type_info->fields_count; index++) {
        Dqn_TypeField const *type_field = type_info->fields + index;
        if (type_field->name == name) {
            result.success = true;
            result.index   = index;
            result.field   = DQN_CAST(Dqn_TypeField *)type_field;
            break;
        }
    }
    return result;
}
