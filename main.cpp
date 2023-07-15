#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>

// Vertex shader source code
const GLchar* vertexShaderSource = R"(
    #version 450 core
    layout (location = 0) in vec3 position;
	layout (location = 1) in vec3 color;
	out vec3 vertexColor;
    void main()
    {
		vertexColor = color;
        gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    }
)";

// Fragment shader source code
const GLchar* fragmentShaderSource = R"(
    #version 450 core
	in vec3 vertexColor;
	out vec4 fragColor;
    void main()
    {
        fragColor = vec4(vertexColor, 1.0f);
    }
)";

const float PI = 3.14159265358979323;

// Vertex array object (VAO) is an object that stores the configuration of vertex data for rendering
GLuint VAO;

// Shader program. A shader program is an object capable of combining shaders
GLuint shaderProgram;


//Object physical kinematic characteristics
float ObjectPositionX = -1.0;
float ObjectPositionY = -1.0;

float ObjectVelocityX = 0.0;
float ObjectVelocityY = 0.0;

float ObjectAccelerationX = 0.5;
float ObjectAccelerationY = 0.5;


//Object characteristics
float radius = 0.01;
const int niterations = 20;
float beta = 360.0f / niterations * (2 * PI / 360.0f);
float alpha = beta;
GLfloat vertices[6 * niterations];													
GLfloat colors[9 * niterations];

//Time characteristics
float deltatime = 0.0;
float frametime = 0.0;


// Initialize OpenGL state
void init() {

    // Create vertex array object (VAO)
    glGenVertexArrays(1, &VAO);														
    glBindVertexArray(VAO);

    for (int i = 0; i < 6 * niterations; i++) {
        vertices[i] = 0;
        vertices[i + 1] = 0;

        vertices[i + 2] = radius * cos(beta);
        vertices[i + 3] = radius * sin(beta);

        vertices[i + 4] = radius * cos(beta + alpha);
        vertices[i + 5] = radius * sin(beta + alpha);

        i = i + 5;
        beta = beta + alpha;
    };

    for (int i = 0; i < 9 * niterations; i += 3) {

        colors[i] = 1.0;
        colors[i + 1] = 0.0;
        colors[i + 2] = 0.0;
    
	}

    // Create vertex buffer object (VBO) for vertices
    GLuint vertexVBO;															
    glGenBuffers(1, &vertexVBO);													
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);										
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Specify vertex attribute pointers for vertices
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Create vertex buffer object (VBO) for colors
    GLuint colorVBO;
    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    // Specify vertex attribute pointers for colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);

    // Unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Render the scene
void display() {

    // Clear the color buffer
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);

    // Use the shader program
    glUseProgram(shaderProgram);

	//Update the vertex buffer data
	GLuint vertexVBO;
	glGenBuffers(1, &vertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Specify vertex attribute pointers for vertices
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3 * niterations);

    glBindVertexArray(0);
}

// GLFW framebuffer size callback function
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // Update viewport dimensions
    glViewport(0, 0, width, height);
}

void updateobjectposition() {

	//Update time
	GLfloat totalTime = glfwGetTime();
	deltatime = totalTime - frametime;
	frametime = totalTime;

	//Update position
	ObjectPositionX = (ObjectPositionX) + (ObjectVelocityX * deltatime) + ((ObjectAccelerationX * deltatime * deltatime)/2);
	ObjectPositionY = (ObjectPositionY) + (ObjectVelocityY * deltatime) + ((ObjectAccelerationY * deltatime * deltatime)/2);

	//Update velocity
	ObjectVelocityX = (ObjectVelocityX) + (ObjectAccelerationX * deltatime);
	ObjectVelocityY = (ObjectVelocityY) + (ObjectAccelerationY * deltatime);

	for(int i = 0; i < 6*niterations; i++) {

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

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cout << "GLFW initialization failed!" << std::endl;
        return -1;
    }

    // Set GLFW options
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dynamic Window", nullptr, nullptr);
    if (!window)
    {
        std::cout << "GLFW window creation failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the created window the current context
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW initialization failed!" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Set GLFW callback functions
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Create and compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Create and compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Create shader program and link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Clean up shader objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Call initialization function
    init();

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
		updateobjectposition();
        display();
        glfwSwapBuffers(window);
    }

    // Clean up resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);

    // Terminate GLFW
    glfwTerminate();

    return 0;
}
