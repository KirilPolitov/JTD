#pragma once
#include "Projectile.hpp"
#include "Enemy.hpp"

struct WaveChunk {
	EnemyType m_type;
	int amount;
	float spawnTime;
};

class Wave {
	std::vector<WaveChunk> wave;
	sf::Vector2i startPos;
	int progress;
	int chunkProgress;
	float time;
	const sf::Texture& texture;
public:
	Wave(std::vector<WaveChunk> wave, sf::Vector2i startPos, const sf::Texture& texture) :
		wave(wave), progress(0), chunkProgress(0), time(0), startPos(startPos), texture(texture) {}
	void update(std::vector<Enemy>& enemies, float deltaTime) {
		if (chunkProgress == wave.size()) return;
		time -= deltaTime;
		if (time < 0) {
			Enemy enemy(texture, startPos, wave[chunkProgress].m_type);
			enemies.push_back(enemy);

			time = wave[chunkProgress].spawnTime;
			progress++;
			if (progress == wave[chunkProgress].amount) {
				chunkProgress++;
				progress = 0;
			}
		}
	}
	bool checkStatus(std::vector<Enemy>& enemies) {
		for (const Enemy& enemy : enemies) {
			if (!enemy.getIsDead()) {
				return false;
			}
		}
		if (chunkProgress == wave.size()) {
			enemies.clear();
			return true;
		}
		return false;
	}
};