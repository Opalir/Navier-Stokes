/*
 * Renderer.h
 *
 *  Created on: 25 oct. 2020
 *      Author: Thomas
 */

#ifndef RENDERER_H_
#define RENDERER_H_

#include <SFML/Graphics.hpp>

extern sf::RenderWindow window;
constexpr int resolution = 2;
constexpr int canvW  = 500;
constexpr int canvH = 500;
constexpr int GRID_HEIGHT = canvH/resolution;
constexpr int GRID_WIDTH = canvW/resolution;

extern float colorField[GRID_WIDTH][GRID_HEIGHT];
extern float fieldx[GRID_WIDTH][GRID_HEIGHT];
extern float fieldy[GRID_WIDTH][GRID_HEIGHT];
extern const int resolution;


void update(); //function thats calls every calculations
void render(); //Render function
void initRender(); //Set up window

#endif /* RENDERER_H_ */
