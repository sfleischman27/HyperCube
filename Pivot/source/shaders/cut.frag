R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 pos;
in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D cutTexture;
uniform sampler2D outsideTexture;
uniform sampler2D dataTexture;
uniform vec2 transOffset;
uniform vec2 screenSize;

void main(void) {
	if (texture(dataTexture, outTexCoord).g == 1.0) {

        // TEMP: layer tex coords
		vec2 transTexCoord;
        
        float numTexX = 1.0;
        float numTexY = 1.0;
		vec2 midTexCoord = outTexCoord + transOffset;
        
        transTexCoord.x = mod(midTexCoord.x * numTexX, 1.0);
        transTexCoord.y = 1.0 - mod(midTexCoord.y * numTexY, 1.0);
		// End temp

		frag_color = texture(outsideTexture, transTexCoord);
		frag_color.a = 1;
	} else {
		discard;
	}
}

/////////// SHADER END //////////)"
