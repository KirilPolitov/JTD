#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
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
	std::optional<sf::RectangleShape> thrust;
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
			thrust.emplace();
			textureSprite.setSize({ 50, 50 });
			thrust->setSize({ 30, 30 });
			break;
		}
		textureSprite.setTexture(&texture);
		switch (m_type) {
		case type::turret:
			m_speed = 200.0f;
			textureSprite.setTextureRect({ { 21 * 64, 12 * 64 }, { 64, 64 } });
			break;
		case type::rocket:
			thrust->setTexture(&texture);
			m_speed = 140.0f;
			textureSprite.setTextureRect({ { 22 * 64, 10 * 64 }, { 64, 64 } });
			thrust->setTextureRect({ { 20 * 64, 12 * 64 }, { 64, 64 } });
			thrust->setOrigin(thrust->getGlobalBounds().size / 2.f);
			thrust->setPosition(position);
			thrust->move(m_direction.normalized() * -20.f);
			break;
		}
		textureSprite.setOrigin(textureSprite.getLocalBounds().size / 2.0f);
		textureSprite.setPosition(position);
		m_direction = target - position;
		origin = position;
		if (m_type != type::rocket)
			textureSprite.move(m_direction.normalized() * 15.f);
		else if (m_type == type::rocket) {
		}
		float angleRad = std::atan2(m_direction.y, m_direction.x);
		float angleDeg = 0;
		switch (m_type) {
		case type::turret:
			angleDeg = angleRad * 180.0f / 3.14159265f + 270;
			break;
		case type::rocket:
			angleDeg = angleRad * 180.0f / 3.14159265f + 90;
			thrust->setRotation(sf::degrees(angleDeg + 180));
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
			if (m_type == type::rocket) {
				thrust->setPosition(textureSprite.getPosition());
				thrust->move(m_direction.normalized() * -20.f);
			}
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
							if (otherEnemy.getIsDead()) {
								switch (otherEnemy.getType()) {
								case EnemyType::normal:
									currency += 1;
									break;
								case EnemyType::tank:
									currency += 3;
									break;
								case EnemyType::armored:
									currency += 5;
									break;
								case EnemyType::heavyArmored:
									currency += 10;
									break;
								}
								bool explosionTriggered = false;
								for (Explosion& explosion : deathExplosions) {
									if (!explosion.isActive()) {
										explosion.callExplosion(otherEnemy.getPosition(), 0.1f);
										explosion.randomizeRotation();
										explosionTriggered = true;
										break;
									}
								}
								if (!explosionTriggered) {
									Explosion newExplosion(explosionTexture, { 40, 40 }, 0.1f);
									newExplosion.callExplosion(otherEnemy.getPosition(), 0.1f);
									newExplosion.randomizeRotation();
									deathExplosions.push_back(newExplosion);
								}
							}
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
					case EnemyType::armored:
						currency += 5;
						break;
					case EnemyType::heavyArmored:
						currency += 10;
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
			thrust.emplace();
			textureSprite.setSize({ 50, 50 });
			thrust->setSize({ 30, 30 });
			break;
		}
		textureSprite.setTexture(&texture);
		switch (m_type) {
		case type::turret:
			m_speed = 200.0f;
			textureSprite.setTextureRect({ { 21 * 64, 12 * 64 }, { 64, 64 } });
			break;
		case type::rocket:
			thrust->setTexture(&texture);
			m_speed = 140.0f;
			textureSprite.setTextureRect({ { 22 * 64, 10 * 64 }, { 64, 64 } });
			thrust->setTextureRect({ { 20 * 64, 12 * 64 }, { 64, 64 } });
			thrust->setOrigin(thrust->getLocalBounds().size / 2.0f);
			thrust->setPosition(position);
			thrust->move(m_direction.normalized() * -20.f);
			break;
		}
		textureSprite.setOrigin(textureSprite.getLocalBounds().size / 2.0f);
		textureSprite.setPosition(position);
		m_direction = target - position;
		origin = position;
		active = true;
		if (m_type != type::rocket)
			textureSprite.move(m_direction.normalized() * 15.f);
		float angleRad = std::atan2(m_direction.y, m_direction.x);
		float angleDeg = 0;
		switch (m_type) {
		case type::turret:
			angleDeg = angleRad * 180.0f / 3.14159265f + 270;
			break;
		case type::rocket:
			angleDeg = angleRad * 180.0f / 3.14159265f + 90;
			thrust->setRotation(sf::degrees(angleDeg + 180));
			break;
		}
		textureSprite.setRotation(sf::degrees(angleDeg));
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		if (!active) return;
		states.transform *= getTransform();
		if (m_type == type::rocket) {
			target.draw(*thrust, states);
		}
		target.draw(textureSprite, states);
	}
};