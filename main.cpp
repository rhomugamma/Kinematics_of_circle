#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>

const float PI = 3.14159265358979323;

GLuint VAO;

GLuint shaderProgram;

float radius = 0.01;
const int iterations = 20;
float beta = 360.0f / iterations * (2 * PI / 360.0f);
float alpha = beta;
GLfloat vertices[6 * iterations];													
GLfloat colors[9 * iterations];

float deltatime = 0.0;
float frametime = 0.0;

float ObjectPositionX = -1.0;
float ObjectPositionY = -1.0;
float ObjectPositionZ = 0.0;

float ObjectVelocityX = 0.0;
float ObjectVelocityY = 0.0;
float ObjectVelocityZ = 0.0;

float ObjectAccelerationX = 0.5;
float ObjectAccelerationY = 0.5;

const GLchar* vertexShaderSource = R"(

    #version 450 core

    layout (location = 0) in vec3 position;
	layout (location = 1) in vec3 color;

	out vec3 vertexColor;

    void main() {

		vertexColor = color;
        gl_Position = vec4(position.x, position.y, 0.0, 1.0);

    }

)";

const GLchar* fragmentShaderSource = R"(

    #version 450 core

	in vec3 vertexColor;
	out vec4 fragColor;

    void main() {

        fragColor = vec4(vertexColor, 1.0f);

    }

)";


void init();

void display();

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

void rendersphere(float radius1, int iterations);

void updateobjectposition();


int main() {

    if (!glfwInit()) {

        std::cout << "GLFW initialization failed!" << std::endl;
        return -1;
    
	}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dynamic Window", nullptr, nullptr);

    if (!window) {

        std::cout << "GLFW window creation failed!" << std::endl;
        glfwTerminate();
        return -1;

    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
		 
        std::cout << "GLEW initialization failed!" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    
	}

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    init();

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();
		updateobjectposition();
        display();
        glfwSwapBuffers(window);
    
	}

    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();

    return 0;
}

void init() {

	rendersphere(radius, iterations);

   }

void display() {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);

    glUseProgram(shaderProgram);

	GLuint vertexVBO;
	glGenBuffers(1, &vertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3 * iterations);
	glDrawArrays(GL_TRIANGLES, 0, 3 * iterations);

    glBindVertexArray(0);

}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {

    glViewport(0, 0, width, height);

}

void rendersphere(float radius, int iterations) {

	glGenVertexArrays(1, &VAO);														
    glBindVertexArray(VAO);

    for (int i = 0; i < 6 * iterations; i++) {
        
		vertices[i] = 0;
        vertices[i + 1] = 0;

        vertices[i + 2] = radius * cos(beta);
        vertices[i + 3] = radius * sin(beta);

        vertices[i + 4] = radius * cos(beta + alpha);
        vertices[i + 5] = radius * sin(beta + alpha);

        i = i + 5;
        beta = beta + alpha;
    
	};

    for (int i = 0; i < 9 * iterations; i += 3) {

        colors[i] = 1.0;
        colors[i + 1] = 0.0;
        colors[i + 2] = 0.0;
    
	}

    GLuint vertexVBO;															
    glGenBuffers(1, &vertexVBO);													
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);										
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    GLuint colorVBO;
    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
 
}

void updateobjectposition() {

	GLfloat totalTime = glfwGetTime();
	deltatime = totalTime - frametime;
	frametime = totalTime;

	ObjectPositionX = (ObjectPositionX) + (ObjectVelocityX * deltatime) + ((ObjectAccelerationX * deltatime * deltatime)/2);
	ObjectPositionY = (ObjectPositionY) + (ObjectVelocityY * deltatime) + ((ObjectAccelerationY * deltatime * deltatime)/2);

	ObjectVelocityX = (ObjectVelocityX) + (ObjectAccelerationX * deltatime);
	ObjectVelocityY = (ObjectVelocityY) + (ObjectAccelerationY * deltatime);

	for(int i = 0; i < 6*iterations; i++) {

		vertices[i] = 0 + ObjectPositionX;
        vertices[i + 1] = 0 + ObjectPositionY;

        vertices[i + 2] = radius * cos(beta) + ObjectPositionX;
        vertices[i + 3] = radius * sin(beta) + ObjectPositionY;

        vertices[i + 4] = radius * cos(beta + alpha) + ObjectPositionX;
        vertices[i + 5] = radius * sin(beta + alpha) + ObjectPositionY;

        i = i + 5;

        beta = beta + alpha;
	
	}
	
}
