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

void main(void) {
    
    // Pixelate
    vec2 grid_uv = round(outTexCoord * float(40)) / float(40);
    frag_color = texture2D(screenTexture, grid_uv);
    
    // Black shader
	frag_color.xyz = mix(frag_color.xyz, vec3(0.0), blackFrac);
	frag_color.a = 1.0;
}

/////////// SHADER END //////////)"
