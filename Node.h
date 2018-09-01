#pragma once
#ifndef BATTLEROYALE_PROJECTILE_H
#define BATTLEROYALE_PROJECTILE_H

#include <SFML/Graphics.hpp>

class Node : public sf::CircleShape
{
private:
	sf::RenderWindow& rWindRef;
	sf::Text valueText;
	int connections = 0;
	sf::VertexArray connectionLines = sf::VertexArray(sf::Lines, connections * 2);

public:
	bool hovered = false;
	std::vector<std::shared_ptr<Node>> connectedNodes;
	int index = -1;
	int value = 0;

	Node(int index, sf::Vector2f pos, sf::Font& font, sf::RenderWindow& rWind) : rWindRef(rWind) {
		this->index = index;

		// Circle Graphics
		this->setRadius(32.f);
		this->setFillColor(sf::Color(131, 131, 131, 255));
		this->setOutlineColor(sf::Color(196, 196, 196, 255));
		this->setOutlineThickness(0.5f);
		this->setOutlineColor(sf::Color::White);
		this->setOrigin(sf::Vector2f(this->getGlobalBounds().width / 2.f, this->getGlobalBounds().height / 2.f));
		this->setPosition(pos);
	
		// Text Graphics
		valueText.setFont(font);
		valueText.setFillColor(sf::Color::White);
		valueText.setOutlineColor(sf::Color::Black);
		valueText.setOutlineThickness(0.5f);
		valueText.setString(((value < 0) ? "-$" : "$") + std::to_string(abs(value)));
		valueText.setOrigin(sf::Vector2f(valueText.getGlobalBounds().width / 2.f, valueText.getGlobalBounds().height / 2.f));
		valueText.setPosition(getPosition());
	}

	// Update value and set text
	void setValue(int newVal) {
		value = newVal;
		valueText.setString(((value < 0) ? "-$" : "$") + std::to_string(abs(value)));
		valueText.setOrigin(sf::Vector2f(valueText.getGlobalBounds().width / 2.f, valueText.getGlobalBounds().height / 2.f));
		valueText.setPosition(getPosition());
	}

	// Set connection lines to green if mouse is inside the bounds
	bool CheckHover(bool alreadyFoundOne=false) {
		if (!alreadyFoundOne) {
			bool hover = sf::Mouse::getPosition(rWindRef).x >= this->getGlobalBounds().left &&
				sf::Mouse::getPosition(rWindRef).x < this->getGlobalBounds().left + this->getGlobalBounds().width &&
				sf::Mouse::getPosition(rWindRef).y >= this->getGlobalBounds().top &&
				sf::Mouse::getPosition(rWindRef).y < this->getGlobalBounds().top + this->getGlobalBounds().height;
			hovered = hover;
		} else {
			hovered = false;
		}
		
		for (int i = 0; i < connectionLines.getVertexCount(); i++) {
			connectionLines[i].color = hovered ? sf::Color::Green : sf::Color::White;
		}
		return hovered;
	}

	// Distribute money to connections
	void click() {
		setValue(value-connections);
		for (auto n : connectedNodes) {
			n->setValue(n->value + 1);
		}
	}

	void Update(float dt) {
		
	}

	// Check if the connection is valid and then add it in
	bool connect(std::shared_ptr<Node> other, int nodeCount) {
		// check if valid
		if (connections < (nodeCount-1)) {
			// if already connected
			for (auto n : connectedNodes) {
				if (n == other) {
					return false;
				}
			}
			connections++;
			connectedNodes.push_back(other);
			connectionLines.resize(connections * 2);
			connectionLines.append(getPosition());
			connectionLines.append(other->getPosition());
			return true;
		} else {
			std::cout << "Invalid" << std::endl;
			return false;
		}
	}

	void DrawConnections() {
		rWindRef.draw(connectionLines);
	}

	void Draw() {
		rWindRef.draw(*this);
		rWindRef.draw(valueText);
	}

};

#endif // BATTLEROYALE_PROJECTILE_H