#ifndef RENDERER_H_
#define RENDERER_H_

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

constexpr int resolution = 2;

constexpr int GRID_HEIGHT = 250;
constexpr int GRID_WIDTH = 250;
constexpr int canvW = GRID_WIDTH*resolution;
constexpr int canvH = GRID_HEIGHT*resolution;

extern cv::Mat colorFld;
extern const int resolution;


void update(); //function thats calls every calculations
void render(); //Render function
void initRender(); //Set up window

#endif /* RENDERER_H_ */
