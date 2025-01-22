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




    sf::Window window(sf::VideoMode(800, 600), "OpenGL Chunk", sf::Style::Default, contextSettings);
    window.setActive(true);
    window.setMouseCursorGrabbed(true);
    window.setMouseCursorVisible(false);

    // Inicjalizacja OpenGL przez GLAD
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Ustawienia OpenGL
    glViewport(0, 0, static_cast<GLsizei>(window.getSize().x),
        static_cast<GLsizei>(window.getSize().y));
    glEnable(GL_DEPTH_TEST);

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f),
        -90.0f, 0.0f);

    // Tworzenie shaderów
    ShaderProgram shaders;
    


	shaders.Use();
    
	CubePalette palette;
	PerlinNoise perlin;

	const size_t chunkSize = 16;
	Chunk<chunkSize, chunkSize, chunkSize> chunk(glm::vec2(0.0f, 0.0f), palette);
	chunk.Generate(perlin);

    Ray::HitType hitType;
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
    sf::Clock clock;
    sf::Vector2i windowCenter(window.getSize().x / 2, window.getSize().y / 2);
    sf::Vector2i lastMousePosition = sf::Mouse::getPosition(window);

	

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        // Obs³uga zdarzeñ
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::Resized)
                glViewport(0, 0, event.size.width, event.size.height);
        }




        if (event.type == sf::Event::MouseButtonPressed) {
            Ray ray(camera.GetPosition(), camera.GetFront());
            Ray::HitType hitType = chunk.Hit(ray, 0.0f, 3.0f, hitRecord); // Ustaw max na wiêksz¹ wartoœæ

            if (hitType == Ray::HitType::Hit) {
                std::cout << "Hit at: " << hitRecord.m_cubeIndex.x << ", "
                    << hitRecord.m_cubeIndex.y << ", "
                    << hitRecord.m_cubeIndex.z << std::endl;

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


        // Wyœwietlanie okna
        window.display();
    }

    return 0;
}

