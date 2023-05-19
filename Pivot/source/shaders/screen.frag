R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D screenTexture;
uniform float blackFrac;
uniform float pixelFrac;
uniform vec2 screenSize;

vec2 maxResolution = screenSize;
vec2 minResolution = .007 * screenSize;

vec2 nonLinearMix(vec2 a, vec2 b, float frac) {
    float t = pow(2.71828, -10.0 * frac);
    return a * t + b * (1.0 - t);
}

void main(void) {
    
    // Pixelate
    if (pixelFrac > 0.0) {
        vec2 midTexCoord = outTexCoord * 2.0 - 1.0;
        vec2 resolution = nonLinearMix(maxResolution, minResolution, pixelFrac);
        vec2 grid_uv;
        //grid_uv.x = (round((midTexCoord.x * float(resolution.x)))) / float(resolution.x);
        //grid_uv.y = (round((midTexCoord.y * float(resolution.y)))) / float(resolution.y);
        grid_uv.x = (round((midTexCoord.x * float(resolution.x)) - 0.5) + 0.5) / float(resolution.x);
        grid_uv.y = (round((midTexCoord.y * float(resolution.y)) - 0.5) + 0.5) / float(resolution.y);
        grid_uv = (grid_uv + 1.0) / 2.0;
        frag_color = texture(screenTexture, grid_uv);
    } else {
        frag_color = texture(screenTexture, outTexCoord);
    }
    
    // Black shader
	frag_color.xyz = mix(frag_color.xyz, vec3(0.0), blackFrac);
	frag_color.a = 1.0;
}

/////////// SHADER END //////////)"
