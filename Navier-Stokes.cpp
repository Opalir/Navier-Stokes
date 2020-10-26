//============================================================================
// Name        : Navier-Stokes.cpp
// Author      : Opalir
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

#include "Renderer.h"
using namespace std;


const int mouseAdd = 150;
sf::Vector2i lastMousePos(0,0);
const float delta =0.1;
const float rho = 1;

bool mousePressed = false;
int mouseButton = 0;

int mouseX,mouseY,lastmouseY,lastmouseX;


//int clock = 0;

float nextField1[GRID_WIDTH][GRID_HEIGHT] = {};
float pressureTab[GRID_WIDTH][GRID_HEIGHT] ={};

float fieldx[GRID_WIDTH][GRID_HEIGHT] = {};
float fieldy[GRID_WIDTH][GRID_HEIGHT] = {};
float nextField2x[GRID_WIDTH][GRID_HEIGHT] = {};
float nextField2y[GRID_WIDTH][GRID_HEIGHT] = {};

float divergenceTab[GRID_WIDTH][GRID_HEIGHT] = {};
float colorField[GRID_WIDTH][GRID_HEIGHT] = {};


const float pressCoef = -((2*rho)/delta); //attention div
const float correctCoef = delta/(2*rho);


const int jacobiIterations = 10;


float interpolateF(float point,int x0,float y0,int x1,float y1){ //avec des floats
	return ((y1-y0)*point+(y0*x1-x0*y1))/(x1-x0);
}
float interpolateI(float point,int x0,int y0,int x1,int y1){ //avec des ints
	return ((y1-y0)*point+(y0*x1-x0*y1))/(x1-x0);
}
void advect(){
	int tempsTotal = 0;
	//sf::Clock clock;

	for(int x=1;x<GRID_WIDTH-1;x++){
		for(int y=1;y<GRID_HEIGHT-1;y++){
				//Coordonnées du point recherché.
				float wantx = x-fieldx[x][y]*delta; //avec vitesse en x
				float wanty = y-fieldy[x][y]*delta; //avec vitesse en y


				//Calcul de la case qui entoure le point recherché
				int leftx = floor(wantx);
				int rightx = leftx+1;
				int upy = floor(wanty);
				int downy = upy+1;



				//Interpolation sur le segment du haut.
				float topx = interpolateF(wantx,leftx,fieldx[leftx][upy],rightx,fieldx[rightx][upy]); //avec la vitesse x
				float topy = interpolateF(wantx,leftx,fieldy[leftx][upy],rightx,fieldy[rightx][upy]); //avec la vitesse y

				//Interpolation sur le segment du bas.
				float botx = interpolateF(wantx,leftx,fieldx[leftx][downy],rightx,fieldx[rightx][downy]); //avec la vitesse x
				float boty = interpolateF(wantx,leftx,fieldy[leftx][downy],rightx,fieldy[rightx][downy]); //avec la vitesse y


				//Interpolation de haut en bas à partir des 2 trouvées + haut.
				float middlex = interpolateF(wanty,upy,topx,downy,botx); //avec la vitesse x
				float middley = interpolateF(wanty,upy,topy,downy,boty); //avec la vitesse y


				//tempsTotal+=clock.getElapsedTime().asMicroseconds(); //Ajout du temps total
				//clock.restart(); //restart de la clock

				//Copie du tableau
				nextField2x[x][y]=middlex;
				nextField2y[x][y]=middley;
			}
		}
		cout<<"Interpolation : "<<tempsTotal<<" microseconds"<<endl;
		for(int x=1;x<GRID_WIDTH-1;x++){
			for(int y=1;y<GRID_HEIGHT-1;y++){
				fieldx[x][y] = nextField2x[x][y];
				fieldy[x][y] = nextField2y[x][y];//TODO with vector
			}
		}

}

void calcPressure(){ //TODO with vects
	for(int x=2;x<GRID_WIDTH-2;x++){
		for(int y=2;y<GRID_HEIGHT-2;y++){
			divergenceTab[x][y] = pressCoef*(fieldx[x+1][y]-fieldx[x-1][y]+fieldy[x][y+1]-fieldy[x][y-1]);
		}
	}
	for(int x=2;x<GRID_WIDTH-2;x++){
		for(int y=2;y<GRID_HEIGHT-2;y++){
			nextField1[x][y] = (divergenceTab[x][y]+pressureTab[x+2][y]+pressureTab[x-2][y]+pressureTab[x][y-2]+pressureTab[x][y+2])/4;
		}
	}

	for(int x=2;x<GRID_WIDTH-2;x++){
		for(int y=2;y<GRID_HEIGHT-2;y++){
			pressureTab[x][y] = nextField1[x][y];
		}
	}
	//Puis on compute les pressions
	for(int i=0;i<jacobiIterations-1;i++){
		for(int x=2;x<GRID_WIDTH-2;x++){
			for(int y=2;y<GRID_HEIGHT-2;y++){
				nextField1[x][y] = (divergenceTab[x][y]+pressureTab[x+2][y]+pressureTab[x-2][y]+pressureTab[x][y-2]+pressureTab[x][y+2])/4;
			}
		}
		for(int x=2;x<GRID_WIDTH-2;x++){
			for(int y=2;y<GRID_HEIGHT-2;y++){
				pressureTab[x][y] = nextField1[x][y];
				//cout<<pressureTab[x][y];

			}
			//cout<<endl;
		}
	}
}
void correctField(){
	for(int x=1;x<GRID_WIDTH-1;x++){
		for(int y=1;y<GRID_HEIGHT-1;y++){
			//sf::Vector2f fieldNode =  field[x][y];
			fieldx[x][y] = fieldx[x][y]-correctCoef*(pressureTab[x+1][y]-pressureTab[x-1][y]);
			fieldy[x][y] = fieldy[x][y]-correctCoef*(pressureTab[x][y+1]-pressureTab[x][y-1]);
		}
	}
}
void advectColor(){

	for(int x=1;x<GRID_WIDTH-1;x++){
		for(int y=1;y<GRID_HEIGHT-1;y++){
			float wantx = x-fieldx[x][y]*delta;
			float wanty = y-fieldy[x][y]*delta;

			//ATTENTION coords pas objet
			int x1 = (int)floor(wantx);
			int y1 = (int)floor(wanty);
			sf::Vector2i upleft(x1,y1);
			sf::Vector2i upright(upleft.x+1,upleft.y);
			sf::Vector2i downright(upleft.x+1,upleft.y+1);
			sf::Vector2i downleft(upleft.x,upleft.y+1);


			//if(x==5)cout<<x<<" "<<y<<":"<<upleft.x<<" "<<upleft.y<<":"<<colorField[upleft.x][upleft.y]<<endl;
			float top= interpolateI(wantx,upleft.x,colorField[upleft.x][upleft.y],upright.x,colorField[upright.x][upright.y]);
			float bot = interpolateI(wantx,downleft.x,colorField[downleft.x][downleft.y],downright.x,colorField[downright.x][downright.y]);
			float middle = interpolateI(wanty,upleft.y,top,downleft.y,bot);

			nextField1[x][y]= middle;
		}
	}
	for(int x=1;x<GRID_WIDTH-1;x++){
		for(int y=1;y<GRID_HEIGHT-1;y++){
			colorField[x][y] = nextField1[x][y];
		}
	}
}
void computeBounds(){
	for(int x=0;x<GRID_WIDTH;x++){
		fieldy[x][1] = -fieldy[x][2];
		fieldx[x][1] = 0;
	}
	for(int y=0;y<GRID_HEIGHT;y++){
		fieldx[1][y] = -fieldx[2][y];
		fieldy[1][y] = 0;
	}
	for(int x=0;x<GRID_WIDTH;x++){
		fieldy[x][GRID_HEIGHT-2] = -fieldy[x][GRID_HEIGHT-3];
		fieldx[x][GRID_HEIGHT-2] = 0;
	}
	for(int y=0;y<GRID_HEIGHT;y++){
		fieldx[GRID_WIDTH-2][y] = -fieldx[GRID_WIDTH-3][y];
		fieldy[GRID_WIDTH-2][y] = 0;
	}
}
void getInputs(){
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	sf::Vector2i mouseGrid;
	mouseGrid.x =floor(mousePos.x/resolution);
	mouseGrid.y =floor(mousePos.y/resolution);
	if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
		int depx = mousePos.x-lastMousePos.x;
		int depy = mousePos.y-lastMousePos.y;
		colorField[mouseGrid.x][mouseGrid.y]+=mouseAdd/2;
		for(int i =0;i<5;i++){
			for(int j =0;j<5;j++){
				fieldx[mouseGrid.x+i][mouseGrid.y+j] += depx*7;
				fieldy[mouseGrid.x+i][mouseGrid.y+j] += depy*7;
				colorField[mouseGrid.x+i][mouseGrid.y+j]+=mouseAdd/2;

			}
		}
	}
	lastMousePos = mousePos;

}
void update(){
	sf::Clock clock;
	advect();
	std::cout<<"Advect :"<<clock.getElapsedTime().asMilliseconds()<<"ms";
	clock.restart();
	calcPressure();
	std::cout<<"-Pressure :"<<clock.getElapsedTime().asMilliseconds()<<"ms";
	clock.restart();
	correctField();
	std::cout<<"-Correct :"<<clock.getElapsedTime().asMilliseconds()<<"ms";
	clock.restart();
	computeBounds();
	std::cout<<"-Bounds :"<<clock.getElapsedTime().asMilliseconds()<<"ms";
	clock.restart();
	advectColor();
	std::cout<<"-Color :"<<clock.getElapsedTime().asMilliseconds()<<"ms"<<endl;
	clock.restart();

}
int main()
{
	initRender();
	 // run the program as long as the window is open
	    while (window.isOpen())
	    {

	        // check all the window's events that were triggered since the last iteration of the loop
	        sf::Event event;
	        while (window.pollEvent(event))
	        {
	            // "close requested" event: we close the window
	            if (event.type == sf::Event::Closed)
	                window.close();
	        }
	        //getInputs();
	        getInputs();
	        sf::Clock clock;
	        update();
	        std::cout<<"Update :"<<clock.getElapsedTime().asMicroseconds()<<"micros";
	        clock.restart();
	        render();
	        std::cout<<"-Render :"<<clock.getElapsedTime().asMicroseconds()<<"micros"<<std::endl;
	    }
    return 0;
}

