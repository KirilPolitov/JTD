#pragma once
#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>

class Explosion : public sf::Drawable, public sf::Transformable {
	sf::RectangleShape textureSprite;
	float frameTime = 0.2f; // Time per frame in seconds
	bool active = true;
public:
	Explosion(sf::Texture& texture) :
		textureSprite({90, 90}), active(true) {
		textureSprite.setTexture(&texture);
		textureSprite.setOrigin(textureSprite.getLocalBounds().size / 2.0f);
	}
	void callExplosion(sf::Vector2f position) {
		textureSprite.setPosition(position);
		active = true;
		frameTime = 0.2f; // Reset frame time for the explosion
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
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		if (!active) return;
		target.draw(textureSprite, states);
	}
};