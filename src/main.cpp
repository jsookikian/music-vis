/*
Base code
Currently will make 2 FBOs and textures (only uses one in base code)
and writes out frame as a .png (Texture_output.png)

Winter 2017 - ZJW (Piddington texture write)
2017 integration with pitch and yaw camera lab (set up for texture mapping lab)
*/

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "WindowManager.h"
#include "GLTextureWriter.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

#define NUM_SEGMENTS 10

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program

	// Shape to be used (from obj file)
	// shared_ptr<Shape> shape;
	// shared_ptr<Shape> cubeShape;



	std::shared_ptr<Program> sphere;

	GLuint VertexArrayID;
	GLuint VertexBufferID;  
	GLuint NormalBufferID;  
	GLuint ColorBufferID;
	GLuint IndexBufferID;

	//reference to texture FBO
	GLuint frameBuf[2];
	GLuint texBuf[2];
	GLuint depthBuf;
	float deltaTime, lastTime = glfwGetTime(), timer = lastTime, time = 0;
	bool FirstTime = true;
	bool Moving = false;
	int gMat = 0;

	float cTheta = 0;
	float mouseDx, mouseDy;
	float prevX, prevY, prevZ, curX, curY, curZ;
	double pX, pY;
	float theta, phi;
	float scale = 0.05;

	float sTheta, eTheta, qTheta;
	float GT = 2.5;

	bool mouseDown = false;

	vec3 target, up, eyeVec, view, strafe;
	

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		float speed = 0.2;
		if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if (key == GLFW_KEY_M && action == GLFW_PRESS)
		{
			gMat = (gMat + 1) % 4;
		}
		else if (key == GLFW_KEY_S && (action == GLFW_PRESS|| action == GLFW_REPEAT))
		{
			eyeVec -= speed * target;
		}
		else if (key == GLFW_KEY_W && (action == GLFW_PRESS|| action == GLFW_REPEAT))
		{
			eyeVec += speed * target;
		}
		else if (key == GLFW_KEY_A && (action == GLFW_PRESS|| action == GLFW_REPEAT))
		{
			strafe = cross(target, up);
			eyeVec -= speed * strafe;

		}
		else if (key == GLFW_KEY_D && (action == GLFW_PRESS|| action == GLFW_REPEAT))
		{
			strafe = cross(target, up);
			eyeVec += speed * strafe;
		}
	}

	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY)
	{
		cTheta += (float) deltaX;
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			mouseDown = true;
			glfwGetCursorPos(window, &posX, &posY);
			cout << "Pos X " << posX << " Pos Y " << posY << endl;
			prevX = posX;
			prevY = posY;
			Moving = true;
		}

		if (action == GLFW_RELEASE)
		{
			Moving = false;
			mouseDown = false;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void initSphere(GLfloat *vertexBuf, GLfloat *normalBuf, GLuint *indexBuf) {
		
		float two_pi = 2.0f * M_PI;
		
		int ndx = 0;
		// Generate the verticed around the circle
		int t = 0;
		int k = 0;
		for (ndx = 0; ndx < NUM_SEGMENTS; ndx++) {
			vertexBuf[t*3 + 0] = ndx;
			vertexBuf[t*3 + 1] = ndx;
			vertexBuf[t*3 + 2] = 1;
			std::cout << vertexBuf[t*3 + 0] << ", " << vertexBuf[t*3 + 1] << ", " << vertexBuf[t*3 + 2] << std::endl;
		
			// normalBuf[t*3 + 0] = cos(2 * t);
			// normalBuf[t*3 + 1] = t;
			// normalBuf[t*3 + 2] = sin(2*t);
			t++;

			vertexBuf[t*3 + 0] = ndx;
			vertexBuf[t*3 + 1] = ndx;
			vertexBuf[t*3 + 2] = 0;
			std::cout << vertexBuf[t*3 + 0] << ", " << vertexBuf[t*3 + 1] << ", " << vertexBuf[t*3 + 2] << std::endl;
		
			// normalBuf[t*3 + 0] = cos(2 * t);
			// normalBuf[t*3 + 1] = t;
			// normalBuf[t*3 + 2] = 0;
			t++;
			k += 2;
		}

	}

	void createFBO(GLuint& fb, GLuint& tex)
	{
		//initialize FBO
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);

		//set up framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		//set up texture
		glBindTexture(GL_TEXTURE_2D, tex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			cout << "Error setting up frame buffer - exiting" << endl;
			exit(0);
		}
	}
	void init(const std::string& resourceDirectory)
	{
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		GLSL::checkVersion();
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cTheta = 0;
		// // Set background color.
		// // Enable z-buffer test.
		// glEnable(GL_DEPTH_TEST);


		// //create two frame buffer objects to toggle between
		// glGenFramebuffers(2, frameBuf);
		// glGenTextures(2, texBuf);
		// glGenRenderbuffers(1, &depthBuf);
		// createFBO(frameBuf[0], texBuf[0]);

		// //set up depth necessary as rendering a mesh that needs depth test
		// glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
		// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		// glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

		// //more FBO set up
		// GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		// glDrawBuffers(1, DrawBuffers);

		// //create another FBO so we can swap back and forth
		// createFBO(frameBuf[1], texBuf[1]);
		//this one doesn't need depth

		//set up the shaders to blur the FBO just a placeholder pass thru now
		//next lab modify and possibly add other shaders to complete blur

		target = vec3(1,0,0);
		up = vec3(0,1,0);
		eyeVec = vec3(0,0,0);
		pX = 0;
		pY = 0;
		prevX = 0;
		prevY = 0;
		prevZ = 0;
		theta = 0;
		curX = curY = curZ = 0;
		phi = 0;
		glClearColor(0.12f, 0.34f, 0.56f, 0.0f);
		time = 0;

		sphere = std::make_shared<Program>();
		sphere->setVerbose(true);
		sphere->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		if (! sphere->init()) {
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		sphere->init();
		sphere->addUniform("P");
		sphere->addUniform("M");
		sphere->addUniform("V");
		sphere->addUniform("Time");
		


		sphere->addAttribute("vertPos");
		sphere->addAttribute("vertNor");
	 }

	void initGeom(const std::string& resourceDirectory)
	{
		GLfloat g_vertex_buffer_data[(NUM_SEGMENTS) * 6];
		GLfloat normal_buffer[(NUM_SEGMENTS) * 6];
		GLuint g_index_buffer_data[(NUM_SEGMENTS) * 6];
		initSphere((GLfloat *)&g_vertex_buffer_data, (GLfloat *)&normal_buffer, (GLuint *)&g_index_buffer_data);
		
		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);
		
		//Vertex Buffer
		glGenBuffers(1, &VertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		//glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
		
		
		// Generate and Bind Normal Buffer
		glGenBuffers(1, &NormalBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, NormalBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normal_buffer),normal_buffer, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, NormalBufferID);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);  
		//Generate and Bind Index Buffer
		glGenBuffers(1, &IndexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_index_buffer_data), g_index_buffer_data, GL_STATIC_DRAW);
		glBindVertexArray(0);

	}



	void render()
	{
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (Moving)
		{
			//set up to render to buffer
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuf[0]);
			glfwGetCursorPos(windowManager->getHandle(), &pX, &pY);
			mouseDx = pX - prevX;
			mouseDy = pY - prevY;

			prevX = pX;
			prevY = pY;

			mouseDx *= scale;
			mouseDy *= scale;

			theta += mouseDx;
			phi += mouseDy;

			target.x = cos(theta) * cos(phi);
			target.y = sin(phi);
			target.z = cos(phi) * cos(M_PI / 2 - theta);
			
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}


		/* Leave this code to just draw the meshes alone */
		float aspect = width/(float)height;

		// Create tmakehe matrix stacks
		auto P = make_shared<MatrixStack>();
		auto M = make_shared<MatrixStack>();
		auto V = make_shared<MatrixStack>();
		//time = glfwGetTime();
		time += 0.05;
		// if (time - lastTime > 3) {
		// 	cout << "time" << endl;
		// 	lastTime = time;
		// 	//glClearColor(.12f, .34f, .56f, 1.0f);

		// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// }			glLineWidth(3);

		sphere->bind();
		glUniform1f(sphere->getUniform("Time"), (GLfloat)time);

		// Apply perspective projection.
		P->pushMatrix();
			P->perspective(70.0f, aspect, 0.01f, 100.0f);
			glUniformMatrix4fv(sphere->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
		P->popMatrix();

		// //Draw our scene - two meshes - right now to a texture
		
		//glEnable(GL_PROGRAM_POINT_SIZE);
		//glEnable(GL_LINE_STIPPLE);
		glEnable(GL_DEPTH_TEST);
		
		V->pushMatrix();
			V->loadIdentity();
			V->lookAt(eyeVec, eyeVec + target, up);
			glUniformMatrix4fv(sphere->getUniform("V"), 1, GL_FALSE,value_ptr(V->topMatrix()) );
		V->popMatrix();	

		M->pushMatrix();
			M->loadIdentity();
			//M->rotate(radians(cTheta), vec3(0, 1, 0));
			M->translate(vec3(0,0,0));
			// M->scale(2);
			glUniformMatrix4fv(sphere->getUniform("M"), 1, GL_FALSE,value_ptr(M->topMatrix()) );	
			glBindVertexArray(VertexArrayID);	
			// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_LINES, 0, NUM_SEGMENTS * 6 );
		M->popMatrix();
		
		sphere->unbind();
		glBindVertexArray(0);

	}


};

int main(int argc, char **argv)
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
			resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(512, 512);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
			// Render scene.
			application->render();

			// Swap front and back buffers.
			glfwSwapBuffers(windowManager->getHandle());
			// Poll for and process events.
			glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
