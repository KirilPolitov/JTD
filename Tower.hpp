#pragma once
#include <SFML/Graphics.hpp>
#include "Enemy.hpp"
#include "Projectile.hpp"

enum class state {
	held,
	ready,
	cooldown
};
class Tower : public sf::Drawable {
	type towerType;
	state towerState;
	sf::RectangleShape textureSprite;
	bool active;
	int range;
	float cooldownTime = 1.95f; //seconds

	int getIndexFromPosition(sf::Vector2i position, int mapWidth) {
		return position.x + position.y * mapWidth;
	}
public:
	void setAlpha(float alpha) {
		sf::Color color{
			static_cast<uint8_t>(textureSprite.getFillColor().r),
			static_cast<uint8_t>(textureSprite.getFillColor().g),
			static_cast<uint8_t>(textureSprite.getFillColor().b),
			static_cast<uint8_t>(alpha * 255)
		};
		textureSprite.setFillColor(color);
	}
	void setRed(float red) {
		sf::Color color{
			static_cast<uint8_t>(textureSprite.getFillColor().r),
			static_cast<uint8_t>(red * 255),
			static_cast<uint8_t>(red * 255),
			static_cast<uint8_t>(textureSprite.getFillColor().a)
		};
		textureSprite.setFillColor(color);
	}
	void toggleActive() {
		active = !active;
	}
	const bool isActive() const {
		return active;
	}
	Tower(type type, const sf::Texture& texture, sf::Vector2f position) : towerType(type), towerState(state::held), textureSprite({32, 32}), active(false) {
		switch (type) {
		case type::turret:
			cooldownTime = 1.95f;
			range = 150;
			textureSprite.setSize({ 40, 40 });
			textureSprite.setTexture(&texture);
			textureSprite.setTextureRect({ { 19 * 64, 10 * 64 }, { 64, 64 } }); 
			break;
		case type::rocket:
			cooldownTime = 4.0f;
			range = 200;
			textureSprite.setSize({ 55, 55 });
			textureSprite.setTexture(&texture);
			textureSprite.setTextureRect({ { 22 * 64, 8 * 64 }, { 64, 64 } });
			break;
		}
		textureSprite.setOrigin(textureSprite.getLocalBounds().size / 2.0f);
		textureSprite.setPosition(position);
		setAlpha(0.7f);
	}
	void reset(type type, sf::Vector2f position) {
		towerType = type;
		towerState = state::held;
		active = false;
		switch (type) {
		case type::turret:
			cooldownTime = 1.95f;
			range = 150;
			textureSprite.setSize({ 40, 40 });
			textureSprite.setTextureRect({ { 19 * 64, 10 * 64 }, { 64, 64 } });
			break;
		case type::rocket:
			cooldownTime = 4.0f;
			range = 200;
			textureSprite.setSize({ 55, 55 });
			textureSprite.setTextureRect({ { 22 * 64, 8 * 64 }, { 64, 64 } });
			break;
		}
		textureSprite.setOrigin(textureSprite.getLocalBounds().size / 2.0f);
		textureSprite.setPosition(position);
		setAlpha(0.7f);
	}
	bool place(sf::Vector2f position, const int* map, int spawnTile, const std::vector<Tower>& towers, int currency) {
		if (towerState == state::held) {
			switch (towerType) {
			case type::turret:
				if (currency < 5) return false;
				break;
			case type::rocket:
				if (currency < 10) return false;
				break;
			}
			sf::Vector2i mapPosition = { static_cast<int>(position.x) / 64, static_cast<int>(position.y) / 64 };
			int index = mapPosition.x + mapPosition.y * 16;
			if (map[index] == spawnTile) return false;
			//check collisions based on the diagram in my math notebook
			mapPosition.y -= 1;
			//sides
			index = mapPosition.x + mapPosition.y * 16;
			sf::Vector2f boundary = { mapPosition.x * 64.f, mapPosition.y * 64.f + 64.f };
			if (map[index] == spawnTile) {
				if (position.y - boundary.y < 15)
					return false;
			}
			mapPosition.y += 1;
			mapPosition.x += 1;
			index = mapPosition.x + mapPosition.y * 16;
			boundary.x = mapPosition.x * 64;
			if (map[index] == spawnTile) {
				if (boundary.x - position.x < 15)
					return false;
			}
			mapPosition.x -= 1;
			mapPosition.y += 1;
			index = mapPosition.x + mapPosition.y * 16;
			boundary.y = mapPosition.y * 64;
			if (map[index] == spawnTile) {
				if (boundary.y - position.y < 15)
					return false;
			}
			mapPosition.x -= 1;
			mapPosition.y -= 1;
			index = mapPosition.x + mapPosition.y * 16;
			boundary.x = mapPosition.x * 64 + 64;
			if (map[index] == spawnTile) {
				if (position.x - boundary.x < 15)
					return false;
			}
			//corners
			mapPosition.y -= 1;
			index = mapPosition.x + mapPosition.y * 16;
			boundary.x = mapPosition.x * 64 + 64;
			boundary.y = mapPosition.y * 64 + 64;
			sf::Vector2f direction = boundary - position;
			float distance = direction.lengthSquared();
			if (map[index] == spawnTile) {
				if (distance < 15 * 15)
					return false;
			}
			mapPosition.x += 2;
			index = mapPosition.x + mapPosition.y * 16;
			boundary.x = mapPosition.x * 64;
			boundary.y = mapPosition.y * 64 + 64;
			direction = boundary - position;
			distance = direction.lengthSquared();
			if (map[index] == spawnTile) {
				if (distance < 15 * 15)
					return false;
			}
			mapPosition.y += 2;
			index = mapPosition.x + mapPosition.y * 16;
			boundary.x = mapPosition.x * 64;
			boundary.y = mapPosition.y * 64;
			direction = boundary - position;
			distance = direction.lengthSquared();
			if (map[index] == spawnTile) {
				if (distance < 15 * 15)
					return false;
			}
			mapPosition.x -= 2;
			index = mapPosition.x + mapPosition.y * 16;
			boundary.x = mapPosition.x * 64 + 64;
			boundary.y = mapPosition.y * 64;
			direction = boundary - position;
			distance = direction.lengthSquared();
			if (map[index] == spawnTile) {
				if (distance < 15 * 15)
					return false;
			}
			for (int i = 0; i < towers.size(); i++) {
				direction = towers[i].getPosition() - position;
				distance = direction.lengthSquared();
				if (distance < 30 * 30)
					return false;
			}
			return true;
		}
		return false;
	}
	void shoot(sf::Vector2f target, std::vector<Projectile>& projectiles, const sf::Texture& projectileTexture) {
		// Implement shooting logic based on tower type and target
		if (towerState == state::ready) {
			sf::Vector2f direction = target - textureSprite.getPosition();
			switch (towerType) {
			case type::turret:
				cooldownTime = 1.95f; // Reset cooldown time for the next shot
				break;
			case type::rocket:
				cooldownTime = 4.0f; // Reset cooldown time for the next shot
				break;
			}
			for (Projectile& projectile : projectiles) {
				if (!projectile.isActive()) {
					projectile.reset(towerType, projectileTexture, textureSprite.getPosition(), target);
					towerState = state::cooldown;
					float angleRad = std::atan2(direction.y, direction.x);
					float angleDeg = angleRad * 180.0f / 3.14159265f + 90;
					textureSprite.setRotation(sf::degrees(angleDeg));
					return;
				}
			}
			Projectile newProjectile(towerType, projectileTexture, textureSprite.getPosition(), target);
			projectiles.push_back(newProjectile);
			towerState = state::cooldown;
			float angleRad = std::atan2(direction.y, direction.x);
			float angleDeg = angleRad * 180.0f / 3.14159265f + 90;
			textureSprite.setRotation(sf::degrees(angleDeg));
		}
	}
	void updateCooldown(float deltaTime) {
		if (towerState == state::cooldown) {
			cooldownTime -= deltaTime;
			if (cooldownTime <= 0.0f) {
				towerState = state::ready;
			}
		}
	}
	const type getType() const {
		return towerType;
	}
	void setState(state m_state) {
		towerState = m_state;
	}
	const state getState() const {
		return towerState;
	}
	//void attack(const Enemy& target) {
	//	// Implement attack logic based on tower type and target
	//}
	void setPosition(sf::Vector2f position) {
		textureSprite.setPosition(position);
	}
	const sf::Vector2f getPosition() const {
		return textureSprite.getPosition();
	}
	const int getRange() const {
		return range;
	}
	void setTextureRect(sf::IntRect rect) {
		textureSprite.setTextureRect(rect);
	}
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		if (!active) return;
		target.draw(textureSprite, states);
	}
};