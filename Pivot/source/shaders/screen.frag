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

float maxResolution = screenSize.y;
float minResolution = 4.0;

float nonLinearMix(float a, float b, float frac) {
    float t = pow(2.71828, -10.0 * frac);
    return a * t + b * (1.0 - t);
}

void main(void) {
    
    // Pixelate
    if (pixelFrac > 0.0) {
        float resolution = nonLinearMix(maxResolution, minResolution, pixelFrac);
        vec2 grid_uv = round(outTexCoord * float(resolution)) / float(resolution);
        frag_color = texture2D(screenTexture, grid_uv);
    } else {
        frag_color = texture2D(screenTexture, outTexCoord);
    }
    
    // Black shader
	frag_color.xyz = mix(frag_color.xyz, vec3(0.0), blackFrac);
	frag_color.a = 1.0;
}

/////////// SHADER END //////////)"
