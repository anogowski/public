#version 430

in layout(location=0) vec3 position;
in layout(location=2) vec3 normal;
in layout(location=3) vec2 uv;
in layout(location=4) vec3 tangents;

uniform mat4 modelToWorld;
uniform mat4 worldToScreen;


out vec2 theUV;
out vec3 thePosition;
out smooth mat3 theVertexRotation;
void main()
{
	vec3 biTangent  = cross(normalize(tangents),normalize(normal));

	mat3 rotateM = mat3(tangents, biTangent, normal);

	mat4 pos = worldToScreen * modelToWorld;
	gl_Position =  pos  * vec4(position, 1.0f);
	
	theUV = uv;
	thePosition =vec3(modelToWorld * vec4(position,1));

	theVertexRotation = rotateM;
};


