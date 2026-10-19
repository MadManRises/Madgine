// Octahedral encoding of unit vectors in two components ([-1, 1]^2), e.g. normals in a G-buffer

float2 octWrap(float2 v)
{
    return (1.0 - abs(v.yx)) * float2(v.x >= 0.0 ? 1.0 : -1.0, v.y >= 0.0 ? 1.0 : -1.0);
}

float2 encodeNormal(float3 n)
{
    n /= abs(n.x) + abs(n.y) + abs(n.z);
    n.xy = n.z >= 0.0 ? n.xy : octWrap(n.xy);
    return n.xy;
}

float3 decodeNormal(float2 e)
{
    float3 n = float3(e, 1.0 - abs(e.x) - abs(e.y));
    if (n.z < 0.0)
        n.xy = octWrap(n.xy);
    return normalize(n);
}
