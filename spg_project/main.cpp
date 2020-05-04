#include "main.h"

#define DEFAULT_WIDTH 1240
#define DEFAULT_HEIGHT 720

int WINDOW_WIDTH = DEFAULT_WIDTH;
int WINDOW_HEIGHT = DEFAULT_HEIGHT;

GLfloat manualMovementStep = 0.1;
GLfloat cube_height_scale_x = 40;
GLfloat cube_height_scale_y = 29;

glm::vec3 cam_pos_b;
glm::vec2 mouse_pos = glm::vec2(0, 0);
glm::vec3 camMovementVector = glm::vec3(0,0,0);
GLfloat mouse_sensitivity = 0.005f;
GLfloat cameraMoveTime = 50;
GLfloat cameraMoveTimer = 0;

GLfloat x_rot = 0, y_rot = 0, z_rot = 0;
GLfloat t_old = 0;
GLfloat camSpeed = 10;

float heightScale = 0.05;
float heightScaleStep = 0.01;
unsigned int primaryLayers = 10;
unsigned int secondaryLayers = 5;

CatmullRomCurve position_curve;
squadCurve rot_curve;

camera cam;
bool in_progress = false;
bool camlock = false;
bool continueApplication = true;

static const GLfloat white[] = {1.0, 1.0, 1.0, 1.0};

//materialSpecular and materialShininess can be used to change the initial settings of the light source
static const GLfloat materialSpecular[] = {1.0, 1.0, 1.0, 1.0};
static const GLfloat materialShininess = 20.0;



std::vector<float> points;
std::vector<float> particles_test_array;
float pointScale = 1;

float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	   // positions   // texCoords
	   -1.0f,  1.0f,  0.0f, 1.0f,
	   -1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

	   -1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
};

glm::mat4 proj = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 trans = glm::mat4(1.0f);

Shader* marchingCubesShader;
Shader* displacementShader;
unsigned int VAO, VBO, EBO;
unsigned int diffuseX, diffuseY, diffuseZ;
unsigned int displacementX, displacementY, displacementZ;
unsigned int normalY;

unsigned int FBO;
unsigned int FBOtexture;


unsigned int RBO;

Shader* FBOShader;
unsigned int VAO_FBO, VBO_FBO;

Shader* lampShader;
unsigned int lightVAO;

GLfloat base_cube_pos[] = {0, 3, 0};
GLfloat cam_pos[] = {0.0, 0, 0};
GLfloat distanceBetweenCubes = 2;
Model* ourModel;
float normalLevel = 0.3f;
GLfloat normalLevelStep = 0.1f;

std::vector<glm::vec3> lightPos{
	glm::vec3(-10.0f, 10.f, -10.f),
	glm::vec3(5.f, 10.f, -10.f)
};

unsigned long oldTimeSinceStart;
float deltaTime;
//Shadow Map
std::vector<unsigned int> depthMapFBO;
const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
std::vector<unsigned int> depthMap;
Shader* simpleDepthShader;

Shader *densityShader, *computeShader3DTexture;
GLuint densityTextureA, densityTextureB;
unsigned int textureWidth = 96;
unsigned int textureDepth = 96;
unsigned int textureHeight = 256;

GLuint mcTableTexture;
GLuint mcTableBuffer;

unsigned int quadVAO, quadVBO;
GLenum* DrawBuffers = new GLenum[2];
bool wireframeMode = false;

int cameraSector = 0;
int oldcameraSector = 0;
int reloadTopSectorBound = 150;
int reloadLowerSectorBound = -150;
int cameraSectorHeight = 255;
bool reload = false;


//Particle Shader
Shader* particleDisplayShader;
Shader* particleGenerationShader;
unsigned int particleVBO[2];
unsigned int particleTransformFeedbackBuffer[2];
unsigned int particleVAO;
const unsigned int maxParticles = 10000000;
const unsigned int emitterParticles = 10;

unsigned int randomTexture;

bool spawnParticles = false;
glm::vec3 spawnParticlePosition = glm::vec3(0,0,0);

//test Particle Input
std::vector<float> particles_test;
bool isFirstDraw = true;

int currentVB = 0;
int currentTFB = 1;

std::vector<glm::vec3>test_triangle;
std::vector<glm::vec3>test_triangle2;



struct particleStruct
{
	glm::vec3 position;
	glm::vec3 velocity;
	float lifeTime;
	float type;
};


void loadShaders() {
	reload = true;
	if(marchingCubesShader != nullptr)
	{
		delete marchingCubesShader;
		delete displacementShader;
		delete particleDisplayShader;
		delete particleGenerationShader;
		delete computeShader3DTexture;
	}
	//Basic Default Shadow Mapping
	marchingCubesShader = new Shader("shader/vshader.glsl", "shader/fshader.glsl", "shader/gshader.glsl");
	displacementShader = new Shader("shader/vdispShader.glsl", "shader/fdispShader.glsl");
	particleDisplayShader = new Shader("shader/vParticleShader.glsl", "shader/fParticleShader.glsl", "shader/gParticleShader.glsl");

	const GLchar* varyings[4];
	varyings[0] = "outPosition";
	varyings[1] = "outVelocity";
	varyings[2] = "outLifeTime";
	varyings[3] = "outType";
	
	particleGenerationShader = new Shader("shader/vParticleTransformShader.glsl", "shader/fParticleTransformShader.glsl", "shader/gParticleTransformShader.glsl", varyings, 4);
	computeShader3DTexture = new Shader("shader/cshader.glsl");
}

//initialization of the application.
void init()
{
	//add points for Camera path
	cam.setPosition(glm::vec3(0, 0, 0 ));
	//cam.setRotation(glm::quat(-0.709228, 0.033732, 0.042226, 0.702905));
	
	GLenum err = glewInit();
	glClearColor(0.0, 0.0, 0.0, 0.0);

	const int arraySize = (textureWidth - 1) * (textureDepth - 1);
	
	//initialize vertices for marching cubes geometry
	//points = new float[arraySize];
	
	
	for (int i = 0; i < textureWidth - 1; ++i)
	{
		for (int j = 0; j < textureDepth - 1; ++j)
		{
			int index = i * textureWidth + j;
			points.push_back(float(i) * pointScale);
			points.push_back(float(j) * pointScale);
		}		
	}

	
	GLint iMultiSample = 0;
	GLint iNumSamples = 0;
	//glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
	//glGetIntegerv(GL_SAMPLES, &iNumSamples);
	//printf("MSAA on, GL_SAMPLE_BUFFERS = %d, GL_SAMPLES = %d\n", iMultiSample, iNumSamples);


	//Basic Pass
	glEnable(GL_DEPTH_TEST);
	
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);
	
	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	//Particle Setup
	//RayCollisiontTriangles	
	test_triangle.push_back(glm::vec3(-10, 10, 0));
	test_triangle.push_back(glm::vec3(10, 10, 0));
	test_triangle.push_back(glm::vec3(-10, -10, 0));

	test_triangle2.push_back(glm::vec3(10, -10, 0));
	test_triangle2.push_back(glm::vec3(-10, 10, 0));
	test_triangle2.push_back(glm::vec3(10, 10, 0));
	
	auto* particles = new particleStruct[maxParticles];
	for (int i = 0; i < emitterParticles; i++)
	{
		//position
		particles[i].position = glm::vec3(i/10.f, 1.f, 0.f);

		//velocity
		particles[i].velocity = glm::vec3(0.f, 0.f, 0.f);

		//lifetime
		particles[i].lifeTime = float(i)/ 10.f;

		//type
		particles[i].type = 0;
	}

	

	glGenBuffers(2, particleVBO);
	glGenTransformFeedbacks(2, particleTransformFeedbackBuffer);
	for (unsigned int i = 0; i < 2; i++) {
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, particleTransformFeedbackBuffer[i]);
		glBindBuffer(GL_ARRAY_BUFFER, particleVBO[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particleStruct) * maxParticles, particles, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleVBO[i]);
	}

	delete[] particles;


	//Particle feedback
	
	loadShaders();

	//3D Texture for marching cubes (x2 for continous creation of the geometry
	glEnable(GL_TEXTURE_3D);
	glGenTextures(1, &densityTextureA);
	glBindTexture(GL_TEXTURE_3D, densityTextureA);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, textureWidth, textureDepth, textureHeight, 0, GL_RED, GL_UNSIGNED_SHORT, nullptr);
	glBindImageTexture(0, densityTextureA, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R16F);
	err = glGetError();

		
	glEnable(GL_TEXTURE_3D);
	glGenTextures(1, &densityTextureB);
	glBindTexture(GL_TEXTURE_3D, densityTextureB);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, textureWidth, textureDepth, textureHeight, 0, GL_RED, GL_UNSIGNED_SHORT, nullptr);
	glBindImageTexture(0, densityTextureB, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R16F);
	err = glGetError();

	
	//Create Table Buffer
	glGenBuffers(1, &mcTableBuffer);
	glBindBuffer(GL_TEXTURE_BUFFER, mcTableBuffer);
	glBufferData(GL_TEXTURE_BUFFER,	sizeof(triTable), triTable, GL_STATIC_DRAW);
	//glBufferStorage(GL_TEXTURE_BUFFER, 4096 * sizeof(GLuint), &triTable, GL_MAP_READ_BIT);
	//
	//Create Table Texture
	glGenTextures(1, &mcTableTexture);
	glBindTexture(GL_TEXTURE_BUFFER, mcTableTexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, mcTableBuffer);


#pragma region TEXTURES
	int width, height, nrChannels;

	unsigned char* data = imageLoader::loadImageData("textures/stone_wall_X_2K_Albedo.jpg", &width, &height, &nrChannels, 0);
	glGenTextures(1, &diffuseX);
	imageLoader::setDefault2DTextureFromData(diffuseX, width, height, data);
	imageLoader::freeImage((data));
	
	
	data = imageLoader::loadImageData("textures/rock_jagged_Y_2K_Albedo.jpg", &width, &height, &nrChannels, 0);
	glGenTextures(1, &diffuseY);
	imageLoader::setDefault2DTextureFromData(diffuseY, width, height, data);
	imageLoader::freeImage((data));

	
	data = imageLoader::loadImageData("textures/moss_rock_Z_2K_Albedo.jpg", &width, &height, &nrChannels, 0);
	glGenTextures(1, &diffuseZ);
	imageLoader::setDefault2DTextureFromData(diffuseZ, width, height, data);
	imageLoader::freeImage((data));

	data = imageLoader::loadImageData("textures/stone_wall_X_2K_Displacement.jpg", &width, &height, &nrChannels, 0);	
	glGenTextures(1, &displacementX);
	imageLoader::setDefault2DTextureFromData(displacementX, width, height, data);
	imageLoader::freeImage((data));


	data = imageLoader::loadImageData("textures/rock_jagged_Y_2K_Displacement.jpg", &width, &height, &nrChannels, 0);
	glGenTextures(1, &displacementY);
	imageLoader::setDefault2DTextureFromData(displacementY, width, height, data);
	imageLoader::freeImage((data));


	data = imageLoader::loadImageData("textures/moss_rock_Z_2K_Displacement.jpg", &width, &height, &nrChannels, 0);
	glGenTextures(1, &displacementZ);
	imageLoader::setDefault2DTextureFromData(displacementZ, width, height, data);
	imageLoader::freeImage((data));

	data = imageLoader::loadImageData("textures/rock_jagged_Y_2K_Normal.jpg", &width, &height, &nrChannels, 0);
	glGenTextures(1, &normalY);
	imageLoader::setDefault2DTextureFromData(normalY, width, height, data);
	imageLoader::freeImage((data));
#pragma endregion

	srand(int("unsinn"));
	
	glm::vec3* pRandomData = new glm::vec3[1000];
	for (unsigned int i = 0; i < 1000; i++) {
		pRandomData[i].x = (rand() % 10000) / 10000.f;
		pRandomData[i].y = (rand() % 10000) / 10000.f;
		pRandomData[i].z = (rand() % 10000) / 10000.f;
	}

	glGenTextures(1, &randomTexture);
	glBindTexture(GL_TEXTURE_1D, randomTexture);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 1000, 0, GL_RGB, GL_FLOAT, pRandomData);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	delete[] pRandomData;
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void reshape(int w, int h)
{
	WINDOW_WIDTH = w;
	WINDOW_HEIGHT = h;
	glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(60, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1, 50);
	proj = glm::perspective(glm::radians(60.0f), static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),0.1f, 600.f);
	
	glMatrixMode(GL_MODELVIEW);
}

void renderQuad()
{
	if (quadVAO == 0)
	{
		// positions
		glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
		glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
		glm::vec3 pos3(1.0f, -1.0f, 0.0f);
		glm::vec3 pos4(1.0f, 1.0f, 0.0f);
		// texture coordinates
		glm::vec2 uv1(0.0f, 1.0f);
		glm::vec2 uv2(0.0f, 0.0f);
		glm::vec2 uv3(1.0f, 0.0f);
		glm::vec2 uv4(1.0f, 1.0f);
		// normal vector
		glm::vec3 nm(0.0f, 0.0f, 1.0f);

		// calculate tangent/bitangent vectors of both triangles
		glm::vec3 tangent1, bitangent1;
		glm::vec3 tangent2, bitangent2;
		// triangle 1
		// ----------
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent1 = glm::normalize(tangent1);

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent1 = glm::normalize(bitangent1);

		// triangle 2
		// ----------
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent2 = glm::normalize(tangent2);


		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent2 = glm::normalize(bitangent2);


		float quadVertices[] = {
			// positions            // normal         // texcoords  // tangent                          // bitangent
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
		};
		// configure plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

bool RayIntersectsTriangle(glm::vec3 rayOrigin,	glm::vec3 rayVector, std::vector<glm::vec3> inTriangle,	glm::vec3& outIntersectionPoint)
{
	const float EPSILON = 0.0000001;
	glm::vec3 vertex0 = inTriangle[0];
	glm::vec3 vertex1 = inTriangle[1];
	glm::vec3 vertex2 = inTriangle[2];
	glm::vec3 edge1, edge2, h, s, q;
	float a, f, u, v;
	edge1 = vertex1 - vertex0;
	edge2 = vertex2 - vertex0;
	h = glm::cross(rayVector,edge2);
	a = glm::dot(edge1, h);
	if (a > -EPSILON && a < EPSILON)
		return false;    // This ray is parallel to this triangle.
	f = 1.0 / a;
	s = rayOrigin - vertex0;
	u = f * glm::dot(s,h);
	if (u < 0.0 || u > 1.0)
		return false;
	q = glm::cross(s, edge1);
	v = f * glm::dot(rayVector, q);
	if (v < 0.0 || u + v > 1.0)
		return false;
	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = f * glm::dot(edge2, q);
	if (t > EPSILON) // ray intersection
	{
		outIntersectionPoint = rayOrigin + rayVector * t;
		return true;
	}
	else // This means that there is a line intersection but not a ray intersection.
		return false;
}

void display()
{
	
	float near_plane = 0.1f, far_plane = 300;

	//camera position depended texture changes
	if (-cam.getPosition().z < (cameraSector * cameraSectorHeight) + reloadLowerSectorBound)
	{
		cameraSector--;
	}
	if (-cam.getPosition().z > (cameraSector * cameraSectorHeight) + reloadTopSectorBound)
	{
		cameraSector++;
	}
	
	//render 3D Texture
	if (reload || oldcameraSector != cameraSector)
	{
		reload = false;
		oldcameraSector = cameraSector;
		computeShader3DTexture->use();	

		printf("Camera Position %f %f %f \n", cam.getPosition().x, cam.getPosition().y, cam.getPosition().z);

		printf("Camera Sector %i \n", cameraSector);
		
		
		glActiveTexture(GL_TEXTURE0);


		computeShader3DTexture->setInt("cameraSector", cameraSector);
		glBindTexture(GL_TEXTURE_3D, densityTextureA);
		glBindImageTexture(0, densityTextureA, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R16F);

		glDispatchCompute(textureWidth, textureDepth, textureHeight);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


		computeShader3DTexture->setInt("cameraSector", cameraSector - 1);
		glBindTexture(GL_TEXTURE_3D, densityTextureB);
		glBindImageTexture(0, densityTextureB, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R16F);
		
		glDispatchCompute(textureWidth, textureDepth, textureHeight);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.5f, 0.9f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//time, ms are taken for sufficient for this exercise 
	unsigned long timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = (timeSinceStart - oldTimeSinceStart) / 1000.f;
	oldTimeSinceStart = timeSinceStart;

	if (cameraMoveTimer < cameraMoveTime && in_progress)
	{
		cameraMoveTimer += deltaTime;
	}

	//smooth Camera Movement

	if (camMovementVector != glm::vec3(0,0,0))
	{
		cam.addToLocalPosition(camSpeed * glm::normalize(camMovementVector) * (deltaTime));
	}
	
	//rotate the scene
	if (in_progress)
	{
		cam.setRotation(rot_curve.evaluate(cameraMoveTimer / cameraMoveTime));
		//catmull pos
		cam.setPosition(position_curve.evaluate(cameraMoveTimer / cameraMoveTime));
	}


	view = glm::mat4(1.0f);
	view = cam.getTransformationMatrix();
	
	if (wireframeMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	marchingCubesShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, densityTextureA);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, diffuseX);
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, diffuseY);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, diffuseZ);
	
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, displacementX);

	
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, displacementY);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, displacementZ);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, mcTableTexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I	, mcTableBuffer);
	
	marchingCubesShader->setVec3("densityTextureDimensions", textureWidth, textureDepth, textureHeight);
	marchingCubesShader->setInt("cameraSector", cameraSector);
	marchingCubesShader->setMat4("projection", proj);
	marchingCubesShader->setMat4("view", view);
	model = glm::mat4(1.0f);
	marchingCubesShader->setMat4("model", model);
	marchingCubesShader->setVec3("viewPos", cam.getPosition());
	glBindVertexArray(VAO);
	glDrawArraysInstanced(GL_POINTS, 0, (textureWidth - 1) * (textureDepth -1), textureHeight-1);


	marchingCubesShader->setInt("cameraSector", cameraSector - 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, densityTextureB);
	glDrawArraysInstanced(GL_POINTS, 0, (textureWidth - 1) * (textureDepth - 1), textureHeight-1);


	displacementShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseY);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, displacementY);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, normalY);
	displacementShader->setInt("diffuseMap", 0);
	displacementShader->setInt("displacementMap", 1);
	displacementShader->setInt("normalMap", 2);
	displacementShader->setInt("cameraSector", cameraSector);
	displacementShader->setFloat("heightScale", heightScale);
	displacementShader->setFloat("normalLevel", normalLevel);
	displacementShader->setInt("primaryLayers", primaryLayers);
	displacementShader->setInt("secondaryLayers", secondaryLayers);
	displacementShader->setMat4("projection", proj);
	displacementShader->setMat4("view", view);
	model = glm::translate(model, glm::vec3(0,0,-1));
	model = glm::scale(model, glm::vec3(10));
	displacementShader->setMat4("model", model);
	displacementShader->setVec3("viewPos", cam.getPosition());

	
	
	renderQuad();


	particleGenerationShader->use(); 
	particleGenerationShader->setFloat("deltaTime", deltaTime);
	particleGenerationShader->setFloat("programTime", float(oldTimeSinceStart));
	particleGenerationShader->setBool("spawnNewEmitter", spawnParticles);
	particleGenerationShader->setFloat("spawnUpdateFactor", heightScale);


	if (spawnParticles)
	{
		float relativeMouseX = (mouse_pos.x / (WINDOW_WIDTH * 0.5f) - 1.0f);
		float relativeMouseY = (mouse_pos.y / (WINDOW_HEIGHT * 0.5f) - 1.0f);

		glm::mat4 invVP = glm::inverse(proj * view);
		glm::vec4 screenMousePos = glm::vec4(relativeMouseX, relativeMouseY, 1.0f, 1.0f);
		glm::vec4 worldMousePos = invVP * screenMousePos;

		glm::vec3 rayDir = -glm::normalize(glm::vec3(worldMousePos.x, worldMousePos.y, worldMousePos.z));
		glm::vec3 result;
		//ray intersection
		if(RayIntersectsTriangle(cam.getPosition(),rayDir,test_triangle, result))
		{
			spawnParticlePosition = -result / 10.f;

		}
		else if(RayIntersectsTriangle(cam.getPosition(), rayDir, test_triangle2, result))
		{
			spawnParticlePosition = -result / 10.f;
		}
		else
		{
			particleGenerationShader->setBool("spawnNewEmitter", false);
		}
		spawnParticles = false;
	}

	particleGenerationShader->setVec3("spawnPosition", spawnParticlePosition);
	
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_1D, randomTexture);
	
	glEnable(GL_RASTERIZER_DISCARD);
	
	glBindBuffer(GL_ARRAY_BUFFER, particleVBO[currentVB]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, particleTransformFeedbackBuffer[currentTFB]);

	//updated with new particles
	
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(particleStruct), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(particleStruct), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(particleStruct), (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(particleStruct), (void*)(sizeof(float) * 7));
	
	glBeginTransformFeedback(GL_POINTS);

	
	if (isFirstDraw) {
		glDrawArrays(GL_POINTS, 0, emitterParticles);

		isFirstDraw = false;
	}
	else {
		glDrawTransformFeedback(GL_POINTS, particleTransformFeedbackBuffer[currentVB]);
	}
		
	glEndTransformFeedback();
		
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	

	glDisable(GL_RASTERIZER_DISCARD);

	
	particleDisplayShader->use();
	
	particleDisplayShader->setMat4("projection", proj);
	particleDisplayShader->setMat4("view", view);
	particleDisplayShader->setMat4("model", model);

	glBindBuffer(GL_ARRAY_BUFFER, particleVBO[currentTFB]);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(particleStruct), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(particleStruct), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(particleStruct), (void*)(sizeof(float) * 7));
	glDrawTransformFeedback(GL_POINTS, particleTransformFeedbackBuffer[currentTFB]);
	//glDrawArrays(GL_POINTS, 0, 6);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(3);	
	currentTFB = currentVB;
	currentVB = (currentTFB + 1) & 1;
	glutSwapBuffers();
}

void CalcLightSpaceMatrix(float near_plane, float far_plane, glm::vec3 pos, glm::mat4& lightSpaceMatrix)
{
	glm::mat4 lightProjection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, near_plane, far_plane);
	glm::mat4 lightView = lookAt(pos,
	                             glm::vec3(0.0f, 0.0f, 0.0f),
	                             glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;
}

void idle()
{
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_UP) return;
		cam.addEulerDegRotation(glm::vec3(0, 0, -5));
	}
	if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_UP) return;
		cam.addEulerDegRotation(glm::vec3(0, 0, 5));
	}
	if ((button == 3) || (button == 4)) // It's a wheel event
	{
		// Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
		if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
		normalLevel += button == 3 ? normalLevelStep : -normalLevelStep;
		normalLevel = glm::clamp(normalLevel, 0.f, 100.f);
		heightScale += button == 3 ? heightScaleStep : -heightScaleStep;
		heightScale = glm::clamp(heightScale, 0.f, 100.f);
	}
	if (button == GLUT_MIDDLE_BUTTON)
	{
		spawnParticles = true;
		
	}
	
}

void saveSamplesToFile(int Samples)
{
	std::ofstream outfile;
	outfile.open("config.txt");
	outfile << Samples;
	outfile.close();
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 't')
	{
		wireframeMode = !wireframeMode;
	}
	
	if (key == 'r')
	{
		loadShaders();		

	}
	
	if (key == 's')
	{
		camMovementVector += glm::vec3(0, 0, -manualMovementStep);
	}

	if (key == 'w')
	{
		camMovementVector += glm::vec3(0, 0, manualMovementStep);
	}

	if (key == 'd')
	{
		camMovementVector += glm::vec3(-manualMovementStep, 0, 0);

	}

	if (key == 'a')
	{
		camMovementVector += glm::vec3(manualMovementStep, 0, 0);
	}

	if (key == 'q')
	{
		camMovementVector += glm::vec3(0, -manualMovementStep, 0);
	}

	if (key == 'e')
	{
		camMovementVector += glm::vec3(0, manualMovementStep, 0);
	}
	
	//add point
	if (key == ' ')
	{
		rot_curve.addPoint(cam.getRotation());
		position_curve.addPoint(cam.getPosition());
	}

	//play
	if (key == 'p')
	{
		in_progress = !in_progress;
		cameraMoveTimer = 0;
	}

	//clear
	if (key == 'c')
	{
		std::cout << "cleared Points" << std::endl;
		rot_curve.clear();
		position_curve.clear();
	}

	//HeightScale +
	if (key == '1')
	{
		heightScale += heightScaleStep;
	}

	//HeightScale -
	if (key == '2')
	{
		heightScale -= heightScaleStep;
	}

	//primaryLayers +
	if (key == '3')
	{
		primaryLayers++;
	}

	//primaryLayers -
	if (key == '4')
	{
		if(primaryLayers > 1) primaryLayers--;
	}
	//secondaryLayers +
	if (key == '5')
	{
		secondaryLayers++;
	}

	//secondaryLayers -
	if (key == '6')
	{
		if (secondaryLayers > 1) secondaryLayers--;
	}

	
	//speed + 
	if (key == '.')
	{
		camSpeed += 10;
		float current_t = cameraMoveTimer / cameraMoveTime;
		++cameraMoveTime;
		cameraMoveTimer = cameraMoveTime * current_t;
		std::cout << "Tracktime: " << cameraMoveTime << std::endl;
	}

	//speed -
	if (key == ',' && cameraMoveTime > 1)
	{

		camSpeed -= 10;
		float current_t = cameraMoveTimer / cameraMoveTime;
		--cameraMoveTime;
		cameraMoveTimer = cameraMoveTime * current_t;
		std::cout << "Tracktime: " << cameraMoveTime << std::endl;
	}
}

void keyboard_up(unsigned char key, int x, int y)
{
	if (key == 's' || key == 'w' || key == 'a' || key == 'd' || key == 'q' || key == 'e')
	{
		camMovementVector = glm::vec3(0,0,0);
	}
		
}

void mouse_move(int x, int y)
{
	//mouse x controls y rotation
	//mouse y controls x rotation

	if (!camlock && glm::abs(x - mouse_pos.x) < 100 && glm::abs(y - mouse_pos.y) < 100)
	{
		cam.addRotation(glm::quat(glm::vec3{0, (x - mouse_pos.x) * mouse_sensitivity, 0}));
		cam.addRotation(glm::quat(glm::vec3{(y - mouse_pos.y) * mouse_sensitivity, 0, 0}));
	}
	mouse_pos.x = x;
	mouse_pos.y = y;

}

int main(int argc, char* argv[])
{
	// Initialize GLUT
	while (continueApplication == true)
	{
		continueApplication = false;
		glutInit(&argc, argv);

		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
		
		// Set the window size
		glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

		//Anti Aliasing 
		std::fstream myfile("config.txt");

		int sampleAmount = 0;

		if (myfile.is_open())
		{
			//read xy
			std::string line;
			std::getline(myfile, line);
			sampleAmount = std::stoi(line);
			myfile.close();
		}
		else
		{
			std::cout << "Unable to open file" << std::endl;
		}

		glutSetOption(GLUT_MULTISAMPLE, sampleAmount);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);


		glutCreateWindow("UE_KNAB");

		//call initialization routine
		init();


		// Bind the two functions (above) to respond when necessary
		glutDisplayFunc(display);

		glutIdleFunc(idle);

		glutKeyboardFunc(keyboard);
		glutKeyboardUpFunc(keyboard_up);
		glutMouseFunc(mouse);

		glutPassiveMotionFunc(mouse_move);

		glutReshapeFunc(reshape);

		// Very important!  This initializes the entry points in the OpenGL driver so we can 
		// call all the functions in the API.
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			fprintf(stderr, "GLEW error");
			return 1;
		}
		glutMainLoop();
	}
}
