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

GLfloat mouse_sensitivity = 0.005f;
GLfloat cameraMoveTime = 30;
GLfloat cameraMoveTimer = 0;

GLfloat x_rot = 0, y_rot = 0, z_rot = 0;
GLfloat t_old = 0;
GLfloat camSpeed = 200;


CatmullRomCurve position_curve;
squadCurve rot_curve;

camera cam;
bool in_progress = false;
bool camlock = false;
bool continueApplication = true;

static const GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };

//materialSpecular and materialShininess can be used to change the initial settings of the light source
static const GLfloat materialSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
static const GLfloat materialShininess = 20.0;


float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f
};

float quadVertices[] = {
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

Shader* ShadowMapShader;
unsigned int VAO, VBO, EBO;
unsigned int diffuseMap, specMap, normalMap;

unsigned int FBO;
unsigned int FBOtexture;


unsigned int RBO;

Shader* FBOShader;
unsigned int VAO_FBO, VBO_FBO;

Shader* lampShader;
unsigned int lightVAO;

GLfloat base_cube_pos[] = { 0, 3, 0};
GLfloat cam_pos[] = { 0.0, 0, 0 };
GLfloat distanceBetweenCubes = 2;
Model * ourModel;
float normalLevel = 0.3f;
GLfloat normalLevelStep = 0.1f;

std::vector<glm::vec3> lightPos{ glm::vec3(-10.0f,	10.f,	-10.f),
									glm::vec3(5.f,		10.f,	-10.f)};

long oldTimeSinceStart;
int deltaTime;
//Shadow Map
std::vector<unsigned int> depthMapFBO;
const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
std::vector<unsigned int> depthMap;
Shader* simpleDepthShader;

//initialization of the application.
void init()
{
	GLenum err = glewInit();
	glClearColor(0.0, 0.0, 0.0, 0.0);

	GLint iMultiSample = 0;
	GLint iNumSamples = 0;
	glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
	glGetIntegerv(GL_SAMPLES, &iNumSamples);
	printf("MSAA on, GL_SAMPLE_BUFFERS = %d, GL_SAMPLES = %d\n", iMultiSample, iNumSamples);



	//model loading 
	ourModel = new Model("models/cube.obj");

	glGenBuffers(1, &VBO);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);



	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//Basic Default Shadow Mapping
	ShadowMapShader = new Shader("shader/vshader.txt", "shader/fshader.txt");
	simpleDepthShader = new Shader("shader/vshader_depth.txt", "shader/fshader_depth.txt");

	//light
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//Basic Default Lamp Shading
	lampShader = new Shader("shader/vshader.txt", "shader/fshader_light.txt");

	//FRAMEBUFFER VBO AND VAO
	glGenBuffers(1, &VBO_FBO);

	glGenVertexArrays(1, &VAO_FBO);
	glBindVertexArray(VAO_FBO);

	// 2. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO_FBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);



	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Basic Default FBO
	FBOShader = new Shader("shader/vshader_FBO.txt", "shader/fshader_FBO.txt");

	


#pragma region FBOs_RBOs

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &FBOtexture);
	glBindTexture(GL_TEXTURE_2D, FBOtexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBOtexture, 0);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glDeleteFramebuffers(1, &FBO);
	
	glGenRenderbuffers(1, &RBO); 
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	FBOShader->use();
	FBOShader->setInt("screenTexture", 0);
	for (int i = 0; i < lightPos.size(); i++)
	{
		depthMap.push_back(0);
		depthMapFBO.push_back(0);
		//depthFBO
		glGenFramebuffers(1, &depthMapFBO[i]);

		glGenTextures(1, &depthMap[i]);
		glBindTexture(GL_TEXTURE_2D, depthMap[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap[i], 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	

#pragma endregion


#pragma region texture


	glGenTextures(1, &diffuseMap);
	glBindTexture(GL_TEXTURE_2D, diffuseMap); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("textures/tlumbejo_4K_Albedo.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glGenTextures(1, &specMap);
	glBindTexture(GL_TEXTURE_2D, specMap); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("textures/tlumbejo_4K_Gloss.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glGenTextures(1, &normalMap);
	glBindTexture(GL_TEXTURE_2D, normalMap); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("textures/tlumbejo_4K_Normal.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	ShadowMapShader->use();
	ShadowMapShader->setInt("material.diffuse", 0);
	ShadowMapShader->setInt("material.specular", 1);
	ShadowMapShader->setInt("material.normal", 2);

	

#pragma endregion



	//glShadeModel(GL_FLAT);
	

	glEnable(GL_DEPTH_TEST);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
	glEnable(GL_MULTISAMPLE);

	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	//glEnable(GL_NORMALIZE);

	cam.setPosition(glm::vec3(0, 0.5, -1));

	cam.setRotation(glm::quat(0.0f,0.0f,1.0f,0.0f));
	//glm::vec3 check = rot_curve.evaluateEuler(1);

	//cout << " x: " << check.x << " y: " << check.y << " z: " << check.z << endl;
	//select clearing color (color that is used as 'background')

	//set Light
	glPushMatrix();
		glTranslated(0, 5,0);
		glLightfv(GL_LIGHT0, GL_POSITION, new float[4]{ 0,0,0,1.0 });
	glPopMatrix();

	// Common Light Props
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);



}

void reshape(int w, int h)
{
	WINDOW_WIDTH = w;
	WINDOW_HEIGHT = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(60, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1, 50);
	proj = glm::perspective(glm::radians(60.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
}



void geometry(Shader* UsingShader)
{
	
	//render floor
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0, -1, 0));
	model = glm::scale(model, glm::vec3(20, 1, 20));

	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f));	
	UsingShader->setFloat("normalLevel", 0);
	UsingShader->setMat4("model", model);
	ourModel->Draw(*UsingShader);
	UsingShader->setFloat("normalLevel", normalLevel);
	model = glm::mat4(1.0f);
	// render Cubes

	for (int i = -5; i <= 4; i++)
	{
		for (int j = -5; j <= 4; j++)
		{

			
			glm::mat4 model = glm::mat4(1.0f);	
			model = glm::translate(model, glm::vec3(base_cube_pos[0] - i * distanceBetweenCubes, base_cube_pos[1] + (2 * cos(i * cube_height_scale_x) + 2 * cos(j * cube_height_scale_y)), base_cube_pos[2] - j * distanceBetweenCubes));
			model = glm::rotate(model, (1 + i * -j) * oldTimeSinceStart / 10000.f, glm::vec3(glm::abs(cos(i)),glm::abs(cos(j)), 0));
			model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
			//model = glm::scale(model, glm::vec3(1, cos(i * cube_height_scale_x) + cos(j * cube_height_scale_y) + 1.2f, 1));
			UsingShader->setMat4("model", model);
			ourModel->Draw(*UsingShader);
			
		}
	}
}

void display()
{
	// 1. first render to depth map
	float near_plane = 5.0f, far_plane = 30;
	for (int i = 0; i < lightPos.size(); i++)
	{	
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glm::mat4 lightSpaceMatrix;
		CalcLightSpaceMatrix(near_plane, far_plane, lightPos[i], lightSpaceMatrix);

		simpleDepthShader->use(); 
		simpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[i]);
		
			glClear(GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);
			glBindVertexArray(VAO);
			geometry(simpleDepthShader);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);// 2. then render scene as normal with shadow mapping (using depth map)
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, depthMap[i]);
	}
	
	
	// first pass
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
	glEnable(GL_DEPTH_TEST);

	//time, ms are taken for sufficient for this exercise 
	long timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = timeSinceStart - oldTimeSinceStart;
	oldTimeSinceStart = timeSinceStart;

	if (cameraMoveTimer < cameraMoveTime && in_progress)
	{
		cameraMoveTimer += (float)deltaTime / 1000;
	}



	//rotate the scene
	if (in_progress) {
		cam.setRotation(rot_curve.evaluate(cameraMoveTimer / cameraMoveTime));
		//catmull pos
		cam.setPosition(position_curve.evaluate(cameraMoveTimer / cameraMoveTime));
	}




	view = glm::mat4(1.0f);
	view = cam.getTransformationMatrix();

	//light
	for (int i = 0; i < lightPos.size(); i++)
	{
		lampShader->use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos[i]);
		model = glm::scale(model, glm::vec3(0.2f));

		lampShader->setMat4("projection", proj);
		lampShader->setMat4("view", view);
		lampShader->setMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
		



	ShadowMapShader->use();

	ShadowMapShader->setMat4("projection", proj);
	ShadowMapShader->setMat4("view", view);
	ShadowMapShader->setVec3("viewPos", cam.getPosition());

	ShadowMapShader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	ShadowMapShader->setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
	ShadowMapShader->setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
	ShadowMapShader->setVec3("material.specular", 0.5f, 0.5f, 0.5f);
	ShadowMapShader->setFloat("material.shininess", 32.0f);

	ShadowMapShader->setFloat("normalLevel", normalLevel);
	
	ShadowMapShader->setVec3("light[0].diffuse", 3, 3, 3);
	ShadowMapShader->setVec3("light[1].diffuse", 2, 2, 1);
	ShadowMapShader->setVec3("light[0].specular", 3, 3, 3);
	ShadowMapShader->setVec3("light[1].specular", 5, 5, 5);
	for (int i = 0; i < lightPos.size(); i++) {
		ShadowMapShader->setVec3("light[" + std::to_string(i) + "].ambient", 0.1f, 0.1f, 0.1f);
		//ShadowMapShader->setVec3("light[" + std::to_string(i) + "].diffuse", 0.5, 0.5f, 0.5f);
		//ShadowMapShader->setVec3("light[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
		ShadowMapShader->setVec3("light[" + std::to_string(i) + "].position", lightPos[i]);

		glm::mat4 lightSpaceMatrix;
		CalcLightSpaceMatrix(near_plane, far_plane, lightPos[i], lightSpaceMatrix);
		ShadowMapShader->setMat4("lightSpaceMatrix[" + std::to_string(i) + "]", lightSpaceMatrix);
	}
	

	glBindVertexArray(VAO);

	//bind Textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specMap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	for (int i = 0; i < lightPos.size(); i++)
	{
		glActiveTexture(GL_TEXTURE3 + i);
		glBindTexture(GL_TEXTURE_2D, depthMap[i]);

		ShadowMapShader->setInt("shadowMap[" + std::to_string(i) + "]", 3 + i);
	}
	
	geometry(ShadowMapShader);

	//Second Pass
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
	//glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);

	//FBOShader->use();
	//glBindVertexArray(VAO_FBO);
	////glDisable(GL_DEPTH_TEST);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, FBOtexture);
	//glDrawArrays(GL_TRIANGLES, 0, 6);


	glutSwapBuffers();
}

void CalcLightSpaceMatrix(float near_plane, float far_plane, glm::vec3 pos, glm::mat4& lightSpaceMatrix)
{
	glm::mat4 lightProjection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(pos,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;
}

void idle() {
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_UP) return;
		cam.addEulerDegRotation(glm::vec3(0, 0, -5));
	}
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_UP) return;
		cam.addEulerDegRotation(glm::vec3(0, 0, 5));
	}
	if ((button == 3) || (button == 4)) // It's a wheel event
	{
		// Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
		if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
		normalLevel += button == 3 ? normalLevelStep : -normalLevelStep;
		normalLevel = glm::clamp(normalLevel,0.f,1.f);
	}
}

void saveSamplesToFile(int Samples) {
	std::ofstream outfile;
	outfile.open("config.txt");
	outfile << Samples;
	outfile.close();
}

void keyboard(unsigned char key, int x, int y) {
	if (key == '1')
	{
		saveSamplesToFile(2);
		continueApplication = true;
		glutLeaveMainLoop();
	}
	if (key == '2')
	{
	   	saveSamplesToFile(4);
		continueApplication = true;
		glutLeaveMainLoop();
	}
	if (key == '3')
	{
		saveSamplesToFile(8);
		continueApplication = true;
		glutLeaveMainLoop();
	}
	if (key == '4')
	{
		saveSamplesToFile(16);
		continueApplication = true;
		glutLeaveMainLoop();
	}
	if (key == '0')
	{
		glDisable(GL_MULTISAMPLE);
	}
	if (key == '9')
	{
		glEnable(GL_MULTISAMPLE);
	}
	if (key == 's' ) {
		cam.addToLocalPosition(glm::vec3(0, 0, -manualMovementStep));
		glutPostRedisplay();
	}

	if (key == 'w') {
		cam.addToLocalPosition(glm::vec3(0, 0, manualMovementStep));
		glutPostRedisplay();
	}

	if (key == 'd') {
		cam.addToLocalPosition(glm::vec3(-manualMovementStep, 0, 0));
		glutPostRedisplay();
	}

	if (key == 'a') {
		cam.addToLocalPosition(glm::vec3(manualMovementStep, 0, 0));
		glutPostRedisplay();
	}

	if (key == 'q') {
		cam.addToLocalPosition(glm::vec3(0, -manualMovementStep, 0));
		glutPostRedisplay();
	}

	if (key == 'e') {
		cam.addToLocalPosition(glm::vec3(0, manualMovementStep, 0));
		glutPostRedisplay();
	}

	if (key == 'l') {
		camlock = true;
	}

	if(key == 'u') {
		camlock = false;
	}

	if (key == 'r')
	{
		//Basic Default Shadow Mapping
		ShadowMapShader = new Shader("shader/vshader.txt", "shader/fshader.txt");
		FBOShader = new Shader("shader/vshader_FBO.txt", "shader/fshader_FBO.txt");
		ShadowMapShader->use();
		ShadowMapShader->setInt("material.diffuse", 0);
		ShadowMapShader->setInt("material.specular", 1);
		ShadowMapShader->setInt("material.normal", 2);
		ShadowMapShader->setInt("shadowMap", 2);
	}

	//add point
	if (key == ' ') {
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
		cout << "cleared Points" << endl;
		rot_curve.clear();
		position_curve.clear();
	}

	//speed + 
	if (key == '.') {
		float current_t = cameraMoveTimer / cameraMoveTime;
		++cameraMoveTime;
		cameraMoveTimer = cameraMoveTime * current_t;
		cout << "Tracktime: " << cameraMoveTime << endl;
	}

	//speed -
	if (key == ',' && cameraMoveTime > 1) {
		float current_t = cameraMoveTimer / cameraMoveTime;
		--cameraMoveTime;
		cameraMoveTimer = cameraMoveTime * current_t;
		cout << "Tracktime: " << cameraMoveTime << endl;
	}
}

void mouse_move(int x, int y)
{
	//mouse x controls y rotation
	//mouse y controls x rotation

	if (!camlock && glm::abs(x - mouse_pos.x) < 100 && glm::abs(y - mouse_pos.y) < 100) {

		cam.addRotation(glm::quat(glm::vec3{0, (x - mouse_pos.x) * mouse_sensitivity, 0}));
		cam.addRotation(glm::quat(glm::vec3{(y - mouse_pos.y) * mouse_sensitivity, 0, 0 }));
	}
	mouse_pos.x = x;
	mouse_pos.y = y;
}

int main(int argc, char* argv[]) {
	// Initialize GLUT
	while (continueApplication == true) {
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
		else {
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

		glutMouseFunc(mouse);

		glutPassiveMotionFunc(mouse_move);

		glutReshapeFunc(reshape);

		// Very important!  This initializes the entry points in the OpenGL driver so we can 
		// call all the functions in the API.
		GLenum err = glewInit();
		if (GLEW_OK != err) {
			fprintf(stderr, "GLEW error");
			return 1;
		}


		glutMainLoop();

	}
	
}