#version 430

uniform sampler2D normalMap;
uniform sampler2D diffuseMap;
uniform sampler2D ambientMap;

uniform mat4 modelToWorld;
uniform vec3 objectColor;
uniform vec3 ambientLight;
uniform vec3 diffuseLightColor;
uniform vec3 eyePosition;
uniform vec3 specularLightColor;
uniform vec3 lightPosition;

uniform bool useNormal;
uniform bool useDiffuse;
uniform bool useAmbient;


in vec2 theUV;
in vec3 thePosition;
in mat3 theVertexRotation;

out vec4 daColor;

void main()
{
	
    vec3 normalColoring =  vec3(0,0,1);
	vec4 diffuseColoring = vec4(1,1,1,1);
	vec4 ambientColoring = vec4(1,1,1,1);
	if(useNormal)
	{
	    normalColoring = 2 * normalize(texture(normalMap,theUV).rgb) -1;
	}
	if (useDiffuse)
	{
		diffuseColoring = texture(diffuseMap,theUV);
	}
	if (useAmbient)
	{
		ambientColoring = texture(ambientMap,theUV);
	}
		vec3 lightNormalTemp =  theVertexRotation *  normalColoring;
		vec3 lightNormal =  normalize(vec3(modelToWorld * vec4(lightNormalTemp,0)));
	
    
	//Diffuse
	vec3 lightVector =  normalize(lightPosition - thePosition);
    float brightness = dot(lightVector, lightNormal);
    brightness  = clamp(brightness,0,1);
    
	//specular
	vec3 reflectedLight  = reflect(-lightVector, lightNormal);
    vec3 eyeVectorWorld = normalize(eyePosition -thePosition);
    float specularity = dot(reflectedLight, eyeVectorWorld);
    specularity = clamp(pow(specularity,100),0,1);
    
		//diffuseColoring = texture(diffuseMap,theUV);

	//Color
	vec4 light = vec4(ambientLight,1) + vec4((brightness* diffuseLightColor ),1);
		 
	 //(Diffuse base * light + spec) * ambient
	 daColor = (diffuseColoring * light + vec4((specularity * specularLightColor),1) ) * ambientColoring;
   
};