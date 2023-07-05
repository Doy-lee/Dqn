#if !defined(DQN_NO_DSMAP)
// NOTE: [$DMAP] Dqn_DSMap =========================================================================
DQN_API Dqn_DSMapKey Dqn_DSMap_KeyU64NoHash(uint64_t u64)
{
    Dqn_DSMapKey result = {};
    result.type         = Dqn_DSMapKeyType_U64NoHash;
    result.payload.u64  = u64;
    result.hash         = DQN_CAST(uint32_t)u64;
    return result;
}

DQN_API bool Dqn_DSMap_KeyEquals(Dqn_DSMapKey lhs, Dqn_DSMapKey rhs)
{
    bool result = false;
    if (lhs.type == rhs.type && lhs.hash == rhs.hash) {
        switch (lhs.type)  {
        case Dqn_DSMapKeyType_Invalid:   result = true; break;
        case Dqn_DSMapKeyType_U64NoHash: result = true; break;
        case Dqn_DSMapKeyType_U64:       result = lhs.payload.u64         == rhs.payload.u64; break;
        case Dqn_DSMapKeyType_Buffer:    result = lhs.payload.buffer.size == rhs.payload.buffer.size &&
                                         memcmp(lhs.payload.buffer.data, rhs.payload.buffer.data, lhs.payload.buffer.size) == 0; break;
        }
    }
    return result;
}

DQN_API bool operator==(Dqn_DSMapKey lhs, Dqn_DSMapKey rhs)
{
    bool result = Dqn_DSMap_KeyEquals(lhs, rhs);
    return result;
}
#endif // !defined(DQN_NO_DSMAP)
