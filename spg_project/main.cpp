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

static const GLfloat white[] = {1.0, 1.0, 1.0, 1.0};

//materialSpecular and materialShininess can be used to change the initial settings of the light source
static const GLfloat materialSpecular[] = {1.0, 1.0, 1.0, 1.0};
static const GLfloat materialShininess = 20.0;



std::vector<float> points;
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
unsigned int VAO, VBO, EBO;
unsigned int diffuseMap, specMap, normalMap;

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

long oldTimeSinceStart;
int deltaTime;
//Shadow Map
std::vector<unsigned int> depthMapFBO;
const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
std::vector<unsigned int> depthMap;
Shader* simpleDepthShader;

Shader *densityShader, *computeShader3DTexture;
GLuint densityTexture;
unsigned int textureWidth = 96;
unsigned int textureDepth = 96;
unsigned int textureHeigth = 256;

unsigned int quadVAO, quadVBO;
GLenum* DrawBuffers = new GLenum[2];
bool wireframeMode = false;

void loadShaders() {
	//Basic Default Shadow Mapping
	marchingCubesShader = new Shader("shader/vshader.glsl", "shader/fshader.glsl", "shader/gshader.glsl");
	computeShader3DTexture = new Shader("shader/cshader.glsl");
}

//initialization of the application.
void init()
{
	GLenum err = glewInit();
	glClearColor(0.0, 0.0, 0.0, 0.0);

	const int arraySize = (textureWidth - 1) * (textureDepth * 2 - 1);
	
	//initialize vertices
	//points = new float[arraySize];
	
	for (int i = 0; i < textureWidth - 1; ++i)
	{
		for (int j = 0; j < textureDepth * 2 - 1; j += 2)
		{
			int index = i * textureWidth + j;
			points.push_back(float(i) * pointScale);
			points.push_back(float(j) * pointScale);
		}		
	}

	//points = new float[4]{1,0,0,1};
	
	GLint iMultiSample = 0;
	GLint iNumSamples = 0;
	//glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
	//glGetIntegerv(GL_SAMPLES, &iNumSamples);
	//printf("MSAA on, GL_SAMPLE_BUFFERS = %d, GL_SAMPLES = %d\n", iMultiSample, iNumSamples);



	//model loading 
	ourModel = new Model("models/cube.obj");

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), &points[0], GL_STATIC_DRAW);
	
	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	glGenBuffers(1, &quadVBO);
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	loadShaders();
	glEnable(GL_TEXTURE_3D);
	glGenTextures(1, &densityTexture);
	glBindTexture(GL_TEXTURE_3D, densityTexture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, textureWidth, textureDepth, textureHeigth, 0, GL_RED, GL_UNSIGNED_SHORT, NULL);
	glBindImageTexture(0, densityTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R8);
	err = glGetError();
	glBindTexture(GL_TEXTURE_3D, 0);

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
	proj = glm::perspective(glm::radians(60.0f), static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
	                        0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
}



void display()
{
	
	float near_plane = -5.0f, far_plane = 30;

	//render 3D Texture
	
	computeShader3DTexture->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, densityTexture);
	glBindImageTexture(0, densityTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R16F);

	glDispatchCompute(textureWidth, textureDepth, textureHeigth);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.5f, 0.9f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//time, ms are taken for sufficient for this exercise 
	long timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = timeSinceStart - oldTimeSinceStart;
	oldTimeSinceStart = timeSinceStart;

	if (cameraMoveTimer < cameraMoveTime && in_progress)
	{
		cameraMoveTimer += static_cast<float>(deltaTime) / 1000;
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
	glBindTexture(GL_TEXTURE_3D, densityTexture);
	marchingCubesShader->setMat4("projection", proj);
	marchingCubesShader->setMat4("view", view);
	model = glm::mat4(1.0f);
	marchingCubesShader->setMat4("model", model);
	glBindVertexArray(VAO);
	glDrawArraysInstanced(GL_POINTS, 0, (textureWidth - 1) * (textureDepth -1), textureHeigth);

	//glDrawArrays(GL_POINTS, 0, 2);
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
		normalLevel = glm::clamp(normalLevel, 0.f, 1.f);
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
	if (key == 's')
	{
		cam.addToLocalPosition(glm::vec3(0, 0, -manualMovementStep));
		glutPostRedisplay();
	}

	if (key == 'w')
	{
		cam.addToLocalPosition(glm::vec3(0, 0, manualMovementStep * 100));
		glutPostRedisplay();
	}

	if (key == 'd')
	{
		cam.addToLocalPosition(glm::vec3(-manualMovementStep, 0, 0));
		glutPostRedisplay();
	}

	if (key == 'a')
	{
		cam.addToLocalPosition(glm::vec3(manualMovementStep, 0, 0));
		glutPostRedisplay();
	}

	if (key == 't')
	{
		wireframeMode = !wireframeMode;
	}

	if (key == 'q')
	{
		cam.addToLocalPosition(glm::vec3(0, -manualMovementStep, 0));
		glutPostRedisplay();
	}

	if (key == 'e')
	{
		cam.addToLocalPosition(glm::vec3(0, manualMovementStep, 0));
		glutPostRedisplay();
	}

	if (key == 'l')
	{
		camlock = true;
	}

	if (key == 'u')
	{
		camlock = false;
	}

	if (key == 'r')
	{
		loadShaders();

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
		cout << "cleared Points" << endl;
		rot_curve.clear();
		position_curve.clear();
	}

	//speed + 
	if (key == '.')
	{
		float current_t = cameraMoveTimer / cameraMoveTime;
		++cameraMoveTime;
		cameraMoveTimer = cameraMoveTime * current_t;
		cout << "Tracktime: " << cameraMoveTime << endl;
	}

	//speed -
	if (key == ',' && cameraMoveTime > 1)
	{
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

	if (!camlock && glm::abs(x - mouse_pos.x) < 100 && glm::abs(y - mouse_pos.y) < 100)
	{
		cam.addRotation(glm::quat(glm::vec3{0, (x - mouse_pos.x) * mouse_sensitivity, 0}));
		cam.addRotation(glm::quat(glm::vec3{(y - mouse_pos.y) * mouse_sensitivity, 0, 0}));
	}
	mouse_pos.x = x;
	mouse_pos.y = y;

	//printf("current Texture Z = %i\n", glm::clamp(static_cast<int>(cam.getEulerDegRotation().x), 0, 256));
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
