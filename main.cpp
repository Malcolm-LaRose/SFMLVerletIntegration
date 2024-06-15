// THE PLAN
// 
// Real physics
// ????
// Profit


#include "Color.h"

#include <GL/glew.h>

#include <SFML/Graphics.hpp>

#include <array>
#include <random>
#include <cmath>
#include <iostream>
#include <string>

std::mt19937 gen(std::random_device{}());
constexpr float PI = 3.141592;
const sf::Color& fillColor = Color::BLUEPRINT;
const sf::Vector2f& zeroVector = sf::Vector2f(0.0f, 0.0f);

static constexpr short borderSize = 4;

static constexpr short xPos = borderSize;
static constexpr short yPos = borderSize;

static constexpr short targetFPS = 60;
static constexpr int initScreenWidth = 1920;
static constexpr int initScreenHeight = 1080;

static constexpr int width = initScreenWidth - (2 * borderSize);
static constexpr int height = initScreenHeight - (2 * borderSize);

sf::RenderWindow window;

sf::Font font;

static const sf::Vector2f screenCenter = sf::Vector2f(initScreenWidth / 2.0f, initScreenHeight / 2.0f);


int genRandomInt(int min, int max) {
	std::uniform_int_distribution<int> dist(min, max);
	return dist(gen);
}

const sf::Vector2f& vectorSquared(const sf::Vector2f& input) { // Component-wise multiplication of vectors
	return sf::Vector2f(input.x * input.x, input.y * input.y);
}

const sf::Vector2f& multiplyVectorByScalar(const float& scalar, const sf::Vector2f& vector) {
	return sf::Vector2f(scalar * vector);
}


float deltaTime = 0.0001; // Framerate adjustable time step
float totalTime = 0.0;



struct Ball {

	Ball() : position(screenCenter), velocity(zeroVector), acceleration(zeroVector) {}

	void update(const float& totalTime, const float& dT) {

		sf::Vector2f updatedPos = position + (velocity * dT) + acceleration * (dT * dT) * 0.5f;
		sf::Vector2f updatedAcc = applyForce();
		sf::Vector2f updatedVel = velocity + multiplyVectorByScalar((dT * 0.5f), updatedAcc + acceleration);

		position = updatedPos;
		velocity = updatedVel;
		acceleration = updatedAcc;
		
	}

	const sf::Vector2f& applyForce() const { // Really an acceleration

		const sf::Vector2f gravity = sf::Vector2f(0.0f, 98.1f);
		const sf::Vector2f dragForce = sf::Vector2f(multiplyVectorByScalar(drag, velocity)); // This is less FUCKED than D = 0.5 * (rho * C * Area * vel^2) it seems
		const sf::Vector2f dragAcc = sf::Vector2f(multiplyVectorByScalar(1.0f / mass, dragForce));
		return gravity - dragAcc;

	}

	void render() const {
		sf::CircleShape ball = sf::CircleShape(radius,38);
		ball.setOrigin(radius,radius);
		ball.setPosition(position);

		window.draw(ball);
	}

	const bool contains(const int& x, const int& y) const { // Checks if a point is within a ball
		const float& distX = x - (position.x);
		const float& distY = y - (position.y);
		return (distX * distX + distY * distY) <= (radius * radius);
	}

	static constexpr float radius = 60;
	sf::Color color = Color::BLUEPRINT;


	sf::Vector2f position;
	sf::Vector2f velocity;
	sf::Vector2f acceleration;

	float mass = 1.0f;
	float drag = 0.1;


};

class World {
public:

	World() {

		// Init SFML
		window.create(sf::VideoMode(initScreenWidth, initScreenHeight), "Physics Box", sf::Style::Titlebar | sf::Style::Close);
		window.setKeyRepeatEnabled(false);
		window.setVerticalSyncEnabled(false); // Probably ignored by driver
		//window.setFramerateLimit(targetFPS); // Comment out to unlimit framerate


		// Init background
		setupVertexBuffer(borderAndBGRect, xPos, yPos, width, height, Color::EIGENGRAU);

		// Init ball storage
		listOfBalls.push_back(&ball);
		
		// Initialize screenSpacePartition
		screenSpacePartition.resize(sSPWidth, std::vector<std::vector<Ball*>>(sSPHeight));

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


	void frameCounterDisplay(const int& frameTime, const int& avg) {
		frameText.setString("FrameTime (us): " + std::to_string(frameTime) + "\nAvg. FPS: " + std::to_string(avg));

		window.draw(frameText);
	}

	void mainLoop() {

		while (running) {

			handleEvents();
			updateLogic();
			renderAll();

			frameTime = clock.restart().asMicroseconds();
			deltaTime = frameTime / 1000000.0f;
			totalFrameTime += frameTime;
			totalTime += deltaTime;

			frameCounterDisplay(frameTime, frameCount / (totalFrameTime / 1000000.0f));
			frameCount++;
			window.display();

		}
	}



private:

	std::vector<std::vector<std::vector<Ball*>>> screenSpacePartition; // Uniform grid, maybe try quadtree?
	static constexpr int sSPGridSize = 1.5 * Ball::radius;
	static constexpr int sSPWidth = ((initScreenWidth - 8) / sSPGridSize) + 1;
	static constexpr int sSPHeight = ((initScreenHeight - 8) / sSPGridSize) + 1;

	std::vector<Ball*> listOfBalls;


	sf::Clock clock;

	float frameTime = 0;
	float totalFrameTime = 0;

	sf::Text frameText;

	int frameCount = 0;

	bool running = true;

	sf::VertexBuffer borderAndBGRect;

	Ball ball;
	Ball* draggedBall = nullptr;
	float dragOffsetX;
	float dragOffsetY;
	sf::Vector2f mousePosPrev = zeroVector;
	sf::Vector2f movedDistance;

	void initFont() {
		font.loadFromFile(".\\Montserrat-Regular.ttf");
		font.setSmooth(false);
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
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left) {
					sf::Vector2i mousePos = sf::Mouse::getPosition(window);
					for (Ball* ball : listOfBalls) {
						if (ball->contains(mousePos.x, mousePos.y)) {
							ball->color = Color::BLUE;
							ball->acceleration - ball->applyForce();
							ball->velocity = zeroVector;
							draggedBall = ball; // Get the address of the ball that is being dragged
							dragOffsetX = mousePos.x - ball->position.x;
							dragOffsetY = mousePos.y - ball->position.y;
							break;
						}
					}
				}
				break;
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left) {
					if (draggedBall != nullptr) {
						draggedBall->color = Color::BLUEPRINT;
						draggedBall->velocity = movedDistance / deltaTime / 100.0f;
						draggedBall = nullptr; // Reset the pointer after releasing
					}
				}
				break;
			case sf::Event::MouseMoved:
				if (draggedBall != nullptr) { // Only drag if a ball is being dragged
					for (Ball* ball : listOfBalls) {
						if (ball == draggedBall) {
							draggedBall->acceleration -= draggedBall->applyForce();
						}
					}
					dragBall(); 
				}
				break;
			default: 
			
			
				if (draggedBall != nullptr) {
					for (Ball* ball : listOfBalls) {
						if (ball == draggedBall) {
							draggedBall->acceleration -= draggedBall->applyForce();
						}
					}
				}
			}
		}
	}

	void updateLogic() {

		ball.update(totalTime, 4 * deltaTime); // 4 makes the sim run 4 times faster but costs precision
		updatePartition();
		checkBoundaryCollision();

	}

	void updatePartition() { // Necessary for many balls, sorta silly with just one
		for (std::vector<std::vector<Ball*>>& column : screenSpacePartition) {
			for (std::vector<Ball*>& cell : column) {
				cell.clear();
			}
		}

		int cellX = static_cast<int>(ball.position.x / sSPGridSize);
		int cellY = static_cast<int>(ball.position.y / sSPGridSize);
		if (cellX >= 0 && cellX < sSPWidth && cellY >= 0 && cellY < sSPHeight) {
			screenSpacePartition[cellX][cellY].push_back(&ball);
		}

	}

	void checkBoundaryCollision() { // Checks extreme edge cells only, treats boundaries as having infinite mass
		// First and last columns (x = 0 and x = maxX)
		const auto& frontCol = screenSpacePartition[0]; // First column (x = 0)
		const auto& backCol = screenSpacePartition[screenSpacePartition.size() - 1]; // Last column (x = maxX)

		// Iterate through each cell in the first column
		for (const auto& cell : frontCol) {
			for (const auto& ballPtr : cell) {
				if (ballPtr != nullptr) {
					if (ballPtr->position.x <= (borderSize + ball.radius)) {
						ballPtr->position.x = borderSize + ball.radius; // Correct the position
						ballPtr->velocity.x = -ballPtr->velocity.x; // Invert the velocity
					}
				}
			}
		}

		// Iterate through each cell in the last column
		for (const auto& cell : backCol) {
			for (const auto& ballPtr : cell) {
				if (ballPtr != nullptr) {
					if (ballPtr->position.x >= (initScreenWidth - borderSize - ball.radius)) {
						ballPtr->position.x = initScreenWidth - borderSize - ball.radius; // Correct the position
						ballPtr->velocity.x = -ballPtr->velocity.x; // Invert the velocity
					}
				}
			}
		}

		// First and last rows (y = 0 and y = maxY) in all columns
		for (const auto& col : screenSpacePartition) {
			const auto& frontRow = col[0]; // First row in each column (y = 0)
			const auto& backRow = col[col.size() - 1]; // Last row in each column (y = maxY)

			// Iterate through each ball in the first row of each column
			for (const auto& ballPtr : frontRow) {
				if (ballPtr != nullptr) {
					if (ballPtr->position.y <= (borderSize + ball.radius)) {
						ballPtr->position.y = borderSize + ball.radius; // Correct the position
						ballPtr->velocity.y = -ballPtr->velocity.y; // Invert the velocity
					}
				}
			}

			// Iterate through each ball in the last row of each column
			for (const auto& ballPtr : backRow) {
				if (ballPtr != nullptr) {
					if (ballPtr->position.y >= (initScreenHeight - borderSize - ball.radius)) {
						ballPtr->position.y = initScreenHeight - borderSize - ball.radius; // Correct the position
						ballPtr->velocity.y = -ballPtr->velocity.y; // Invert the velocity
					}
				}
			}
		}
	}

	void dragBall() {
		const sf::Vector2i& mousePos = sf::Mouse::getPosition(window);
		movedDistance = sf::Vector2f(mousePos) - mousePosPrev;
		mousePosPrev = sf::Vector2f(mousePos);


		// Calculate the boundaries considering the size of the dragged ball
		int maxXPos = window.getSize().x - draggedBall->radius;
		int maxYPos = window.getSize().y - draggedBall->radius;

		// Ensure the mouse position stays within the window bounds
		int newXPos = std::max(0, std::min(mousePos.x, maxXPos));
		int newYPos = std::max(0, std::min(mousePos.y, maxYPos));

		// Update the position of the dragged ball
		draggedBall->position = sf::Vector2f(newXPos, newYPos);
	}



	void renderWorld() const {
		window.draw(borderAndBGRect);
	}

	void renderAll() const {
		renderWorld();
		ball.render();
	}

};


int main() {

	World world;

	world.mainLoop();

	return 0;
}