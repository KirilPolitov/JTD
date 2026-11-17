#pragma once
#include <SFML/Graphics.hpp>
#include "TileMap.hpp"
#include <iostream>

enum class EnemyType {
	normal,
	tank
	//add more types as needed
};
class Enemy : public sf::Drawable, public sf::Transformable {
	sf::Sprite textureSprite;
	sf::Vector2i position;
	sf::Vector2i destrinationPosition;
	sf::Vector2f destination;
	bool isMoving = false;
	std::array <sf::Vector2u, 4> visitationBuffer;
	int hp;
	EnemyType enemyType;
	bool isDead = false;
public:
	Enemy(const sf::Texture& texture, sf::Vector2i startPos, EnemyType type):
		textureSprite(texture),
		position(startPos),
		enemyType(type),
		isDead(false)
		{
		//by type make an intRect for the texture sprite
		textureSprite.setOrigin(textureSprite.getLocalBounds().size / 2.0f);
		textureSprite.setPosition({static_cast<float>(position.x * 64 + 32), static_cast<float>(position.y * 64 + 32)});
		switch (enemyType) {
		case EnemyType::normal:
			hp = 1;
			break;
		case EnemyType::tank:
			hp = 5;
			break;
		}
		std::cout << "spawned with hp: " << hp << std::endl;
	}
	void update(float deltaTime, const int* map, int width, int height, int& lives) {
		if (isDead) return;
		int index = position.x + position.y * width;
		if (!isMoving) {
			bool approval = false;
			bool passed = false;
			if (!(position.x + 1 >= width)) {
				if (map[index] == map[position.x + 1 + position.y * width]) {
					for (int i = 0; i < 4; ++i) {
						if (visitationBuffer[i] == sf::Vector2u(position.x + 1, position.y)) {
							approval = false;
							passed = false;
							break;
						}
						else { approval = true; passed = true; }
					}
					if (approval) {
						destrinationPosition = sf::Vector2i(position.x + 1, position.y);
						destination = sf::Vector2f(destrinationPosition.x * 64 + 32, destrinationPosition.y * 64 + 32);
						isMoving = true;
					}
				}
			}
			if (!(position.x - 1 < 0) && !passed) {
				if (map[index] == map[position.x - 1 + position.y * width]) {
					for (int i = 0; i < 4; ++i) {
						if (visitationBuffer[i] == sf::Vector2u(position.x - 1, position.y)) {
							approval = false;
							passed = false;
							break;
						}
						else { approval = true; passed = true; }
					}
					if (approval) {
						destrinationPosition = sf::Vector2i(position.x - 1, position.y);
						destination = sf::Vector2f(destrinationPosition.x * 64 + 32, destrinationPosition.y * 64 + 32);
						isMoving = true;
					}
				}
			}
			if (!(position.y - 1 < 0) && !passed) {
				if (map[index] == map[position.x + (position.y - 1) * width]) {
					for (int i = 0; i < 4; ++i) {
						if (visitationBuffer[i] == sf::Vector2u(position.x, position.y - 1)) {
							approval = false;
							passed = false;
							break;
						}
						else { approval = true; passed = true; }
					}
					if (approval) {
						destrinationPosition = sf::Vector2i(position.x, position.y - 1);
						destination = sf::Vector2f(destrinationPosition.x * 64 + 32, destrinationPosition.y * 64 + 32);
						isMoving = true;
					}
				}
			}
			if (!(position.y + 1 >= height) && !passed) {
				if (map[index] == map[position.x + (position.y + 1) * width]) {
					for (int i = 0; i < 4; ++i) {
						if (visitationBuffer[i] == sf::Vector2u(position.x, position.y + 1)) {
							approval = false;
							passed = false;
							break;
						}
						else { approval = true; passed = true; }
					}
					if (approval) {
						destrinationPosition = sf::Vector2i(position.x, position.y + 1);
						destination = sf::Vector2f(destrinationPosition.x * 64 + 32, destrinationPosition.y * 64 + 32);
						isMoving = true;
					}
				}
			}
			if (!passed) {
				//no valid move found, enemy is stuck
				isDead = true;
				switch (enemyType) {
				case EnemyType::normal:
					lives -= 1;
					break;
				case EnemyType::tank:
					lives -= 5;
					break;
				}
			}
		}
		if (isMoving) {
			sf::Vector2f currentPosition = textureSprite.getPosition();
			sf::Vector2f direction = destination - currentPosition;
			sf::Vector2f normalizedDirection = direction.normalized();
			if (std::round(currentPosition.x) == std::round(destination.x) && std::round(currentPosition.y) == std::round(destination.y)) {
				isMoving = false;
				position = destrinationPosition;
				textureSprite.setPosition(destination);
				for (int i = 3; i > 0; --i) {
					visitationBuffer[i] = visitationBuffer[i - 1];
				}
				visitationBuffer[0] = sf::Vector2u(position.x, position.y);
			}
			else {
				textureSprite.move(normalizedDirection * deltaTime * 50.0f);
				if (normalizedDirection.x > 0) {
					textureSprite.setRotation(sf::degrees(0));
				}
				if (normalizedDirection.x < 0) {
					textureSprite.setRotation(sf::degrees(180));
				}
				if (normalizedDirection.y < 0) {
					textureSprite.setRotation(sf::degrees(270));
				}
				if (normalizedDirection.y > 0) {
					textureSprite.setRotation(sf::degrees(90));
				}
			}
		}
	};
	sf::Vector2f getPosition() const {
		return textureSprite.getPosition();
	}
	void addDamage(int damage) {
		hp -= damage;
		if (hp <= 0) {
			isDead = true;
		}
	}
	const bool getIsDead() const {
		return isDead;
	}
	const EnemyType getType() const {
		return enemyType;
	}
	void setTextureRect(sf::IntRect rect) {
		textureSprite.setTextureRect(rect);
		textureSprite.setOrigin(textureSprite.getLocalBounds().size / 2.0f);
	}
	void printDebug() const {
		std::cout << "Enemy size: " << textureSprite.getLocalBounds().size.x << "x" << textureSprite.getLocalBounds().size.y << "y" << std::endl;
		std::cout << "Enemy position: " << textureSprite.getPosition().x << "x" << textureSprite.getPosition().y << "y" << std::endl;
	}
private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		if (isDead) return;
		states.transform *= getTransform();
		states.texture = &textureSprite.getTexture();
		target.draw(textureSprite, states);
	}
};