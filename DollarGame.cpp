#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include "math.h"
#include <memory>

#include "Node.h"

inline double randomDouble(double a, double b) {
	double random = ((double)rand()) / (double)RAND_MAX;
	double diff = b - a;
	double r = random * diff;
	return a + r;
}

// inclusive min, inclusive max
template <typename T>
inline T rNum(T Min, T Max) { return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min; }
// inclusive max
template <typename T>
inline T rNum(T Max) { return T((float(rand()) / float(RAND_MAX)) * (Max - 0)) + 0; }

int main() {
	const unsigned int SCREEN_WIDTH = 1280;
	const unsigned int SCREEN_HEIGHT = 720;

	const unsigned int FPS = 60; //The desired FPS. (The number of updates each second) or 0 for uncapped.

	srand(static_cast<unsigned int>(time(NULL)));

	sf::ContextSettings settings;
	settings.antialiasingLevel = 16;

	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Dollar Game", sf::Style::Default, settings);
	window.setFramerateLimit(FPS);

	sf::Color clearColour = sf::Color(51, 51, 51);

	sf::Font ArialFont;
	ArialFont.loadFromFile("C:/Windows/Fonts/arial.ttf");
	
	// Controls
	int nodeCount = 6;
	int maxSum = 1; 
	int minValue = -6;
	int maxValue = 4;
	double borderInset = 0.1;

	// Generation vars
	int edgeCount = -1;
	int pot = -1;
	int genus = -1;
	std::vector<std::shared_ptr<Node>> nodes;

	while (pot < 0 || pot > maxSum) { // difficulty of the game
		// Create nodes
		nodes.clear();
		pot = -1;
		for (int i = 0; i < nodeCount; i++) {
			int newVal = rNum(minValue, maxValue);
			pot += newVal;
			std::shared_ptr<Node> newNode = std::make_shared<Node>(nodes.size(),sf::Vector2f(randomDouble(borderInset, 1.0-borderInset)*SCREEN_WIDTH, randomDouble(borderInset, 1.0-borderInset)*SCREEN_HEIGHT), ArialFont, window);
			newNode->setValue(newVal);
			nodes.push_back(newNode);
		}
	}

	// first make sure every node has at least one connection
	for (int i = 0; i < nodeCount; i++) {
		bool valid = false;
		while (!valid) {
			int indexA = i;
			int indexB = rNum(0, nodeCount);
			while (indexB == indexA) {
				indexB = rNum(0, nodeCount);
			}
			// if one of the nodes isn't fully connected it is valid
			bool aValid = nodes[indexA]->connect(nodes[indexB], nodeCount);
			bool bValid = nodes[indexB]->connect(nodes[indexA], nodeCount);
			valid = aValid || bValid;
		}
		edgeCount++;

		// Check Genus
		genus = edgeCount - nodeCount + 1;
	}

#pragma region UI
	sf::Text FPSCounter("FPS", ArialFont);
	FPSCounter.setCharacterSize(20);
	FPSCounter.setFillColor(sf::Color::White);
	FPSCounter.setOutlineColor(sf::Color::Black);
	FPSCounter.setOutlineThickness(0.5f);
	FPSCounter.setPosition(SCREEN_WIDTH - 30, 0);
	sf::Text BankText = sf::Text("Bank: " + std::to_string(pot), ArialFont);
	BankText.setFillColor(sf::Color::White);
	BankText.setOutlineColor(sf::Color::Black);
	BankText.setOutlineThickness(0.5f);
	BankText.setPosition(0, 0);
	sf::Text GenusText = sf::Text("Genus: " + std::to_string(genus), ArialFont);
	GenusText.setFillColor(sf::Color::White);
	GenusText.setOutlineColor(sf::Color::Black);
	GenusText.setOutlineThickness(0.5f);
	GenusText.setPosition(sf::Vector2f(0, 30));
#pragma endregion UI

	// Selected node index
	int selected = -1;

	bool paused = false;
	sf::Event ev;
	sf::Clock clock;
	float lastDeltaTime = 0.f;
	while (window.isOpen()) {
		float deltaTime = clock.restart().asSeconds();
		float fps = 1.f / deltaTime;
		lastDeltaTime = deltaTime;

		FPSCounter.setString(std::to_string((int)fps));

		//Handle events
		while (window.pollEvent(ev)) {
			if (ev.type == sf::Event::Closed)
				window.close();

			if (ev.type == sf::Event::KeyPressed) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
					window.close();
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
					paused = !paused;
				}
			}

			// Check for buttons when we move the mouse
			if (ev.type == sf::Event::MouseMoved)
			{
				bool found = false; // have we already found one
				for (auto n : nodes) {
					if (n->CheckHover(found) && !found) {
						selected = n->index;
						found = true;	
					}
				}
			}

			// Activates the button if it is hovered
			if (ev.type == sf::Event::MouseButtonPressed) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					nodes[selected]->click();
				}
			}
		}

		// Update stuff
		if (!paused) {
		
		}

		// Draw stuff
		window.clear(clearColour);

		// Draw all non selected lines first
		for (auto n : nodes) {
			if (n != nodes[selected])
				n->DrawConnections();
		}

		// Draw selected lines above the not selected lines
		if (selected != -1) {
			nodes[selected]->DrawConnections();
		}

		// Draw all nodes
		for (auto n : nodes) {
			n->Draw();
		}

		window.draw(BankText);
		window.draw(GenusText);
		window.draw(FPSCounter);

		window.display();
	}

	return 0;
}