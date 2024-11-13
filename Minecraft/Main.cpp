#include <SFML/Window.hpp>
#include <GLAD/glad.h>
#include <iostream>



// Vertex shader
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 position;
void main() {
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
}
)";

// Fragment shader
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 fragmentColor;
void main() {
    fragmentColor = vec4(1.0, 0.5, 0.2, 1.0);
}
)";

// Kompilacja shaderów
GLuint CreateShader(const GLchar *shaderSource, GLenum shaderType) { 
    const GLuint shaderId = glCreateShader(shaderType); 
    if(!shaderId) { 
        std::cerr << "Error creating shader! (shaderId do not exists)" << std::endl;
        return 0;   // null handle 
    } 
     
    glShaderSource(shaderId, 1, &shaderSource, nullptr); 
    glCompileShader(shaderId); 
     
    // error handling 
    GLint success;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        std::cerr << "Shader compilation failed" << std::endl;
        return 0;
    }
     
    return shaderId; 
}

// Linkowanie programu
GLuint CreateProgram(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader = 0) 
{ 
    const GLuint programId = glCreateProgram(); 
    if(!programId) { 
        std::cerr << "Error creating shader program " << std::endl;
        return 0;   // null handle 
    } 
     
    glAttachShader(programId, vertexShader); 
    glAttachShader(programId, fragmentShader); 
    if(geometryShader) glAttachShader(programId, geometryShader); 
     
    glLinkProgram(programId); 
     
    return programId; 
}

// Vertex Buffer Object
std::pair<GLuint, GLuint> CreateVertexBufferObject() {
    const float triangle[] = {
        //  x      y      z 
       -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    GLuint vbo, vao;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return std::make_pair(vbo, vao);
}

int main() {
    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 24;
    contextSettings.sRgbCapable = false;
    contextSettings.majorVersion = 3;
    contextSettings.minorVersion = 3;

    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, contextSettings);
    window.setActive(true);

    gladLoadGL();
    glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));

    // Create shaders
    GLuint vertexShaderId = CreateShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShaderId = CreateShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    // Link shaders 
    GLuint programId = CreateProgram(vertexShaderId, fragmentShaderId);
    // Create vbo & vao 
    auto [vbo, vao] = CreateVertexBufferObject();

    bool running = 1;
    while (running) {
        //Events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running = false;
            }
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(programId);
        glBindVertexArray(vao);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        window.display();
    }

    // Clean-up
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(programId);
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    window.close();
    return 0;
}
