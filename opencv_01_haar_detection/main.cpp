/*** Include ***/
/* for general */
#include <stdint.h>
#include <stdio.h>
#include <fstream> 
#include <vector>
#include <string>
#include <chrono>

/* for OpenCV */
#include <opencv2/opencv.hpp>

/* for GLFW */
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp> 

#include "shader.h"
#include "texture.h"
#include "objloader.h"
#include "CameraControls.h"
#include "Background.h"

/*** Macro ***/
/* macro functions */
#define RUN_CHECK(x)                                         \
  if (!(x)) {                                                \
    fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
    exit(1);                                                 \
  }

/* Settings */
#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT  720
#define OBJECT_NUM 2
//#define HAAR_FILENAME "resource/haarcascade_frontalface_alt.xml"
#define HAAR_FILENAME "resource/rpalm.xml"

/*** Global variables ***/


/*** Function ***/
int generateObjectFromFile(const char *filename, GLuint *vertexBuffer, GLuint *uvBuffer, float *sizeY)
{
	/* Read .obj file */
	std::vector<unsigned short> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	RUN_CHECK(loadAssImp(filename, indices, vertices, uvs, normals));

	/* Create Vertex Buffer Object and copy data */
	glGenBuffers(1, vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	if (uvs.size() > 0) {
		glGenBuffers(1, uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, *uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	}

	/* Calculate object size (Y only) */
	float objectMinY = 999999;
	float objectMaxY = -999999;
	for (int i = 0; i < vertices.size(); i++) {
		if (vertices[i].y > objectMaxY) objectMaxY = vertices[i].y;
		if (vertices[i].y < objectMinY) objectMinY = vertices[i].y;
	}
	*sizeY = objectMaxY - objectMinY;
	return vertices.size();
}

int main(int argc, char *argv[])
{
	/*** Initialize ***/
	/* Initialize camera (OpenCV) */
	static cv::VideoCapture cap;
	cap = cv::VideoCapture(0);
	cap.set(cv::CAP_PROP_FRAME_WIDTH, WINDOW_WIDTH);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, WINDOW_HEIGHT);
	// cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('B', 'G', 'R', '3'));
	cap.set(cv::CAP_PROP_BUFFERSIZE, 1);
	
	/* Initialize cascade (OpenCV) */
	cv::CascadeClassifier cascade;
	cascade.load(HAAR_FILENAME);

	/* Initialize GLFW */
	GLFWwindow* window;
	RUN_CHECK(glfwInit() == GL_TRUE);

	/* Create a window (x4 anti-aliasing, OpenGL3.3 Core Profile)*/
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	RUN_CHECK(window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "main", NULL, NULL));
	glfwMakeContextCurrent(window);

	/* Initialize GLEW */
	glewExperimental = true;
	RUN_CHECK(glewInit() == GLEW_OK);

	/* Ensure not to miss input */
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);

	/* Dark blue background */
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	/* Enable depth test */
	glEnable(GL_DEPTH_TEST);
	/* Accept fragment if it closer to the camera than the former one */
	glDepthFunc(GL_LESS);
	/* Cull triangles which normal is not towards the camera */
	glEnable(GL_CULL_FACE);

	/* Load shader and get handle */
	GLuint programId = LoadShaders("resource/TransformVertexShader.vertexshader", "resource/TextureFragmentShader.fragmentshader");
	GLuint matrixId = glGetUniformLocation(programId, "MVP");
	GLuint textureId = glGetUniformLocation(programId, "myTextureSampler");

	/* Read the texture */
	GLuint texture = loadDDS("resource/uvmap.DDS");

	/* Create Vertex Array Object */
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/* Initialize for background */
	BackgroundDrawer_init(WINDOW_WIDTH, WINDOW_HEIGHT);

	/* Load object file and geneerate VBO  */
	int indexObject = 0;
	int partCount[OBJECT_NUM];
	GLuint vertexBuffer[OBJECT_NUM];
	GLuint uvBuffer[OBJECT_NUM];
	float objectSize[OBJECT_NUM];
	float objectDefaultScale[OBJECT_NUM];
	partCount[0] = generateObjectFromFile("resource/miku_Ver17.02.pmx", &vertexBuffer[0], &uvBuffer[0], &objectSize[0]);
	partCount[1] = generateObjectFromFile("resource/nendomiku_ver3_00.pmx", &vertexBuffer[1], &uvBuffer[1], &objectSize[1]);
	/* Calculate scale to fit the object to -1.0 ~ 1.0 window (Orthogonal coordinates)  */
	for (int i = 0; i < OBJECT_NUM; i++) {
		objectDefaultScale[i] = 2.0f / objectSize[i];
	}

	/* Initialize camera matrix controls (Initial position : on +Z, toward -Z) */
	CameraControls_initialize(window, glm::vec3(0, 0, 5), 3.14f, 0.0f);

	/*** Start loop ***/
	while (1) {
		/* Clear the screen */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Capture camera image */
		cv::Mat capImage;
		cap.read(capImage);

		/* Detect objects */
		cv::Mat imageGray;
		//cvtColor(capImage, imageGray, cv::COLOR_BGR2GRAY);
		//cv::equalizeHist(imageGray, imageGray);
		std::vector<cv::Rect> listDet;
		cascade.detectMultiScale(capImage, listDet, 1.1, 8, cv::CASCADE_SCALE_IMAGE , cv::Size(30, 30));
		static double s_lastDetTime = glfwGetTime();
		for (size_t i = 0; i < listDet.size(); i++) {
			cv::rectangle(capImage, listDet[i], cv::Scalar(255, 0, 0));
			s_lastDetTime = glfwGetTime();
		}
		if (glfwGetTime() - s_lastDetTime > 2 && s_lastDetTime != -1) {
			/* switch object */
			indexObject = (indexObject + 1) % OBJECT_NUM;
			s_lastDetTime = -1;	// -1 means already switched, but not displayed yet
		}

		/* Draw background */
		//cv::cvtColor(capImage, capImage, cv::COLOR_BGR2RGB);
		cv::resize(capImage, capImage, cv::Size(WINDOW_WIDTH, WINDOW_HEIGHT));
		BackgroundDrawer_draw(WINDOW_WIDTH, WINDOW_HEIGHT, capImage.data);
		glClear(GL_DEPTH_BUFFER_BIT);		// draw background as back

		glUseProgram(programId);
		/* Camera matrix */
		CameraControls_update(window);
		glm::mat4 Projection = CameraControls_getProjectionMatrix();
		glm::mat4 View = CameraControls_getViewMatrix();

		/* Model matrix (move and resize using detection result, and always rotation) */
		glm::mat4 Model = glm::mat4(1.0f);
		static float rotY = 0.0f;
		rotY += 0.8f;
		glm::mat4 matModelRot = glm::rotate(rotY / (2 * 3.14f), glm::vec3(0, 1, 0));
		glm::mat4 matModelScaling = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
		glm::mat4 matModelTranslate = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
		if (listDet.size() > 0) {
			/* mode to the center of bounding box, and resize to the same size as bbox.height */
			float x = (listDet[0].x + listDet[0].width / 2 - WINDOW_WIDTH / 2.0f) / (WINDOW_WIDTH / 2);
			float y = (listDet[0].y + listDet[0].height / 2 - WINDOW_HEIGHT / 2.0f) / (WINDOW_HEIGHT / 2);
			matModelTranslate = glm::translate(glm::vec3(x, -y, 0.0f));
			float scale = (float)listDet[0].height / WINDOW_HEIGHT;	// scale against to window size
			scale *= 0.75;	// adjustment
			scale *= objectDefaultScale[indexObject];
			matModelScaling = glm::scale(glm::vec3(scale, scale, scale));
		} else {
			/* don't display */
			matModelScaling = glm::scale(glm::vec3(0.0, 0.0, 0.0));
		}
		Model = matModelTranslate * matModelRot * matModelScaling * Model;

		/* Calculate ModelViewProjection matrix and send it to shader */
		glm::mat4 MVP = Projection * View * Model;
		glUniformMatrix4fv(matrixId, 1, GL_FALSE, &MVP[0][0]);

		/* Bind Texture */
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(textureId, 0);

		/* Set attribute buffer */
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[indexObject]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[indexObject]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		
		/* Draw the triangle */
		glDrawArrays(GL_LINE_LOOP, 0, partCount[indexObject]);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glUseProgram(0);

		/* Swap buffers */
		glfwSwapBuffers(window);
		glfwPollEvents();

		/* Check if the ESC key was pressed or the window was closed */
		if (glfwWindowShouldClose(window) != 0 || glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			break;
		}
	}

	/*** Finalize ***/
	BackgroundDrawer_finalize();
	/* Cleanup VBO and shader */
	glDeleteBuffers(OBJECT_NUM, &vertexBuffer[0]);
	glDeleteBuffers(OBJECT_NUM, &uvBuffer[0]);
	glDeleteTextures(1, &textureId);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(programId);

	/* Close OpenGL window and terminate GLFW */
	glfwTerminate();

	return 0;
}
