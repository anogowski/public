#ifndef  MY_RENDERER_H
#define MY_RENDERER_H 
#include <ExportHeader.h>
#include <GL\glew.h>
#include <QtOpenGL\qglwidget>
#include <Rendering\RendererTypeDefs.h>
#include <Rendering\Renderable.h>
#include <Rendering\Info\GeometryInfo.h>
#include <Rendering\Info\BufferInfo.h>
#include <Rendering\Info\ShaderInfo.h>
#include <Rendering\Info\StreamedParameterInfo.h>
#include <Rendering\Info\UniformInfo.h>
#include <Rendering\Info\VertexLayoutInfo.h>
#include <Rendering\Info\TextureInfo.h>
#include <Rendering\Info\PassInfo.h>
#include <NeumontTools\include\ShapeData.h>
#include <DebugTools\Shapes\Manager\DebugShapes.h>
#include <DebugTools\Shapes\ShapeMaker\ShapeMaker.h>
#include <Qt\qdebug.h>
#include <IO\FileIO.h>

class Renderer : public QGLWidget
{
	rend_uint numRenderables;
	rend_uint numStreamedParameterInfos;
	rend_uint numPassInfos;
	rend_uint numUniformInfos;
	rend_uint numTextureInfos;
	bool foundSlot;

	static const unsigned int MAX_GEOMETRIES = 100;
	GeometryInfo geometries[MAX_GEOMETRIES];

	static const unsigned int MAX_BUFFER_INFOS = 100;
	BufferInfo bufferInfos[MAX_BUFFER_INFOS];

	static const unsigned int MAX_SHADER_INFOS = 100;
	ShaderInfo shaderInfos[MAX_SHADER_INFOS];

	static const unsigned int MAX_RENDERABLES = 2500;
	Renderable renderables[MAX_RENDERABLES];

	static const unsigned int MAX_VERTEX_LAYOUT_INFOS = 100;
	VertexLayoutInfo vertexLayoutInfos[MAX_VERTEX_LAYOUT_INFOS];

	static const unsigned int MAX_STREAMED_PARAMETER_INFOS = 100;
	StreamedParameterInfo streamedParameterInfos[MAX_STREAMED_PARAMETER_INFOS];

	static const unsigned int MAX_TEXTURE_INFOS = 10;
	TextureInfo textureInfos[MAX_TEXTURE_INFOS];

	static const unsigned int MAX_PASS_INFOS = 100;
	PassInfo passInfos[MAX_PASS_INFOS];

	PassInfo* thePass;


	BufferInfo* allocateNewBuffer();
	BufferInfo* getAvailableBuffer(GLsizeiptr neededSize);
	BufferInfo* findBufferWithSpace(GLsizeiptr neededSize);
	BufferInfo* findUnallocatedBuffer();

	GeometryInfo* findAvailableGeometry();

	Renderable*  getAvailableRenderable();

	VertexLayoutInfo* getAvailableVertexLayoutInfo();

	TextureInfo* findAvialbleTextureInfo();


	void sendDataToBuffer(BufferInfo* buffer, void* data, GLsizeiptr neededSize);

	ShaderInfo*  getAvailableShaderInfo();

	void setupUniforms(Renderable* renderable);
	void setupConventionalUniforms(Renderable* renderable, PassInfo* passInfo);

	void doPass(PassInfo* passInfo);

	GLuint setupVertexArrayObject(VertexLayoutInfo* vertexLayoutInfo, GLuint vertexBufferID, GLuint vertexBufferDataOffset, GLuint indexBufferID);

	rend_uint compileShader(const char* code, GLenum shaderType);
	void checkCompileStatus(rend_uint shaderID);
	rend_uint linkProgram(rend_uint vertexShaderID, rend_uint fragmentShaderID);
	void checkLinkStatus(rend_uint programID);



	static Renderer* instance;
	Renderer(){}
	Renderer(const Renderer&){}
	Renderer& operator=(const Renderer&);
protected:
	void initializeGL();
	void paintGL();
public:
	ENGINE_SHARED	static bool initialize();
	ENGINE_SHARED    void setup();
	ENGINE_SHARED	static bool shutdown();

	ENGINE_SHARED	void draw();

	ENGINE_SHARED	GeometryInfo* addGeometry(void* verts, GLuint vertexDataSize, GLuint* indices, rend_uint numIndicies, GLuint  indexDataSize, GLuint indexingMode, VertexLayoutInfo* vertexLayoutInfo);
	ENGINE_SHARED	GeometryInfo* addGeometry(void* verts, GLuint vertexDataSize, GLushort* indices, rend_uint numIndicies, GLuint  indexDataSize, GLuint indexingMode, VertexLayoutInfo* vertexLayoutInfo);

	ENGINE_SHARED	ShaderInfo* addShader(const char* vertexShaderPath, const char* fragmentShaderPath);
	ENGINE_SHARED	Renderable* addRenderable(GeometryInfo* geometry, ShaderInfo* shader, mat4* position);
	ENGINE_SHARED  Renderable* addRenderable(GeometryInfo* geometry, ShaderInfo* shader, mat4* position, GLuint texID);

	ENGINE_SHARED	VertexLayoutInfo* addVertexLayoutInfo(rend_uint* sizes, rend_uint numAttributes, rend_uint stride);
	ENGINE_SHARED	StreamedParameterInfo* addStreamedParameterInfo(GLuint layoutLocation, ParameterType parameterType);
	ENGINE_SHARED	UniformInfo* addUniformInfo(UniformType uniformType, char* uniformName, void* data, Renderable* renderable);
	ENGINE_SHARED	TextureInfo* addTextureInfo(rend_uint slotID, char* location, ShaderInfo* shader);
	ENGINE_SHARED TextureInfo* convertToTextureInfo(rend_uint slotID, int width, int height, GLubyte* data);
	ENGINE_SHARED void updateTextureInfo(TextureInfo* ret, int width, int height, GLubyte* data);

	ENGINE_SHARED	PassInfo* addPassInfo();

	ENGINE_SHARED	static Renderer& getInstance() { return *instance; }

	ENGINE_SHARED Renderable* addShapeData(ShapeData data, ShaderInfo* shader, mat4* translate, TextureInfo* tex, rend_uint texID, rend_uint mode, vec3* color);
	ENGINE_SHARED Renderable* addShapeData(ShapeData data, ShaderInfo* shader, mat4* translate, TextureInfo* tex, TextureInfo* tex2, vec3* color);

	ENGINE_SHARED Renderable* addShapeInfo(ShapeInfo data, ShaderInfo* shader, mat4* translate, TextureInfo* tex, rend_uint texID, rend_uint mode, vec3* color);
	ENGINE_SHARED Renderable* addShapeInfo(ShapeInfo data, ShaderInfo* shaders, mat4* translate, TextureInfo* tex, TextureInfo* tex2, rend_uint mode, vec3* color);

	ENGINE_SHARED Renderable* addDebugMenuShape(ShapeData data, ShaderInfo* shader, mat4* translate, bool* vis);

	ENGINE_SHARED Renderable* setRenderable(GeometryInfo* geo, ShaderInfo* shader, mat4* transform, rend_uint mode, TextureInfo* tex, vec3* color);
	ENGINE_SHARED Renderable* setRenderable(GeometryInfo* geo, ShaderInfo* shader, mat4* transform, rend_uint mode, TextureInfo* tex, TextureInfo* tex2, vec3* color);

	ENGINE_SHARED Renderable* setUniforms(Renderable* renderable, rend_uint mode, mat4& translate, TextureInfo* texture, vec3* color);
	ENGINE_SHARED Renderable*  setUniforms2Tex(Renderable* renderable, rend_uint mode, mat4& translate, TextureInfo* texture, TextureInfo* texture2, vec3* color);


};

#define renderer Renderer::getInstance()

#endif