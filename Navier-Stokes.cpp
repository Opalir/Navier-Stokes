/*
 * Navier-Stokes simulation for incompressible fluid
 * C++ using opencv.
 * Every calculations are done with matrix
 *
 *      Author: Opalir
 */


#include <iostream>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "Renderer.h"

using namespace std;


const float mouseAdd = 0.5;
const float delta =0.1;
const float rho = 1;

bool mousePressed = false;
int mouseButton = 0;

int mouseX,mouseY,lastmouseY,lastmouseX;

const float pressCoef = -((2*rho)/delta); //attention div
const float correctCoef = delta/(2*rho);

cv::Mat meshGridX(GRID_HEIGHT, GRID_WIDTH, CV_32F);
cv::Mat meshGridY(GRID_HEIGHT, GRID_WIDTH, CV_32F);

cv::Mat colorFld = cv::Mat::zeros(cv::Size(GRID_WIDTH, GRID_HEIGHT), CV_32F);

cv::Mat fldx = cv::Mat::zeros(cv::Size(GRID_WIDTH, GRID_HEIGHT), CV_32F);
cv::Mat fldy= cv::Mat::zeros(cv::Size(GRID_WIDTH, GRID_HEIGHT), CV_32F);

cv::Mat pressureFld= cv::Mat::zeros(cv::Size(GRID_WIDTH, GRID_HEIGHT), CV_32F);


float kerDatax[1][3] = {-1,0,1};
cv::Mat ker1(cv::Size(3,1),CV_32F,kerDatax);
cv::Mat ker2(cv::Size(1,3),CV_32F,kerDatax);

float kerData2[5][5] = {{0,0,1,0,0},{0,0,0,0,0},{1,0,0,0,1},{0,0,0,0,0},{0,0,1,0,0}};
cv::Mat ker3(cv::Size(5,5),CV_32F,kerData2);

const int jacobiIterations = 10;

void advect(){
    cv::Mat mapX_x = meshGridX - fldx * delta;
    cv::Mat mapX_y = meshGridY - fldy * delta;

    cv::Mat dstX(cv::Size(GRID_WIDTH, GRID_HEIGHT), CV_32F);
    cv::Mat dstY(cv::Size(GRID_WIDTH, GRID_HEIGHT), CV_32F);

    cv::remap(fldx, dstX, mapX_x, mapX_y, cv::INTER_LINEAR, cv::BORDER_CONSTANT); //Interpolation bilinéaire
    cv::remap(fldy, dstY, mapX_x, mapX_y, cv::INTER_LINEAR, cv::BORDER_CONSTANT); //Interpolation bilinéaire

    fldx = dstX;
    fldy = dstY;
}
void calcPressure(){
	cv::Mat dstDivx = cv::Mat(cv::Size(GRID_WIDTH,GRID_HEIGHT),CV_32F);
	cv::Mat dstDivy = cv::Mat(cv::Size(GRID_WIDTH,GRID_HEIGHT),CV_32F);
	cv::filter2D(fldx,dstDivx,-1,ker1); //Convolution : Noyau ker1   => A[x][y] = A[x+1][y]-A[x-1][y]
	cv::filter2D(fldy,dstDivy,-1,ker2); //Convolution : Noyau ker2   => A[x][y] = A[x][y+1]-A[x][y-1]

	cv::Mat divFld = pressCoef*(dstDivx+dstDivy);

	cv::Mat convolutedPressureFld = cv::Mat(cv::Size(GRID_WIDTH,GRID_HEIGHT),CV_32F);

	for(int i=0;i<jacobiIterations;i++){
		cv::filter2D(pressureFld,convolutedPressureFld,-1,ker3,cv::Point(-1,-1),0,cv::BORDER_CONSTANT); //Convolution : Noyau ker3   => A[x][y] = A[x+2][y]+A[x-2][y]+A[x][y+2]+A[x][y-2]
		pressureFld = (divFld + convolutedPressureFld)/4;
	}
}
void correctField(){
	cv::Mat dstPressx = cv::Mat(cv::Size(GRID_WIDTH,GRID_HEIGHT),CV_32F);
	cv::Mat dstPressy = cv::Mat(cv::Size(GRID_WIDTH,GRID_HEIGHT),CV_32F);

	cv::filter2D(pressureFld,dstPressx,-1,ker1); //convolution
	cv::filter2D(pressureFld,dstPressy,-1,ker2); //convolution

	fldx= fldx - correctCoef*dstPressx;
	fldy= fldy - correctCoef*dstPressy;
}
void advectColor(){
	cv::Mat mapX_x = meshGridX - fldx * delta;
	cv::Mat mapX_y = meshGridY - fldy * delta;
	cv::Mat src = colorFld;
	cv::remap(src,colorFld, mapX_x, mapX_y, cv::INTER_LINEAR, cv::BORDER_CONSTANT);
}
void computeBounds(){
	//Inutile ?
	fldy.row(1) = -fldy.row(2);
	fldx.row(1) = cv::Mat::zeros(1,GRID_WIDTH,CV_32F);
	fldy.row(GRID_WIDTH-2) = -fldy.row(GRID_WIDTH-3);
	fldx.row(GRID_WIDTH-2) =cv::Mat::zeros(1,GRID_WIDTH,CV_32F);

	fldy.col(1) = cv::Mat::zeros(GRID_HEIGHT,1,CV_32F);
	fldx.col(1) = -fldx.col(2);
	fldy.col(GRID_HEIGHT-2) = cv::Mat::zeros(GRID_HEIGHT,1,CV_32F);
	fldx.col(GRID_HEIGHT-2) = -fldx.col(GRID_HEIGHT-3);
}
void getInputs(){
	if(mousePressed){
		int depx = mouseX-lastmouseX;
		int depy = mouseY-lastmouseY;

		cv::circle(fldx,cv::Point(mouseX,mouseY),5,depx*7,-1);
		cv::circle(fldy,cv::Point(mouseX,mouseY),5,depy*7,-1);
		cv::circle(colorFld,cv::Point(mouseX,mouseY),5,1,-1);
	}
	lastmouseX = mouseX;
	lastmouseY = mouseY;

	/*  PASSIVE SOURCE ACTIVATION  (5 is source size,  10 is velocity,   1 is added color (from 0 to 1))
	 * cv::circle(fldx,cv::Point(150,50),5,10,-1);
	cv::circle(colorFld,cv::Point(150,50),5,1,-1);
	*/

}
void update(){
	clock_t debtime = clock();
	advect();
	std::cout<<"Advect :"<<(clock()-debtime)<<"ms";
	debtime = clock();
	calcPressure();
	std::cout<<"-Pressure :"<<(clock()-debtime)<<"ms";
	debtime = clock();
	correctField();
	std::cout<<"-Correct :"<<(clock()-debtime)<<"ms";
	debtime = clock();
	//computeBounds();
	std::cout<<"-Bounds :"<<(clock()-debtime)<<"ms";
	debtime = clock();
	advectColor();
	std::cout<<"-Color :"<<(clock()-debtime)<<"ms"<<endl;
	debtime = clock();

}
void mouse_callback(int  event, int  x, int  y, int  flag, void *param)
{
	if (event == cv::EVENT_LBUTTONDOWN ){
		mousePressed = true;
	}
	if (event == cv::EVENT_LBUTTONUP ){
		mousePressed = false;
	}
    if (event == cv::EVENT_MOUSEMOVE) {
		mouseX = x;
		mouseY = y;
    }
}
void initRender(){
	 cv::namedWindow("Navier-Stokes",cv::WINDOW_NORMAL);
	 cv::setMouseCallback("Navier-Stokes", mouse_callback);
	 cv::resizeWindow("Navier-Stokes", canvW,canvH);
}
int main()
{
    // Init meshgrid
    for (int i = 0; i < GRID_WIDTH; ++i) {
        for (int j = 0; j < GRID_HEIGHT; ++j) {
            meshGridX.at<float>(j,i) = i;
            meshGridY.at<float>(j,i) = j;
        }
    }

    initRender();

	while(true){
		getInputs();

		clock_t debtime = clock();
		update();
		std::cout<<"Update :"<<(clock()-debtime)<<"ms"<<endl;
		debtime = clock();
		render();
		std::cout<<"Render :"<<(clock()-debtime)<<"ms"<<endl;

		cv::waitKey(1); //wait 1ms and go to next frame
	}
    return 0;
}

