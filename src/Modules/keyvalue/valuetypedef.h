VALUETYPE_TYPE(Null, std::monostate, std::monostate)
VALUETYPE_SEP
VALUETYPE_TYPE(String, CoWString, const CoWString&, std::string_view, std::string)
VALUETYPE_SEP
VALUETYPE_TYPE(Bool, bool, bool)
VALUETYPE_SEP
VALUETYPE_TYPE(Int, int, int)
VALUETYPE_SEP
VALUETYPE_TYPE(UInt, uint64_t, uint64_t, uint32_t)
VALUETYPE_SEP
VALUETYPE_TYPE(Float, float, float)
VALUETYPE_SEP
VALUETYPE_TYPE(Scope, TypedScopePtr, const TypedScopePtr&)
VALUETYPE_SEP
VALUETYPE_TYPE(Matrix3, CoW<Matrix3>, const Matrix3&)
VALUETYPE_SEP
VALUETYPE_TYPE(Matrix4, CoW<Matrix4>, const Matrix4&)
VALUETYPE_SEP
VALUETYPE_TYPE(Quaternion, Quaternion, const Quaternion&)
VALUETYPE_SEP
VALUETYPE_TYPE(Vector4, Vector4, const Vector4&)
VALUETYPE_SEP
VALUETYPE_TYPE(Vector3, Vector3, const Vector3&)
VALUETYPE_SEP
VALUETYPE_TYPE(Vector2, Vector2, const Vector2&)
VALUETYPE_SEP
VALUETYPE_TYPE(KeyValueVirtualIterator, KeyValueVirtualIterator, const KeyValueVirtualIterator&)
VALUETYPE_SEP
VALUETYPE_TYPE(ApiFunction, ApiFunction, const ApiFunction&)
VALUETYPE_SEP
VALUETYPE_TYPE(Object, ObjectPtr, const ObjectPtr&)

#undef VALUETYPE_TYPE
#undef VALUETYPE_SEP