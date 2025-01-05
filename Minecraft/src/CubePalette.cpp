#include "CubePalette.h"
#include "Cube.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <ShaderProgram.h>



CubePalette::CubePalette()
{
	Cube grass("assets/blocks/grass.jpg");
	Cube stone("assets/blocks/stone.jpg");
	Cube grass_debug("assets/blocks/grass_debug.jpg");
	m_palette.insert(std::pair<Cube::Type, Cube>(Cube::Type::Stone, std::move(stone)));
	m_palette.insert(std::pair<Cube::Type, Cube>(Cube::Type::Grass, std::move(grass)));
	m_palette.insert(std::pair<Cube::Type, Cube>(Cube::Type::GrassDebug, std::move(grass_debug)));
	/*
	m_palette[Cube::Type::Grass] = Cube("assets/blocks/grass.png");
	m_palette[Cube::Type::Stone] = Cube("assets/blocks/stone.png");
	m_palette[Cube::Type::GrassDebug] = Cube("assets/blocks/grass_debug.png");
	*/

}



const Cube& CubePalette::LookUp(Cube::Type type) const
{
	return m_palette.at(type);
	// TODO: tu wstawi� instrukcj� return
	
	
}
