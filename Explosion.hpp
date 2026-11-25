#pragma once
#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>

class Explosion : public sf::Drawable, public sf::Transformable {
	sf::RectangleShape textureSprite;
	float frameTime = 0.2f; // Time per frame in seconds
	bool active = true;
public:
	Explosion(const sf::Texture& texture, sf::Vector2f size = {90, 90}, float duration = 0.2f) :
		textureSprite(size), active(true), frameTime(duration) {
		textureSprite.setTexture(&texture);
		textureSprite.setOrigin(textureSprite.getLocalBounds().size / 2.0f);
		if (textureSprite.getSize() == sf::Vector2f{50, 50}) {
			textureSprite.setTextureRect({ { 22 * 64, 12 * 64 }, { 64, 64 } });
			active = false;
		}
	}
	void callExplosion(sf::Vector2f position, float duration = 0.2f) {
		textureSprite.setPosition(position);
		active = true;
		frameTime = duration; // Reset frame time for the explosion
	}
	const bool isActive() const {
		return active;
	}
	void updateCooldown(float deltaTime) {
		if (active) {
			frameTime -= deltaTime;
			if (frameTime <= 0.0f) {
				active = false; // Deactivate the explosion after the frame time has elapsed
			}
		}
	}
	void randomizeRotation() {
		float randomAngle = static_cast<float>(std::rand() % 360);
		textureSprite.setRotation(sf::degrees(randomAngle));
	}
	void setRotation(float angle) {
		textureSprite.setRotation(sf::degrees(angle));
	}
	void move(sf::Vector2f direction) {
		textureSprite.move(direction.normalized() * 25.f);
	}
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		if (!active) return;
		target.draw(textureSprite, states);
	}
};