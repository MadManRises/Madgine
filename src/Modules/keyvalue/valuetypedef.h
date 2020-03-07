VALUETYPE_TYPE(std::monostate, std::monostate, Null)
VALUETYPE_SEP
VALUETYPE_TYPE(std::string, HeapObject<std::string>, OwningString)
VALUETYPE_SEP
VALUETYPE_TYPE(std::string_view, std::string_view, NonOwningString)
VALUETYPE_SEP
VALUETYPE_TYPE(bool, bool, Bool)
VALUETYPE_SEP
VALUETYPE_TYPE(int, int, Int)
VALUETYPE_SEP
VALUETYPE_TYPE(size_t, size_t, UInt)
VALUETYPE_SEP
VALUETYPE_TYPE(float, float, Float)
VALUETYPE_SEP
VALUETYPE_TYPE(TypedScopePtr, TypedScopePtr, Scope)
VALUETYPE_SEP
VALUETYPE_TYPE(Matrix3, CoW<Matrix3>, Matrix3)
VALUETYPE_SEP
VALUETYPE_TYPE(Matrix4, CoW<Matrix4>, Matrix4)
VALUETYPE_SEP
VALUETYPE_TYPE(Quaternion, Quaternion, Quaternion)
VALUETYPE_SEP
VALUETYPE_TYPE(Vector4, Vector4, Vector4)
VALUETYPE_SEP
VALUETYPE_TYPE(Vector3, Vector3, Vector3)
VALUETYPE_SEP
VALUETYPE_TYPE(Vector2, Vector2, Vector2)
VALUETYPE_SEP
VALUETYPE_TYPE(KeyValueVirtualIterator, KeyValueVirtualIterator, KeyValueVirtualIterator)
VALUETYPE_SEP
VALUETYPE_TYPE(ApiMethod, ApiMethod, ApiMethod)
VALUETYPE_SEP
VALUETYPE_TYPE(ObjectPtr, ObjectPtr, Object)

#undef VALUETYPE_TYPE
#undef VALUETYPE_SEP