#pragma once
#include <SFML/Graphics.hpp>
#include "Enemy.hpp"
#include "Explosion.hpp"

enum class type {
	turret,
	rocket,
	//add more types as needed
};

enum class CollisionResult {
	Hit,
	Miss,
	OutOfRange
};

class Projectile : public sf::Drawable, public sf::Transformable {
	type m_type;
	sf::RectangleShape textureSprite;
	sf::Vector2f m_direction;
	sf::Vector2f origin;
	float m_speed = 100.0f; //pixels per second
	int damage = 1; //default damage value, can be modified based on projectile type
	bool active = true;
public:
	Projectile(type Mtype, const sf::Texture& texture, sf::Vector2f position, sf::Vector2f target) :
		m_type(Mtype), textureSprite(), active(true) {
		switch (m_type) {
		case type::turret:
			textureSprite.setSize({ 40, 40 });
			break;
		case type::rocket:
			textureSprite.setSize({ 50, 50 });
			break;
		}
		textureSprite.setTexture(&texture);
		switch (m_type) {
		case type::turret:
			m_speed = 200.0f;
			textureSprite.setTextureRect({ { 21 * 64, 12 * 64 }, { 64, 64 } });
			break;
		case type::rocket:
			m_speed = 140.0f;
			textureSprite.setTextureRect({ { 22 * 64, 10 * 64 }, { 64, 64 } });
			break;
		}
		textureSprite.setOrigin(textureSprite.getLocalBounds().size / 2.0f);
		textureSprite.setPosition(position);
		m_direction = target - position;
		origin = position;
		textureSprite.move(m_direction.normalized() * 15.f);
		float angleRad = std::atan2(m_direction.y, m_direction.x);
		float angleDeg = 0;
		switch (m_type) {
		case type::turret:
			angleDeg = angleRad * 180.0f / 3.14159265f + 270;
			break;
		case type::rocket:
			angleDeg = angleRad * 180.0f / 3.14159265f + 90;
			break;
		}
		textureSprite.setRotation(sf::degrees(angleDeg));

	}
	void setActive(bool isActive) {
		active = isActive;
	}
	const bool isActive() const {
		return active;
	}
	CollisionResult update(float deltaTime, std::vector<Enemy>& enemies, int& currency, std::vector<Explosion>& explosions, std::vector<Explosion>& deathExplosions, sf::Texture& explosionTexture, bool noMove = false) {
		if (!active) return CollisionResult::Miss;
		if (!noMove) {
			textureSprite.move(m_direction.normalized() * m_speed * deltaTime);
		}
		for (Enemy& enemy : enemies) {
			if (enemy.getIsDead()) continue;
			sf::Vector2f gap = textureSprite.getPosition() - enemy.getPosition();
			float gapF = gap.lengthSquared();
			float collisionDistance = 0;
			switch (m_type) {
			case type::turret:
				collisionDistance = 20.f;
				break;
			case type::rocket:
				collisionDistance = 25.f;
				break;
			}
			if (gapF < collisionDistance * collisionDistance) {
				setActive(false);
				enemy.addDamage(damage);
				if (m_type == type::rocket) {
					for (Enemy& otherEnemy : enemies) {
						if (&otherEnemy == &enemy || otherEnemy.getIsDead()) continue; // Skip the hit enemy and dead enemies
						float distanceToOther = (otherEnemy.getPosition() - textureSprite.getPosition()).lengthSquared();
						if (distanceToOther < 80.f * 80.f) { // AOE radius for rocket
							otherEnemy.addDamage(damage);
							//std::cout << "Rocket AOE hit! Enemy HP: " << otherEnemy.getIsDead() << std::endl;
						}
					}
					bool explosionTriggered = false;
					for (Explosion& explosion : explosions) {
						if (!explosion.isActive()) {
							explosion.callExplosion(textureSprite.getPosition());
							explosion.randomizeRotation();
							explosionTriggered = true;
							break;
						}
					}
					if (!explosionTriggered) {
						Explosion newExplosion(explosionTexture);
						newExplosion.callExplosion(textureSprite.getPosition());
						newExplosion.randomizeRotation();
						explosions.push_back(newExplosion);
					}
				}
				if (enemy.getIsDead()) {
					switch (enemy.getType()) {
					case EnemyType::normal:
						currency += 1;
						break;
					case EnemyType::tank:
						currency += 3;
						break;
					}
					bool explosionTriggered = false;
					for (Explosion& explosion : deathExplosions) {
						if (!explosion.isActive()) {
							explosion.callExplosion(enemy.getPosition(), 0.1f);
							explosion.randomizeRotation();
							explosionTriggered = true;
							break;
						}
					}
					if (!explosionTriggered) {
						Explosion newExplosion(explosionTexture, {40, 40}, 0.1f);
						newExplosion.callExplosion(enemy.getPosition(), 0.1f);
						newExplosion.randomizeRotation();
						deathExplosions.push_back(newExplosion);
					}
				}
				return CollisionResult::Hit;
			}
		}
		sf::Vector2f distance = textureSprite.getPosition() - origin;
		switch (m_type) {
		case type::turret:
			if (distance.lengthSquared() > 150.f * 150.f) {
				setActive(false);
				return CollisionResult::OutOfRange; //projectile has traveled too far without hitting anything
			}
			break;
		case type::rocket:
			if (distance.lengthSquared() > 200.f * 200.f) {
				setActive(false);
				return CollisionResult::OutOfRange; //projectile has traveled too far without hitting anything
			}
			break;
		}
		return CollisionResult::Miss; //no collision detected
	}
	void reset(type Mtype, const sf::Texture& texture, sf::Vector2f position, sf::Vector2f target) {
		m_type = Mtype;
		switch (m_type) {
		case type::turret:
			textureSprite.setSize({ 40, 40 });
			break;
		case type::rocket:
			textureSprite.setSize({ 50, 50 });
			break;
		}
		textureSprite.setTexture(&texture);
		switch (m_type) {
		case type::turret:
			m_speed = 200.0f;
			textureSprite.setTextureRect({ { 21 * 64, 12 * 64 }, { 64, 64 } });
			break;
		case type::rocket:
			m_speed = 140.0f;
			textureSprite.setTextureRect({ { 22 * 64, 10 * 64 }, { 64, 64 } });
			break;
		}
		textureSprite.setOrigin(textureSprite.getLocalBounds().size / 2.0f);
		textureSprite.setPosition(position);
		m_direction = target - position;
		origin = position;
		active = true;
		
		textureSprite.move(m_direction.normalized() * 15.f);
		float angleRad = std::atan2(m_direction.y, m_direction.x);
		float angleDeg = 0;
		switch (m_type) {
		case type::turret:
			angleDeg = angleRad * 180.0f / 3.14159265f + 270;
			break;
		case type::rocket:
			angleDeg = angleRad * 180.0f / 3.14159265f + 90;
			break;
		}
		textureSprite.setRotation(sf::degrees(angleDeg));
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		if (!active) return;
		states.transform *= getTransform();
		target.draw(textureSprite, states);
	}
};