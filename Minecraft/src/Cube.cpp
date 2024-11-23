#include "Cube.h"
#include <iostream>
#include <SFML/Graphics.hpp>

std::array<float, 6 * 6 * 5> Cube::s_vertices = {
       //x       y     z       u     v
		-0.5f, -0.5f, -0.5f,  0.25f, 0.0f,      // lewy dolny (UV 0.25, 0)
		 0.5f, -0.5f, -0.5f,  0.5f, 0.0f,    // prawy dolny (UV 0.5, 0)
		 0.5f,  0.5f, -0.5f,  0.5f, 1.0f/3.0f,   // prawy górny (UV 0.5, 1/3)
		 0.5f,  0.5f, -0.5f,  0.5f, 1.0f/3.0f,   // prawy górny (UV  0.5, 1/3)
		-0.5f,  0.5f, -0.5f,  0.25f, 1.0f/3.0f,     // lewy górny (UV 0.25, 1/3)
		-0.5f, -0.5f, -0.5f,  0.25f, 0.0f,		// lewy dolny (UV 0.25, 0)

        -0.5f, -0.5f,  0.5f,  0.25f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.5f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.5f, 2.0f / 3.0f,
		 0.5f,  0.5f,  0.5f,  0.5f, 2.0f / 3.0f,
		-0.5f,  0.5f,  0.5f,  0.25f, 2.0f / 3.0f,
		-0.5f, -0.5f,  0.5f,  0.25f, 1.f,

        -0.5f,  0.5f,  0.5f,  0.5f, 1.0f/3.0f,      // lewy dolny 
		-0.5f,  0.5f, -0.5f,  0.5f, 2.0f/3.0f,    // prawy dolny 
		-0.5f, -0.5f, -0.5f,  0.75f, 2.0f/3.0f,   // prawy górny 
		-0.5f, -0.5f, -0.5f,  0.75f, 2.0f/3.0f,   // prawy górny 
		-0.5f, -0.5f,  0.5f,  0.75f, 1.0f/3.0f,     // lewy górny 
		-0.5f,  0.5f,  0.5f,  0.5f, 1.0f/3.0f,		// lewy dolny 

		0.5f,  0.5f,  0.5f,  0.25f, 1.0f / 3.0f,	// lewy dolny 
		0.5f,  0.5f, -0.5f,  0.25f, 2.0f / 3.0f,	// prawy dolny
		0.5f, -0.5f, -0.5f,  0.0f, 2.0f / 3.0f,	// prawy górny 
		0.5f, -0.5f, -0.5f,  0.0f, 2.0f / 3.0f,	// prawy górny 
		0.5f, -0.5f,  0.5f,  0.0f, 1.0f / 3.0f,	// lewy górny 
		0.5f,  0.5f,  0.5f,  0.25f, 1.0f / 3.0f,	// lewy dolny

        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f / 3.0f,	// lewy dolny 
		 0.5f, -0.5f, -0.5f,  1.0f, 2.0f / 3.0f,	// prawy dolny
		 0.5f, -0.5f,  0.5f,  0.75f, 2.0f / 3.0f,	// prawy górny 
		 0.5f, -0.5f,  0.5f,  0.75f, 2.0f / 3.0f,	// prawy górny 
		-0.5f, -0.5f,  0.5f,  0.75f, 1.0f / 3.0f,	// lewy górny 
		-0.5f, -0.5f, -0.5f,  1.0f, 1.0f / 3.0f,	// lewy dolny

        -0.5f,  0.5f, -0.5f,  0.25f, 1.0f / 3.0f,
         0.5f,  0.5f, -0.5f,  0.5f, 1.0f / 3.0f,
         0.5f,  0.5f,  0.5f,  0.5f, 2.0f / 3.0f,
         0.5f,  0.5f,  0.5f,  0.5f, 2.0f / 3.0f,
        -0.5f,  0.5f,  0.5f,  0.25f, 2.0f / 3.0f,
        -0.5f,  0.5f, -0.5f,  0.25f, 1.0f / 3.0f
};

extern GLuint CreateTexture(const std::string& path); // Deklaracja funkcji, ¿eby mo¿na by³o jej u¿yæ

Cube::Cube(const std::string& texturePath) {
	// Tworzenie i konfiguracja VAO i VBO
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, s_vertices.size() * sizeof(float), s_vertices.data(), GL_STATIC_DRAW);

	// Atrybuty wierzcho³ków
	// Pozycje (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Teksturowanie (u, v)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// £adowanie tekstury za pomoc¹ funkcji CreateTexture
	m_texture = CreateTexture(texturePath);
	if (m_texture == 0) {
		std::cerr << "Failed to load texture: " << texturePath << std::endl;
	}
}

Cube::~Cube() {
	if (m_texture) glDeleteTextures(1, &m_texture);
	if (m_vbo) glDeleteBuffers(1, &m_vbo);
	if (m_vao) glDeleteVertexArrays(1, &m_vao);
}

Cube::Cube(Cube&& rhs) noexcept
	: m_vbo(std::exchange(rhs.m_vbo, 0))
	, m_vao(std::exchange(rhs.m_vao, 0))
	, m_texture(std::exchange(rhs.m_texture, 0)) {
}

Cube& Cube::operator=(Cube&& rhs) noexcept {
	if (&rhs == this) {
		return *this;
	}

	m_vbo = std::exchange(rhs.m_vbo, 0);
	m_vao = std::exchange(rhs.m_vao, 0);
	m_texture = std::exchange(rhs.m_texture, 0);

	return *this;
}

void Cube::Render() {
	glBindVertexArray(m_vao); // Przypisanie VAO szeœcianu
	glBindTexture(GL_TEXTURE_2D, m_texture); // Przypisanie tekstury
	glDrawArrays(GL_TRIANGLES, 0, 36); // Rysowanie szeœcianu
}
