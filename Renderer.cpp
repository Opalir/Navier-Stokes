


#include <SFML/Graphics.hpp>
#include "Renderer.h"
#include <vector>
#include <cmath>
#include <stdlib.h>
#include <iostream>

sf::RectangleShape rect;
sf::RenderWindow window;

int lastColorField[GRID_WIDTH][GRID_HEIGHT];

void initRender(){
	window.create(sf::VideoMode(GRID_WIDTH*resolution,GRID_HEIGHT*resolution), "SFML Window");
	rect.setSize(sf::Vector2f(resolution,resolution));
	window.setPosition(sf::Vector2i(0,0));
	window.clear(sf::Color::Black);

}
void render(){ //Render everything


	//window.clear(sf::Color::Black);
	for(int i=0;i<GRID_WIDTH-1;i++){
		for(int j=0;j<GRID_HEIGHT-1;j++){


			//ATTENTION : RAFRAICHISSEMENT PARTIEL = ARTEFACTS SUR L'IMAGE
			if(abs(colorField[i][j]-lastColorField[i][j])<1)continue;

			//std::cout<<colorField[i][j]-lastColorField[i][j]<<std::endl;

			lastColorField[i][j] = colorField[i][j];
			float rnd = round(colorField[i][j]);
			float max = std::max(0.f,rnd);
			int color = std::min(255.f,max);
			//if(color<1)continue;
			//renderedTiles++;
			//int color = std::min(255,std::max(0,round((float)colorField[i][j])));

			rect.setFillColor(sf::Color(color,color,color));
			rect.setPosition(i*resolution,j*resolution);
			window.draw(rect);

			//rect.setFillColor(sf::Color::Yellow);

			//rectToRender[Math.min(255,Math.max(0,Math.round(colorField[i][j])))].push({x:(i+a/ITERATION_PER_TILE)*resolution,y:(j+b/ITERATION_PER_TILE)*resolution});

			/*sf::Vector2f beg(i*resolution,j*resolution);
			sf::Vector2f end((i+field[i][j].x)*resolution,(j+field[i][j].y)*resolution);
			sf::Vertex line[] =
			{
				sf::Vertex(beg,sf::Color::Red),
				sf::Vertex(end,sf::Color::Red)
			};
			window.draw(line, 2, sf::Lines);*/
		}
	}
	/*for(let i=0;i<256;i++){
		ctx.fillStyle = "rgb("+i+","+i+","+ i +")";
		renderMultipleRect(rectToRender[i]);
		rectToRender[i]=[];
	}*/


	window.display();
}


//EN COURS
/*void renderMultipleSquare(tab){
	for(unsigned j =0;j<tab.size();++j){
		poly.setPointCount(polygon.size());
		for(int i =0;i<polygon.size();++i){
			poly.setPoint(i, sf::Vector2f(polygon[i].x,polygon[i].y));
			cercle.setPosition(polygon[i].x,polygon[i].y);
		}
		if(j==polygons.size()-1)poly.setFillColor(sf::Color(0,0,100));
		window.draw(poly);
	}
	ctx.beginPath();
		for(let i=0;i<tab.length;i++){
			ctx.moveTo(tab[i].x,tab[i].y);
			ctx.lineTo(tab[i].x+GRAD_RESOLUTION,tab[i].y);
			ctx.lineTo(tab[i].x+GRAD_RESOLUTION,tab[i].y+GRAD_RESOLUTION);
			ctx.lineTo(tab[i].x,tab[i].y+GRAD_RESOLUTION);
		}
		ctx.fill();
}*/
