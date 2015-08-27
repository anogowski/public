#include <Rendering\Renderer.h>
#include <glm\gtc\matrix_transform.hpp>
Renderer* Renderer::instance = 0;

// Not passing down
// Atrribs wrong
// Not Binding

bool Renderer::initialize()
{
	if (instance != 0)
	{
		return false;
	}
	else
	{

		instance = new Renderer;
		instance->show();
		return true;
	}
}

void Renderer::setup()
{
	thePass = renderer.addPassInfo();
}



void Renderer::initializeGL()
{
	glewInit();
	numRenderables = 0;
	numStreamedParameterInfos = 0;
	numPassInfos = 0;
	numUniformInfos = 0;
	numTextureInfos = 0;
	glEnable(GL_DEPTH_TEST);

}

bool Renderer::shutdown()
{
	if (instance == 0)
	{
		return false;
	}
	else
	{
		delete instance;
		instance = 0;
		return true;
	}
}

BufferInfo* Renderer::allocateNewBuffer()
{
	BufferInfo* b = findUnallocatedBuffer();


	glGenBuffers(1, &b->glBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, b->glBufferID);
	glBufferData(GL_ARRAY_BUFFER, MAX_BUFFER_SIZE, 0, GL_STATIC_DRAW);
	b->hasBuffer = true;
	b->availableSize = MAX_BUFFER_SIZE;
	b->nextAvailableByte = 0;
	return b;
}

BufferInfo* Renderer::getAvailableBuffer(GLsizeiptr neededSize)
{

	BufferInfo* ret = findBufferWithSpace(neededSize);

	if (ret != 0)
	{
		return ret;
	}
	else
	{
		return allocateNewBuffer();
	}
}

BufferInfo* Renderer::findBufferWithSpace(GLsizeiptr neededSize)
{
	for (unsigned int i = 0; i < MAX_BUFFER_INFOS; i++)
	{
		if (bufferInfos[i].hasBuffer && bufferInfos[i].getingRemainingSize() > neededSize)
		{
			return bufferInfos + i;
		}
	}
	return 0;
}

BufferInfo* Renderer::findUnallocatedBuffer()
{

	for (unsigned int i = 0; i < MAX_BUFFER_INFOS; i++)
	{
		if (!bufferInfos[i].hasBuffer)
		{
			return bufferInfos + i;
		}
	}

	qDebug() << "Error: falied to find available buffer";
	exit(1);
	//return 0;
}

void Renderer::sendDataToBuffer(BufferInfo* buffer, void* data, GLsizeiptr neededSize)
{
	glBindBuffer(GL_ARRAY_BUFFER, buffer->glBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, buffer->nextAvailableByte, neededSize, data);
}

GeometryInfo* Renderer::findAvailableGeometry()
{
	for (unsigned int i = 0; i < MAX_GEOMETRIES; i++)
	{
		GeometryInfo& g = geometries[i];
		if (g.isAvailable)
		{
			g.isAvailable = false;
			return &g;
		}
	}

	qDebug() << "Error: Failed to add geometry. No empty slots";
	exit(1);

}

GeometryInfo* Renderer::addGeometry(void* verts, GLuint vertexDataSize, GLuint* indices, rend_uint numIndices, GLuint  indexDataSize, GLuint indexingMode, VertexLayoutInfo* vertexLayoutInfo)
{
	GeometryInfo* ret = findAvailableGeometry();

	BufferInfo* vertexBuffer = getAvailableBuffer(vertexDataSize);

	sendDataToBuffer(vertexBuffer, verts, vertexDataSize);
	GLuint vertexDataBufferByteOffset = vertexBuffer->nextAvailableByte;
	vertexBuffer->nextAvailableByte += vertexDataSize;


	BufferInfo* indexBuffer = getAvailableBuffer(indexDataSize);
	sendDataToBuffer(indexBuffer, indices, indexDataSize);
	ret->indexByteOffset = indexBuffer->nextAvailableByte;
	indexBuffer->nextAvailableByte += indexDataSize;

	ret->vertexArrayObjectID = setupVertexArrayObject(vertexLayoutInfo, vertexBuffer->glBufferID,
		vertexDataBufferByteOffset, indexBuffer->glBufferID);

	ret->vertexBufferID = vertexBuffer->glBufferID;
	ret->indexBufferID = indexBuffer->glBufferID;
	ret->vertexByteOffset = vertexDataBufferByteOffset;

	//TODO
	//may be able to remove the vertex buffer ID because the Vertex Array Object should hold that for you.
	ret->vertexLayoutInfo = vertexLayoutInfo;
	ret->indexingMode = indexingMode;
	ret->isAvailable = false;
	ret->numIndices = numIndices;
	ret->indiceDataType = GL_UNSIGNED_INT;
	return ret;
}

GeometryInfo* Renderer::addGeometry(void* verts, GLuint vertexDataSize, GLushort* indices, rend_uint numIndices, GLuint  indexDataSize, GLuint indexingMode, VertexLayoutInfo* vertexLayoutInfo)
{
	GeometryInfo* ret = findAvailableGeometry();

	BufferInfo* vertexBuffer = getAvailableBuffer(vertexDataSize);

	sendDataToBuffer(vertexBuffer, verts, vertexDataSize);
	GLuint vertexDataBufferByteOffset = vertexBuffer->nextAvailableByte;
	vertexBuffer->nextAvailableByte += vertexDataSize;


	BufferInfo* indexBuffer = getAvailableBuffer(indexDataSize);
	sendDataToBuffer(indexBuffer, indices, indexDataSize);
	ret->indexByteOffset = indexBuffer->nextAvailableByte;
	indexBuffer->nextAvailableByte += indexDataSize;

	ret->vertexArrayObjectID = setupVertexArrayObject(vertexLayoutInfo, vertexBuffer->glBufferID,
		vertexDataBufferByteOffset, indexBuffer->glBufferID);

	ret->vertexBufferID = vertexBuffer->glBufferID;
	ret->indexBufferID = indexBuffer->glBufferID;
	ret->vertexByteOffset = vertexDataBufferByteOffset;

	//TODO
	//may be able to remove the vertex buffer ID because the Vertex Array Object should hold that for you.
	ret->vertexLayoutInfo = vertexLayoutInfo;
	ret->indexingMode = indexingMode;
	ret->isAvailable = false;
	ret->numIndices = numIndices;
	ret->indiceDataType = GL_UNSIGNED_SHORT;
	return ret;
}

GLuint Renderer::setupVertexArrayObject(VertexLayoutInfo* vertexLayoutInfo, GLuint vertexBufferID, GLuint vertexBufferDataOffset, GLuint indexBufferID)
{
	GLuint ret;
	glGenVertexArrays(1, &ret);
	glBindVertexArray(ret);
	GLuint currentOffset = 0;
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);

	for (rend_uint i = 0; i < vertexLayoutInfo->numAttributes; i++)
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, vertexLayoutInfo->attributeSizes[i], GL_FLOAT, GL_FALSE, vertexLayoutInfo->stride, (void*)(vertexBufferDataOffset + currentOffset));
		currentOffset += vertexLayoutInfo->attributeSizes[i] * sizeof(float);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBindVertexArray(0);
	return ret;

}
Renderable* Renderer::getAvailableRenderable()
{
	static int currentSlot = 0;

	int i = currentSlot;
	if (i < MAX_RENDERABLES)
	{
		currentSlot++;
		return &renderables[i];
	}
	else
	{
		qDebug() << "Error: No more renderable slots";
		exit(1);
		//return(0);
	}

}

Renderable* Renderer::addRenderable(GeometryInfo* geometry, ShaderInfo* shader, mat4* position)
{
	Renderable* ret = getAvailableRenderable();
	ret->geometry = geometry;
	ret->shader = shader;
	ret->position = *position;
	return ret;
}


Renderable* Renderer::addRenderable(GeometryInfo* geometry, ShaderInfo* shader, mat4* position, GLuint texID)
{
	Renderable* ret = getAvailableRenderable();
	ret->geometry = geometry;
	ret->shader = shader;
	ret->position = *position;
	return ret;
}

ShaderInfo* Renderer::getAvailableShaderInfo()
{
	static int currentSlot = 0;

	int i = currentSlot;

	if (i < MAX_SHADER_INFOS)
	{
		currentSlot++;
		return &shaderInfos[i];
	}
	else
	{
		qDebug() << "No more shaderInfo slots";
		exit(1);
		//return(0);
	}

}

ShaderInfo* Renderer::addShader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	ShaderInfo* ret = getAvailableShaderInfo();

	rend_uint vertexShaderID = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
	rend_uint fragmentShaderID = compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

	checkCompileStatus(vertexShaderID);
	checkCompileStatus(fragmentShaderID);

	rend_uint programID = linkProgram(vertexShaderID, fragmentShaderID);

	ret->programID = programID;
	ret->isAvailable = false;
	return ret;
}

VertexLayoutInfo* Renderer::getAvailableVertexLayoutInfo()
{
	VertexLayoutInfo*  ret = 0;

	for (rend_uint i = 0; i < MAX_VERTEX_LAYOUT_INFOS; i++)
	{
		if (vertexLayoutInfos[i].isAvailable)
		{
			return vertexLayoutInfos + i;
		}

	}
	assert(false);
	return 0;
}

VertexLayoutInfo* Renderer::addVertexLayoutInfo(rend_uint* sizes, rend_uint numAttributes, rend_uint stride)
{
	assert(numAttributes <= VertexLayoutInfo::MAX_VERTEX_ATTRIBUTES);
	VertexLayoutInfo*  ret = getAvailableVertexLayoutInfo();
	memcpy(ret->attributeSizes, sizes, numAttributes * sizeof(sizes));
	ret->numAttributes = numAttributes;
	ret->stride = stride;
	ret->isAvailable = false;
	return ret;
}

StreamedParameterInfo* Renderer::addStreamedParameterInfo(GLuint layoutLocation, ParameterType parameterType)
{
	assert(numStreamedParameterInfos < MAX_STREAMED_PARAMETER_INFOS);

	StreamedParameterInfo*  ret = streamedParameterInfos + numStreamedParameterInfos++;
	ret->layoutLocation = layoutLocation;
	ret->parameterType = parameterType;
	return ret;
}

UniformInfo* Renderer::addUniformInfo(UniformType uniformType, char* uniformName, void* data, Renderable* renderable)
{

	assert(renderable->numUniforms < renderable->MAX_UNIFORM_INFOS);

	UniformInfo ret;
	ret.name = uniformName;
	ret.data = data;
	ret.uniformType = uniformType;
	renderable->uniformInfos[renderable->numUniforms] = ret;
	renderable->numUniforms++;
	return &ret;
}

void  Renderer::setupUniforms(Renderable* renderable)
{
	GLuint programID = renderable->shader->programID;
	float* fdata;

	for (rend_uint i = 0; i < (renderable->numUniforms); i++)
	{
		UniformType uniformType = renderable->uniformInfos[i].uniformType;
		void* data = renderable->uniformInfos[i].data;
		char* name = renderable->uniformInfos[i].name;
		GLuint location = glGetUniformLocation(renderable->shader->programID, name);


		if (uniformType == UniformType::UT_INT)
		{
			GLint idata = reinterpret_cast<GLint>(data);

			glUniform1i(location, idata);
		}
		else if (uniformType == UniformType::UT_FLOAT)
		{
			fdata = reinterpret_cast<float*>(data);
			glUniform1f(location, *fdata);
		}
		else if (uniformType == UniformType::UT_BOOL)
		{
			bool bdata = *reinterpret_cast<bool*>(data);
			glUniform1i(location, bdata);
		}
		else if (uniformType == UniformType::UT_VEC2)
		{
			fdata = reinterpret_cast<float*>(data);
			glm::vec2 v2(fdata[0], fdata[1]);
			glUniform2fv(location, 1, &v2[0]);
		}
		else if (uniformType == UniformType::UT_VEC3)
		{
			fdata = reinterpret_cast<float*>(data);
			glm::vec3 v3(fdata[0], fdata[1], fdata[2]);
			glUniform3fv(location, 1, &v3[0]);
		}
		else if (uniformType == UniformType::UT_VEC4)
		{
			fdata = reinterpret_cast<float*>(data);
			glm::vec4 v4(fdata[0], fdata[1], fdata[2], fdata[3]);

			glUniform4fv(location, 1, &v4[0]);
		}
		else if (uniformType == UniformType::UT_MAT3)
		{
			fdata = reinterpret_cast<float*>(data);
			glm::mat3 m3(fdata[0], fdata[1], fdata[2],
				fdata[3], fdata[4], fdata[5],
				fdata[6], fdata[7], fdata[8]);
			glUniformMatrix3fv(location, 1, false, &m3[0][0]);
		}
		else if (uniformType == UniformType::UT_MAT4)
		{
			fdata = reinterpret_cast<float*>(data);
			glm::mat4 m4(fdata[0], fdata[1], fdata[2], fdata[3],
				fdata[4], fdata[5], fdata[6], fdata[7],
				fdata[8], fdata[9], fdata[10], fdata[11],
				fdata[12], fdata[13], fdata[14], fdata[15]);
			glUniformMatrix3fv(location, 1, false, &m4[0][0]);
		}
		else if (uniformType == UniformType::UT_TEX)
		{
			GLuint slotID = reinterpret_cast<TextureInfo*>(data)->slotID;
			glUniform1i(location, slotID);
		}

	}

}

void  Renderer::setupConventionalUniforms(Renderable* renderable, PassInfo* passInfo)
{
	GLint worldToScreenLocation = glGetUniformLocation(renderable->shader->programID, "worldToScreen");
	GLint modelToWorldLocation = glGetUniformLocation(renderable->shader->programID, "modelToWorld");
	if (worldToScreenLocation != -1 && modelToWorldLocation != -1)
	{
		glm::mat4 worldToScreen = glm::perspective(45.0f, (float)(width() / height()), 0.1f, 100.0f) * passInfo->camera.getWorldToViewMatrix();
		glm::mat4 modelToWorld = renderable->position;
		glUniformMatrix4fv(worldToScreenLocation, 1, GL_FALSE, &worldToScreen[0][0]);
		glUniformMatrix4fv(modelToWorldLocation, 1, GL_FALSE, &modelToWorld[0][0]);
	}
}

TextureInfo* Renderer::findAvialbleTextureInfo()
{
	static int currentSlot = 0;
	int i = currentSlot;

	if (i < MAX_TEXTURE_INFOS)
	{
		currentSlot++;
		return &textureInfos[i];
	}
	else
	{
		qDebug() << "Error: Not more texture slots";
		exit(1);
		//return(0);
	}
}

TextureInfo* Renderer::addTextureInfo(rend_uint slotID, char* name, ShaderInfo* shader)
{

	TextureInfo* ret = findAvialbleTextureInfo();

	ret->slotID = slotID;

	QImage texture;

	if (!texture.load(name))
	{
		qDebug() << name << " falied to load";
		exit(1);
		return 0;
	}

	texture = convertToGLFormat(texture);

	glGenTextures(1, &ret->bufferID);

	glActiveTexture(GL_TEXTURE0 + slotID);

	glBindTexture(GL_TEXTURE_2D, ret->bufferID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width(), texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return ret;
}

TextureInfo* Renderer::convertToTextureInfo(rend_uint slotID, int width, int height, GLubyte* data)
{

	TextureInfo* ret = findAvialbleTextureInfo();

	ret->slotID = slotID;

	glGenTextures(1, &ret->bufferID);

	glActiveTexture(GL_TEXTURE0 + slotID);

	glBindTexture(GL_TEXTURE_2D, ret->bufferID);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//delete[] data;


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return ret;
}

void Renderer::updateTextureInfo(TextureInfo* ret, int width, int height, GLubyte* data)
{
	glActiveTexture(GL_TEXTURE0 + ret->slotID);

	glBindTexture(GL_TEXTURE_2D, ret->bufferID);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//delete[] data;
}


void Renderer::doPass(PassInfo* passInfo)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	for (rend_uint i = 0; i < passInfo->numRenderables; i++)
	{
		Renderable* r = renderables + i;
		if (!(r->visible))
		{
			continue;
		}

		GeometryInfo* g = r->geometry;

		glBindBuffer(GL_ARRAY_BUFFER, g->vertexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g->indexBufferID);

		glUseProgram(r->shader->programID);
		glBindVertexArray(g->vertexArrayObjectID);
		setupUniforms(r);
		setupConventionalUniforms(r, passInfo);

		glDrawElements(g->indexingMode, g->numIndices, g->indiceDataType, (void*)g->indexByteOffset);

	}
}

PassInfo* Renderer::addPassInfo()
{
	assert(numPassInfos < MAX_PASS_INFOS);
	return passInfos + numPassInfos++;
}

void Renderer::paintGL()
{
	glViewport(0, 0, width(), height());
	for (GLuint i = 0; i < numPassInfos; i++)
	{
		doPass(passInfos + i);
	}
}

void Renderer::draw()
{
	repaint();
}

rend_uint Renderer::compileShader(const char* code, GLenum shaderType)
{
	rend_uint ret = glCreateShader(shaderType);

	std::string temp(FileIO::fileToString(code));

	const char* adapter[1];
	adapter[0] = temp.c_str();
	glShaderSource(ret, 1, adapter, 0);
	glCompileShader(ret);
	return ret;
}

void Renderer::checkCompileStatus(rend_uint shaderID)
{
	GLint compileStatus;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_TRUE)
	{
		return;
	}
	else
	{
		GLint logLength;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
		GLint shaderType;
		glGetShaderiv(shaderID, GL_SHADER_TYPE, &shaderType);

		char* buffer = new char[logLength];
		GLsizei bitBucket;
		glGetShaderInfoLog(shaderID, logLength, &bitBucket, buffer);

		qDebug() << (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader complie error... " << buffer;

		delete[] buffer;
		exit(0);
	}

}

rend_uint Renderer::linkProgram(rend_uint vertexShaderID, rend_uint fragmentShaderID)
{
	GLuint programID = glCreateProgram();

	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);

	glLinkProgram(programID);

	//TODO
	//if you have time figure out how to check link status
	//checkLinkStatus(programID);

	return programID;

}

void Renderer::checkLinkStatus(rend_uint programID)
{
	GLint compileStatus;
	glGetProgramiv(programID, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_TRUE)
	{
		return;
	}
	else
	{
		GLint logLength;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);

		char* buffer = new char[logLength];
		GLsizei bitBucket;
		glGetProgramInfoLog(programID, logLength, &bitBucket, buffer);

		qDebug() << "program failed to link... " << buffer;

		delete[] buffer;
		exit(0);
	}
}

Renderable* Renderer::addShapeData(ShapeData data, ShaderInfo* shaders, mat4* translate, TextureInfo* tex, rend_uint texID, rend_uint mode, vec3* color)
{
	rend_uint sizes[] = { 3, 4, 3, 2 };
	VertexLayoutInfo* vertexLayoutInfo = renderer.addVertexLayoutInfo(sizes, ARRAYSIZE(sizes), Neumont::Vertex::STRIDE);

	GeometryInfo* geometry = renderer.addGeometry(data.verts, data.vertexBufferSize(),
		data.indices, data.numIndices, data.indexBufferSize(),
		GL_TRIANGLES, vertexLayoutInfo);

	return setRenderable(geometry, shaders, translate, mode, tex, color);
}

Renderable* Renderer::addShapeData(ShapeData data, ShaderInfo* shader, mat4* translate, TextureInfo* tex, TextureInfo* tex2, vec3* color)
{
	rend_uint sizes[] = { 3, 4, 3, 2 };
	VertexLayoutInfo* vertexLayoutInfo = renderer.addVertexLayoutInfo(sizes, ARRAYSIZE(sizes), Neumont::Vertex::STRIDE);
	GeometryInfo* geometry = renderer.addGeometry(data.verts, data.vertexBufferSize(),
		data.indices, data.numIndices, data.indexBufferSize(),
		GL_TRIANGLES, vertexLayoutInfo);

	return setRenderable(geometry, shader, translate, 3, tex, tex2, color);
}

Renderable* Renderer::addShapeInfo(ShapeInfo data, ShaderInfo* shaders, mat4* translate, TextureInfo* tex, rend_uint texID, rend_uint mode, vec3* color)
{
	rend_uint sizes[] = { 3, 4, 3, 2, 3 };
	VertexLayoutInfo* vertexLayoutInfo = renderer.addVertexLayoutInfo(sizes, ARRAYSIZE(sizes), VertexInfo::STRIDE);

	GeometryInfo* geometry = renderer.addGeometry(data.verts, data.vertexBufferSize(),
		data.indices, data.numIndices, data.indexBufferSize(),
		GL_TRIANGLES, vertexLayoutInfo);

	return setRenderable(geometry, shaders, translate, mode, tex, color);
}


Renderable* Renderer::addShapeInfo(ShapeInfo data, ShaderInfo* shaders, mat4* translate, TextureInfo* tex, TextureInfo* tex2, rend_uint mode, vec3* color)
{
	rend_uint sizes[] = { 3, 4, 3, 2, 3 };
	VertexLayoutInfo* vertexLayoutInfo = renderer.addVertexLayoutInfo(sizes, ARRAYSIZE(sizes), VertexInfo::STRIDE);

	GeometryInfo* geometry = renderer.addGeometry(data.verts, data.vertexBufferSize(),
		data.indices, data.numIndices, data.indexBufferSize(),
		GL_TRIANGLES, vertexLayoutInfo);

	return setRenderable(geometry, shaders, translate, mode, tex, tex2, color);
}

Renderable* Renderer::addDebugMenuShape(ShapeData data, ShaderInfo* shaders, mat4* translate, bool* visible)
{
	rend_uint sizes[] = { 3, 4, 3, 2 };

	VertexLayoutInfo* vertexLayoutInfo = renderer.addVertexLayoutInfo(sizes, ARRAYSIZE(sizes), Neumont::Vertex::STRIDE);

	GeometryInfo* geometry = renderer.addGeometry(data.verts, data.vertexBufferSize(),
		data.indices, data.numIndices, data.indexBufferSize(),
		GL_TRIANGLES, vertexLayoutInfo);


	Renderable* renderable = renderer.addRenderable(geometry, shaders, translate);

	UniformInfo* uni1 = renderer.addUniformInfo(UniformType::UT_VEC3, "objectColor", &vec3(0.75f, 0.75f, 0.75f), renderable);

	thePass->addRenderable(renderable);

	return renderable;
}

Renderable* Renderer::setRenderable(GeometryInfo* geo, ShaderInfo* shader, mat4* transform, rend_uint mode, TextureInfo* tex, vec3* color)
{
	Renderable* renderable = renderer.addRenderable(geo, shader, transform);
	renderable = setUniforms(renderable, mode, *transform, tex, color);
	thePass->addRenderable(renderable);
	return renderable;
}

Renderable* Renderer::setRenderable(GeometryInfo* geo, ShaderInfo* shader, mat4* transform, rend_uint mode, TextureInfo* tex, TextureInfo* tex2, vec3* color)
{
	Renderable* renderable = renderer.addRenderable(geo, shader, transform);
	renderable = setUniforms2Tex(renderable, mode, *transform, tex, tex2, color);
	thePass->addRenderable(renderable);
	return renderable;
}


Renderable*  Renderer::setUniforms(Renderable* renderable, rend_uint mode, mat4& translate, TextureInfo* texture, vec3* color)
{
	if (mode == 1)
	{
		UniformInfo* uni = renderer.addUniformInfo(UniformType::UT_VEC3, "objectColor", color, renderable);
	}
	else if (mode == 2)
	{
		UniformInfo* uni = renderer.addUniformInfo(UniformType::UT_TEX, "tex", texture, renderable);
	}
	return renderable;
}

Renderable*  Renderer::setUniforms2Tex(Renderable* renderable, rend_uint mode, mat4& translate, TextureInfo* texture, TextureInfo* texture2, vec3* color)
{
	UniformInfo* uni = renderer.addUniformInfo(UniformType::UT_TEX, "tex1", texture, renderable);
	UniformInfo* uni2 = renderer.addUniformInfo(UniformType::UT_TEX, "tex2", texture2, renderable);
	return renderable;
}