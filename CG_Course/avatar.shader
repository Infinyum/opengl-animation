#shader vertex
#version 330 core

// input varyings
in vec3 vp;
in vec3 norm;
in vec2 texCoord;

// uniforms
uniform vec3 light;
uniform mat4 modelMatrix;
uniform mat4 inverseModelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

// normal, texture coordinates, light in model coordinates
out vec3 normalOut;
out vec2 texCoordOut;
out vec3 lightOut;

out vec3 viewOut;

// position in model coordinates
out vec3 positionOut;

void main() {
	// transforms light from world coordinates to model coordinates
	lightOut = (inverseModelMatrix * vec4(light, 0.0)).xyz;

	// copies normals and passes them to FS
	normalOut = norm;
	// copies texture coordinates and passes them to FS
	texCoordOut = texCoord;
	// copies view coordinates and passes them to FS
	viewOut = vec3(viewMatrix);

	// copies position and passes it to FS
	positionOut = vp;

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vp, 1.0);
}


#shader fragment
#version 330 core

// normal, texture coordinates, light in model coordinates
in vec3 normalOut;
in vec2 texCoordOut;
in vec3 lightOut;

in vec3 viewOut;

// position in model coordinates
in vec3 positionOut;

// uniforms
uniform sampler2D mire;

// out color
out vec4 frag_colour;

void main() {
	vec3 L = normalize(lightOut - viewOut);
	vec3 E = normalize(-viewOut); // we are in Eye Coordinates, so EyePos is (0,0,0)  
	vec3 R = normalize(-reflect(L, normalOut));

	//calculate Ambient Term:  
	vec4 Iamb = vec4(0.001);

	//calculate Diffuse Term:  
	vec4 Idiff = 0.01 * vec4(dot(normalize(normalOut), normalize(lightOut)),
		dot(normalize(normalOut), normalize(lightOut)),
		dot(normalize(normalOut), normalize(lightOut)),
		0.0f);
	Idiff = clamp(Idiff, 0.0, 1.0);

	// calculate Specular Term:
	vec4 Ispec = 0.5* vec4( pow(max(dot(R, E), 0.0), 5),
							pow(max(dot(R, E), 0.0), 5),
							pow(max(dot(R, E), 0.0), 5),
							0.0f);

	Ispec = clamp(Ispec, 0.0, 1.0);
	//Ispec = clamp(Ispec, 0.0, 0.5);

	// write Total Color:  
	//Textures + Phong Illumination shading + a Small Green Filter (as in the movie)
	gl_FragColor = texture(mire, texCoordOut) + Iamb + Idiff + Ispec + vec4(0.0,0.1,0.0,0.5); 
}
