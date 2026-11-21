#pragma once
#include <SFML/Audio.hpp>
#include <chrono>
#include <thread>
#include <atomic>

inline std::atomic<bool> stopSound(false);
inline std::atomic<bool> isInGame(false);
inline std::atomic<bool> playingBattleMusic(false);

static void MusicThread() {
	sf::Music ambientSound;
	if (!ambientSound.openFromFile(std::filesystem::current_path() / "sounds" / "b423b42.wav")) {
		return;
	}
	ambientSound.setLooping(true);
	ambientSound.setVolume(100);
	ambientSound.play();

	// Keep the thread alive
	while (ambientSound.getStatus() == sf::SoundSource::Status::Playing) {
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
		if (isInGame.load() && !playingBattleMusic.load()) {
			playingBattleMusic.store(true);
			ambientSound.stop();
			if (!ambientSound.openFromFile(std::filesystem::current_path() / "sounds" / "battleThemeA.wav")) {
				return;
			}
			ambientSound.setLooping(true);
			ambientSound.setVolume(25);
			ambientSound.play();
		}
		else if (!isInGame.load() && playingBattleMusic.load()) {
			playingBattleMusic.store(false);
			ambientSound.stop();
			if (!ambientSound.openFromFile(std::filesystem::current_path() / "sounds" / "b423b42.wav")) {
				return;
			}
			ambientSound.setLooping(true);
			ambientSound.setVolume(100);
			ambientSound.play();
		}
		if (stopSound.load()) {
			ambientSound.stop();
		}
	}
}