R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

out vec4 frag_color;

uniform float farPlaneDist;
uniform sampler2D depthTexture;
uniform sampler2D replaceTexture;
uniform vec3 fadeCol;
uniform float severity; // should be 40.0, maybe 20.0, 2.0 on forest

// Editable parameter for depth falloff. Higher = fades out sooner
//const float severity = 40.0;

float DecodeFloatRGBA(vec4 rgba) {
  return dot( rgba, vec4(1.0, 1.0/255.0, 1.0/65025.0, 1.0/16581375.0) );
}

void main(void) {
    // Do not calculate if this area is cut
    if (texture(replaceTexture, outTexCoord).r == 0.0 || texture(depthTexture, outTexCoord).xyz == vec3(1.0)) {
        discard;
    }
	float d = DecodeFloatRGBA(texture(depthTexture, outTexCoord)) * severity;
    d = min(1.0, d);
	frag_color.xyz = fadeCol;
	frag_color.a = d;
}

/////////// SHADER END //////////)"
