// THE PLAN
// 
// Learn stack vs. heap
// std::array -> c style array
// Optimize ball collision by only checking in neighborhood around ball for 'ball2'
// Fix collision skipping --> Multiple 'physics frames' per 'render frame' 
// Dont rerender pixels that didnt change? esp background and border
// Real physics
// ????
// Profit


#include "color.h"

#include <GL/glew.h>

#include <SFML/Graphics.hpp>

#include <array>
#include <random>
#include <cmath>
#include <iostream>
#include <string>

std::mt19937 gen(std::random_device{}());
constexpr float PI = 3.141592;
const sf::Color fillColor = Color::BLUEPRINT;

int genRandomInt(int min, int max) {
	std::uniform_int_distribution<int> dist(min, max);
	return dist(gen);
}

const sf::Vector2f& vectorSquared(const sf::Vector2f& input) { // Component-wise multiplication of vectors
	return sf::Vector2f(input.x * input.x, input.y * input.y);
}

const sf::Vector2f& multiplyVectorByScalar(const float& scalar, const sf::Vector2f& vector) {
	
}


float deltaTime = 0.001; // Framerate adjustable time step
float totalTime = 0.0;



struct Ball {

	void update(const float& totalTime, const float& dT) {

		sf::Vector2f updatedPos = position + velocity * dT + acceleration * (dT * dT) * 0.5f;
		sf::Vector2f updatedAcc = applyForce(); 
		sf::Vector2f updatedVel = velocity + multiplyVectorByScalar((dT * 0.5f), updatedAcc + acceleration);

		position = updatedPos;
		velocity = updatedVel;
		acceleration = updatedAcc;
		
	}


	const sf::Vector2f& applyForce() const {
		
		sf::Vector2f gravity = sf::Vector2f(0.0f, 9.81f);
		sf::Vector2f dragForce = sf::Vector2f(multiplyVectorByScalar(0.5f * drag , vectorSquared(velocity)));
		sf::Vector2f dragAcc = sf::Vector2f(multiplyVectorByScalar(1.0f/mass, dragForce));
		return gravity - dragAcc;


	}

	
	sf::Vector2f position;
	sf::Vector2f velocity;
	sf::Vector2f acceleration;

	float mass;
	float drag;


};

class World {
public:

	World() {

		// Init SFML
		window.create(sf::VideoMode(initScreenWidth, initScreenHeight), "Physics Box", sf::Style::Titlebar | sf::Style::Close);
		window.setKeyRepeatEnabled(false);
		window.setVerticalSyncEnabled(false); // Probably ignored by driver
		window.setFramerateLimit(targetFPS); // Comment out to unlimit framerate


		// Init background
		setupVertexBuffer(borderAndBGRect, xPos, yPos, width, height, Color::EIGENGRAU);
		

		// Init balls


		// Init Font
		initFont();

		// Set framecounter position
		frameText.setPosition(borderSize, borderSize);
	}

	void setupVertexBuffer(sf::VertexBuffer& vertexBuffer, const int& xPos, const int& yPos, const int& width, const int& height, const sf::Color& color) {

		sf::Vertex vertices[6];

		// First triangle (top-left, top-right, bottom-right)
		vertices[0].position = sf::Vector2f(xPos, yPos);
		vertices[1].position = sf::Vector2f(xPos + width, yPos);
		vertices[2].position = sf::Vector2f(xPos + width, yPos + height);

		// Second triangle (top-left, bottom-right, bottom-left)
		vertices[3].position = sf::Vector2f(xPos, yPos);
		vertices[4].position = sf::Vector2f(xPos + width, yPos + height);
		vertices[5].position = sf::Vector2f(xPos, yPos + height);

		for (int i = 0; i < 6; i++) {
			vertices[i].color = color;
		}

		// Create the vertex buffer
		vertexBuffer.create(6);
		vertexBuffer.setPrimitiveType(sf::Triangles);
		vertexBuffer.update(vertices);


	}

	void placeNRandomBalls() {
		int i = 0;
		while (i < numBalls) {

		}
	}


	void frameCounterDisplay(const float& frameTime, const float& avg) {
		frameText.setString("FrameTime (us): " + std::to_string(frameTime) + "\nAvg. FPS: " + std::to_string(avg));

		window.draw(frameText);
	}

	void mainLoop() {

		while (running) {

			handleEvents();
			updateLogic();
			renderAll();

			frameTime = clock.restart().asMicroseconds();
			totalFrameTime += frameTime;

			frameCounterDisplay(frameTime, frameCount / (totalFrameTime / 1000000));
			frameCount++;
			window.display();

		}
	}



private:

	float frameTime = 0;
	float totalFrameTime = 0;

	sf::Text frameText;

	int frameCount = 0;

	bool running = true;

	static constexpr short borderSize = 4;

	static constexpr int xPos = borderSize;
	static constexpr int yPos = borderSize;

	static constexpr short targetFPS = 60;
	static constexpr int initScreenWidth = 1920;
	static constexpr int initScreenHeight = 1080;

	static constexpr int width = initScreenWidth - (2 * borderSize);
	static constexpr int height = initScreenHeight - (2 * borderSize);

	static constexpr int numBalls = 10;

	sf::VertexBuffer borderAndBGRect;

	std::array<Ball, numBalls> listOfBalls; // Any better data structures to use? Can I combine this with ssp?

	sf::RenderWindow window;

	sf::Clock clock;

	sf::Font font;

	void initFont() {
		font.loadFromFile(".\\Montserrat-Regular.ttf");
		frameText.setCharacterSize(34);
		frameText.setFillColor(Color::WHITE);
		frameText.setFont(font);
	}


	void handleEvents() {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				running = false;
				break;
			/*case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left) {
					sf::Vector2i mousePos = sf::Mouse::getPosition(window);
					for (Ball& ball : listOfBalls) {
						if (ball.contains(mousePos.x, mousePos.y)) {
							ball.color = Color::BLUE;
							ball.isDragging = true;
							dragOffsetX = mousePos.x - ball.xPos;
							dragOffsetY = mousePos.y - ball.yPos;
							break;
						}
					}
				}
				break;
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left) {
					for (Ball& ball : listOfBalls) {
						ball.color = Color::BLUEPRINT;
						ball.isDragging = false;
					}
				}
				break;
			case sf::Event::MouseMoved:
				for (Ball& ball : listOfBalls) {
					dragBall(ball);
				}
				break;*/

				/*case sf::Event::KeyPressed:
					if (event.key.scancode == sf::Keyboard::Scan::Space) {
						jiggle();
					}
					break;
				*/
			}
		}
	}


	//void dragBall(Ball& ball) {
	//	if (ball.isDragging) {
	//		const sf::Vector2i& mousePos = sf::Mouse::getPosition(window);

	//		// Putting boundary checking here caused ball overlapping problems
	//		ball.setPosition(mousePos.x - dragOffsetX, mousePos.y - dragOffsetY);
	//	}
	//}


	void updateLogic() {

		// gravity();

		collisionChecking();

	}

	void collisionChecking() {
		
	}

	void checkBoundaryCollision(Ball& ball) {
		

	}

	bool pointContainsBall(const float& x, const float& y, const float& sDist) {


	}

	void checkCollisionCheap(Ball& ball) {

	}




	void renderWorld() {
		window.draw(borderAndBGRect);
	}

	// Num triangles used to render the circles
	static constexpr int numSegments = 12;

	// Calculate the angle between each segment
	static constexpr float angleStep = 2 * PI / numSegments;


	//void renderBalls() {

	//	// Prepare the vertex array to hold all the triangles
	//	sf::VertexArray vertices(sf::Triangles, listOfBalls.size() * 3 * numSegments);

	//	// Populate the vertex array with the vertices of each ball
	//	for (size_t i = 0; i < listOfBalls.size(); ++i) {
	//		Ball& ball = listOfBalls[i];

	//		const sf::Vector2f& position = ball.getPosition();
	//		const float& radius = ball.radius;

	//		// Iterate over each segment and create triangles
	//		for (int j = 0; j < numSegments; ++j) {
	//			// Calculate the angle of the current segment
	//			const float& angle1 = j * angleStep;
	//			const float& angle2 = (j + 1) * angleStep;

	//			// Calculate the vertices of the triangle
	//			const sf::Vector2f vertex1(position.x, position.y);
	//			const sf::Vector2f vertex2(position.x + radius * std::cos(angle1), position.y + radius * std::sin(angle1));
	//			const sf::Vector2f vertex3(position.x + radius * std::cos(angle2), position.y + radius * std::sin(angle2));
	//			
	//			// Set the vertices of the triangle in the vertex array
	//			vertices[i * numSegments * 3 + j * 3].position = vertex1;
	//			vertices[i * numSegments * 3 + j * 3].color = ball.color;
	//			vertices[i * numSegments * 3 + j * 3 + 1].position = vertex2;
	//			vertices[i * numSegments * 3 + j * 3 + 1].color = ball.color;
	//			vertices[i * numSegments * 3 + j * 3 + 2].position = vertex3;
	//			vertices[i * numSegments * 3 + j * 3 + 2].color = ball.color;
	//		}
	//	}

	//	// Draw all the triangles at once
	//	window.draw(vertices);
	//}


	void renderAll() {
		renderWorld();
		// renderBalls();
	}

};


int main() {

	World world;

	world.mainLoop();

	return 0;
}