#include <SFML/Graphics.hpp>
#include <GLAD/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Cube.h>
#include <ShaderProgram.h>
#include <Camera.h>
#include <CubePalette.h>
#include <Chunk.h>
#include <ChunkManager.cpp>


const char* crosshairVertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;

    out vec2 TexCoord;

    void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
        TexCoord = aTexCoord;
    }
)";

const char* crosshairFragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;

    in vec2 TexCoord;

    uniform sampler2D crosshairTexture;

    void main() {
        FragColor = texture(crosshairTexture, TexCoord);
    }
)";


float crosshairVertices[] = {
    // Positions      // Texture Coords
    -0.05f, -0.05f,   0.0f, 0.0f, // Left-bottom
     0.05f, -0.05f,   1.0f, 0.0f, // Right-bottom
     0.05f,  0.05f,   1.0f, 1.0f, // Right-top
    -0.05f,  0.05f,   0.0f, 1.0f  // Left-top
};

unsigned int crosshairIndices[] = {
    0, 1, 2,
    2, 3, 0
};


// Linkowanie programu
GLuint CreateProgram(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader = 0)
{
    const GLuint programId = glCreateProgram();
    if (!programId) {
        std::cerr << "Error creating shader program " << std::endl;
        return 0;   // null handle 
    }

    glAttachShader(programId, vertexShader);
    glAttachShader(programId, fragmentShader);
    if (geometryShader) glAttachShader(programId, geometryShader);

    glLinkProgram(programId);

    return programId;
}

// Kompilacja shaderów
GLuint CreateShader(const GLchar* shaderSource, GLenum shaderType) {
    const GLuint shaderId = glCreateShader(shaderType);
    if (!shaderId) {
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

// Tworzenie tekstury
GLuint CreateTexture(const std::string& path) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    sf::Image image;
    if (image.loadFromFile(path)) {
        image.flipVertically();
        const sf::Vector2u size = image.getSize();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }

    return texture;
}





int main() {

    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 24;
	contextSettings.stencilBits = 8;
    contextSettings.majorVersion = 3;
    contextSettings.minorVersion = 3;




    sf::Window window(sf::VideoMode(1000, 800), "Minecraft alpha", sf::Style::Default, contextSettings);
    window.setActive(true);
    window.setMouseCursorGrabbed(true);
    window.setMouseCursorVisible(false);

    // Inicjalizacja OpenGL przez GLAD
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Ustawienia OpenGL
    glViewport(0, 0, static_cast<GLsizei>(window.getSize().x),
        static_cast<GLsizei>(window.getSize().y));
    glEnable(GL_DEPTH_TEST);

    Camera camera(glm::vec3(9.0f, 3.0f, 6.0f), glm::vec3(0.0f, 0.0f, -1.0f),
        -90.0f, 0.0f);

    // Tworzenie shaderów
    ShaderProgram shaders;
    


	shaders.Use();
    
	CubePalette palette;
	PerlinNoise perlin;

	const size_t chunkSize = 16;
	Chunk<chunkSize, chunkSize, chunkSize> chunk(glm::vec2(0.0f, 0.0f), palette);
	chunk.Generate(perlin);

    //Ray::HitType hitType;
	Chunk<chunkSize, chunkSize, chunkSize>::HitRecord hitRecord;

   


    // Tworzenie kamery
    glm::vec3 initialPosition(0.0f, 0.0f, 3.0f);
    glm::vec3 initialFront(0.0f, 0.0f, -1.0f);
    float initialYaw = -90.0f;
    float initialPitch = 0.0f;
    //Camera camera(initialPosition, initialFront, initialYaw, initialPitch);

    /*
    // Tworzenie tekstury    // Ustawienie tekstury
	GLuint grassTexture = CreateTexture("assets/blocks/grass_debug.jpg");
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    


    // Tworzenie obiektu Cube
    Cube cube("assets/blocks/grass_debug.jpg");
    */



    // Pêtla renderuj¹ca

    // Tworzenie tekstury celownika
    GLuint crosshairTexture = CreateTexture("assets/crosshair/crosshair.png");

    GLuint crosshairVAO, crosshairVBO, crosshairEBO;
    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);
    glGenBuffers(1, &crosshairEBO);

    glBindVertexArray(crosshairVAO);

    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, crosshairEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(crosshairIndices), crosshairIndices, GL_STATIC_DRAW);

    // Pozycje wierzcho³ków
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Wspó³rzêdne tekstur
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);


    GLuint crosshairVertexShader = CreateShader(crosshairVertexShaderSource, GL_VERTEX_SHADER);
    GLuint crosshairFragmentShader = CreateShader(crosshairFragmentShaderSource, GL_FRAGMENT_SHADER);
    GLuint crosshairProgram = CreateProgram(crosshairVertexShader, crosshairFragmentShader);

    // Usuñ pojedyncze shadery po stworzeniu programu
    glDeleteShader(crosshairVertexShader);
    glDeleteShader(crosshairFragmentShader);



    sf::Clock clock;
    sf::Vector2i windowCenter(window.getSize().x / 2, window.getSize().y / 2);
    sf::Vector2i lastMousePosition = sf::Mouse::getPosition(window);

	
    bool isMousePressed = false; // Zmienna stanu klikniêcia

    
    ChunkManager chunkManager(palette, perlin);

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        // Obs³uga zdarzeñ
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::Resized)
                glViewport(0, 0, event.size.width, event.size.height);

            if (event.type == sf::Event::MouseButtonPressed) {
                Ray ray(camera.GetPosition(), camera.GetFront());
                Ray::HitType hitType = chunk.Hit(ray, 0.0f, 3.0f, hitRecord);
                if (hitType == Ray::HitType::Hit) {
                    std::cout << "Hit block at: (" << hitRecord.m_cubeIndex.x << ", "
                        << hitRecord.m_cubeIndex.y << ", "
                        << hitRecord.m_cubeIndex.z << ")" << std::endl;
                    // Sprawdzenie klikniêcia myszy
                    if (event.type == sf::Event::MouseButtonPressed && !isMousePressed) {
                        isMousePressed = true; // Rejestruj, ¿e przycisk zosta³ wciœniêty

                        Ray ray(camera.GetPosition(), camera.GetFront());
                        Ray::HitType hitType = chunk.Hit(ray, 0.0f, 3.0f, hitRecord);

                        if (hitType == Ray::HitType::Hit) {
                            if (event.mouseButton.button == sf::Mouse::Left) {
                                chunk.RemoveBlock(hitRecord.m_cubeIndex.x,
                                    hitRecord.m_cubeIndex.y,
                                    hitRecord.m_cubeIndex.z);
                            }
                            else if (event.mouseButton.button == sf::Mouse::Right) {
                                chunk.PlaceBlock(hitRecord.m_neighbourIndex.x,
                                    hitRecord.m_neighbourIndex.y,
                                    hitRecord.m_neighbourIndex.z,
                                    Cube::Type::Grass);
                            }
                        }
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                isMousePressed = false; // Przywrócenie stanu klikniêcia
            }
        }




		float movementSpeed = 0.025f;
        // Obs³uga klawiatury
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) camera.MoveForward(dt + movementSpeed);
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W)) && (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))) camera.MoveForward(dt + movementSpeed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) camera.MoveBackward(dt + movementSpeed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) camera.MoveLeft(dt + movementSpeed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) camera.MoveRight(dt + movementSpeed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) camera.MoveUp(dt + movementSpeed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) camera.MoveDown(dt + movementSpeed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close(); //Escape do zamkniêcia
        //if (sf::Mouse::isButtonPressed(sf::Mouse::Left));
        

        // Obs³uga myszy
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        sf::Vector2i mouseDelta = mousePosition - lastMousePosition;
        camera.Rotate(mouseDelta);
        lastMousePosition = mousePosition;

        // Czyszczenie ekranu
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Ustawienie shaderów i macierzy
        

        shaders.setUniform("view", camera.View());
        shaders.setUniform("projection", camera.Projection());

        glDrawArrays(GL_LINES, 0, 2);

        // Renderowanie szeœcianu
        //cube.Draw();
		chunk.Draw(shaders);
        chunkManager.Update(camera.GetPosition());
		chunkManager.Draw(shaders);

        // Renderowanie celownika
        glUseProgram(crosshairProgram); // U¿ycie shaderów celownika
        glBindVertexArray(crosshairVAO);
        glBindTexture(GL_TEXTURE_2D, crosshairTexture);

        // Wy³¹cz test g³êbokoœci, aby celownik zawsze by³ widoczny
        glDisable(GL_DEPTH_TEST);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glEnable(GL_DEPTH_TEST); // Przywrócenie testu g³êbokoœci
		 // Wy³¹czenie programu celownika
        glUseProgram(shaders.GetProgramId());
        shaders.setUniform("view", camera.View());
        shaders.setUniform("projection", camera.Projection());

        // Wyœwietlanie okna
        window.display();
    }

    return 0;
}

