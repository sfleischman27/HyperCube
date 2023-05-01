R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D redTexture;
uniform sampler2D greenTexture;
uniform sampler2D blueTexture;

float DecodeFloatRGBA(vec4 rgba) {
  return dot( rgba, vec4(1.0, 1.0/255.0, 1.0/65025.0, 1.0/16581375.0) );
}

void main(void) {
	frag_color.r = DecodeFloatRGBA(texture(redTexture, outTexCoord));
	frag_color.g = DecodeFloatRGBA(texture(greenTexture, outTexCoord));
	frag_color.b = DecodeFloatRGBA(texture(blueTexture, outTexCoord));
	frag_color.a = 1.0;
}

/////////// SHADER END //////////)"
