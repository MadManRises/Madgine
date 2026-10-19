// Helpers for multi-channel signed distance field textures (fonts, icons)

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}
