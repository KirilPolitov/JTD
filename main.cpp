#include <SFML/Graphics.hpp>
#include "TileMap.hpp"
#include "Enemy.hpp"
#include "MapDisplay.hpp"
#include "Tower.hpp"
#include "WaveManager.hpp"
#include "Explosion.hpp"
//#include "json.hpp"

enum class gameState { //add more states as needed
	menu,
	game,
	settings,
	mapSelect,
	endScreen
};

int main() {
	srand(time(0));
	sf::VideoMode videoMode({ 1024, 1024 });
	sf::RenderWindow window(videoMode, "JTD");
	window.setFramerateLimit(60); //temporary, for testing purposes
	std::filesystem::path exePath = std::filesystem::current_path();
	std::filesystem::path tilesetPath = std::filesystem::current_path() / "textures/tileset.png";
	sf::Font font(exePath / "fonts/AppleGaramond.ttf");
#pragma region game
	int spawnTile = 158; //temp
	std::array<int, 256> tiles = {
	157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,
	157,157,157,157,158,158,158,158,158,157,157,157,157,157,157,157,
	158,158,158,157,158,157,157,157,158,157,157,157,157,157,157,157,
	157,157,158,157,158,157,157,158,158,157,157,157,157,157,157,157,
	157,157,158,158,158,157,157,158,157,157,157,157,157,157,157,157,
	157,157,157,157,157,157,157,158,157,157,157,157,157,157,157,157,
	157,157,157,157,157,157,157,158,158,158,158,158,158,158,157,157,
	157,157,157,157,157,157,157,157,157,157,157,157,157,158,157,157,
	157,157,157,157,157,157,157,157,157,157,157,157,157,158,157,157,
	157,157,157,157,158,158,158,158,158,158,157,157,157,158,157,157,
	157,157,157,157,158,157,157,157,157,158,157,157,157,158,157,157,
	158,158,158,158,158,157,157,157,157,158,158,158,158,158,157,157,
	157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,
	157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,
	157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,
	157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157

	};
	sf::View view(sf::FloatRect({ 0, 0 }, { 1024, 1024 }));
	window.setView(view);
	TileMap tileMap;
	if (!tileMap.load(tilesetPath, sf::Vector2u(64, 64), tiles.data(), 16, 16)) {
		return -1;
	}
	sf::Texture tileSet;
	if (!tileSet.loadFromFile(tilesetPath)) {
		return -1;
	}
	std::filesystem::path mapSelectPath = exePath / "textures" / "MapSelect.png";
	sf::IntRect tileRect({ 18 * 64, 11 * 64 }, { 64, 64 });
	//enemy.printDebug();
	Tower tower(type::turret, tileSet, { 0, 0 });
	std::vector<Tower> towers;
	towers.reserve(8);
	sf::CircleShape rangeIndicator;
	rangeIndicator.setOrigin(rangeIndicator.getGeometricCenter());
	rangeIndicator.setFillColor(sf::Color(0, 0, 0, 100));
	bool towerSelected = false;
	std::vector<Projectile> turretProjectiles;
	turretProjectiles.reserve(16);
	std::vector<Projectile> rocketProjectiles;
	rocketProjectiles.reserve(16);
	std::vector<Enemy> enemies;
	enemies.reserve(8);
	bool waveIsActive = false;
	int activeWaveIndex = 0;
	std::vector<Wave> waves;
	waves.reserve(5);
	waves.push_back(Wave(std::vector<WaveChunk>{{ EnemyType::normal, 5, 2.f }}, { 0, 2 }, tileSet));
	waves.push_back(Wave(std::vector<WaveChunk>{{ EnemyType::tank, 1, 1.f }, { EnemyType::normal, 5, 1.f }}, { 0, 2 }, tileSet));
	waves.push_back(Wave(std::vector<WaveChunk>{{ EnemyType::tank, 1, 1.f }, { EnemyType::normal, 1, 1.f }, { EnemyType::tank, 1, 1.f }, { EnemyType::normal, 1, 1.f }, { EnemyType::tank, 1, 1.f }}, { 0, 2 }, tileSet));
	waves.push_back(Wave(std::vector<WaveChunk>{{ EnemyType::tank, 8, 1.f }}, { 0, 2 }, tileSet));
	waves.push_back(Wave(std::vector<WaveChunk>{{ EnemyType::tank, 15, 0.5f }}, { 0, 2 }, tileSet));
	std::vector<Explosion> explosions;
	sf::Texture explosionTexture;
	if (!explosionTexture.loadFromFile(exePath / "textures" / "Explosion.png")) {
		return -1;
	}
	/*
	std::vector<sf::Sprite> explosionSprites;
	explosionSprites.reserve(8);*/
	//Wave wave1(std::vector<WaveChunk>{{ EnemyType::normal, 5, 2.f }}, { 0, 2 }, tileSet);
	//enemies.emplace_back(Enemy(tileSet, { 0, 2 }, EnemyType::tank));
	//enemies[0].setTextureRect(tileRect);
	int currency = 5;
	int lives = 10;
	sf::Text currencyText(font);
	currencyText.setCharacterSize(48);
	currencyText.setFillColor(sf::Color::Black);
	currencyText.setPosition({ 10, 10 });
	sf::Text livesText(font);
	livesText.setCharacterSize(48);
	livesText.setFillColor(sf::Color::Black);
	livesText.setOrigin({ livesText.getLocalBounds().size.x, 0 });
	livesText.setPosition({ static_cast<float>(window.getSize().x) - 10, 10 });
	sf::Text waveText(font);
	waveText.setCharacterSize(48);
	waveText.setFillColor(sf::Color::Black);
	waveText.setPosition({ 10, 60 });
	sf::Text endScreenText(font);
	endScreenText.setCharacterSize(96);
	endScreenText.setFillColor(sf::Color::White);
#pragma endregion
#pragma region menu
	sf::Texture logoTexture;
	if (!logoTexture.loadFromFile(std::filesystem::current_path() / "textures/logo.png")) {
		return -1;
	}
	sf::Sprite logoSprite(logoTexture);
	sf::RectangleShape buttonMapSelect({ 200, 50 });
	logoSprite.setOrigin(logoSprite.getLocalBounds().size / 2.0f);
	logoSprite.setPosition({ window.getSize().x / 2.0f, window.getSize().y / 4.0f });
	buttonMapSelect.setFillColor(sf::Color::Blue);
	buttonMapSelect.setOrigin(buttonMapSelect.getSize() / 2.0f);
	buttonMapSelect.setPosition({ window.getSize().x / 2.0f, window.getSize().y / 2.0f });
	sf::Text mapSelectText(font);
	mapSelectText.setString("Select Map");
	mapSelectText.setCharacterSize(24);
	mapSelectText.setOrigin({mapSelectText.getLocalBounds().size.x / 2.0f, mapSelectText.getLocalBounds().size.y / 2.0f});
	mapSelectText.setPosition(buttonMapSelect.getPosition());
	mapSelectText.setFillColor(sf::Color::White);
#pragma endregion
	sf::Clock clock;
	gameState gameState = gameState::menu;
	std::array <int, 6> mapSelectTiles = {
		0, 1, 2,
		3, 4, 5
	};
	MapDisplay mapSelectDisplay;
	float scale = 3.8f;
	mapSelectDisplay.load(mapSelectPath, { 64, 64 }, mapSelectTiles.data(), 3, 2, scale);
	while (window.isOpen()) {
		float deltaTime = clock.restart().asSeconds();
		while (std::optional<sf::Event> opt = window.pollEvent()) {
			sf::Event event = *opt;
			if (event.is<sf::Event::Closed>()) {
				window.close();
			}
			if (gameState == gameState::menu) {
				if (event.is<sf::Event::MouseMoved>()) {
					sf::Vector2f mousePos = window.mapPixelToCoords({ sf::Mouse::getPosition(window) });
					if (buttonMapSelect.getGlobalBounds().contains(mousePos)) {
						buttonMapSelect.setFillColor(sf::Color::Red);
					}
					else {
						buttonMapSelect.setFillColor(sf::Color::Blue);
					}
				}
				if (event.is<sf::Event::MouseButtonPressed>()) {
					if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
						sf::Vector2f mousePos = window.mapPixelToCoords({ sf::Mouse::getPosition(window) });
						if (buttonMapSelect.getGlobalBounds().contains(mousePos)) {
							gameState = gameState::mapSelect;
						}
					}
				}
			}
			else if (gameState == gameState::mapSelect) {
				if (event.is<sf::Event::MouseButtonPressed>()) {
					if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
						sf::Vector2f mousePos = window.mapPixelToCoords({ sf::Mouse::getPosition(window) });
						//fix this to work with the scale of the map display, and also account for the offset of the map display
						int horizontalIndex = static_cast<int>(mousePos.x / (64.0f * scale + 20 * scale));
						int verticalIndex = static_cast<int>(mousePos.y / (64.0f * scale + 20 * scale));
						std::cout << "Mouse clicked at: " << mousePos.x << ", " << mousePos.y << std::endl << "Horizontal Index: " << horizontalIndex << ", Vertical Index: " << verticalIndex << std::endl;
						if (horizontalIndex < 0 || horizontalIndex >= 3 || verticalIndex < 0 || verticalIndex >= 2 || mousePos.x < 20 * scale || mousePos.y < 20 * scale ||
						mousePos.x < horizontalIndex * 64 * scale + 20 * scale + (20 * horizontalIndex) * scale || mousePos.y < verticalIndex * 64 * scale + 20 * scale + (20 * verticalIndex) * scale) {
							continue;
						}
						int index = verticalIndex * 3 + horizontalIndex;
						int selectedTile = mapSelectTiles[index];
						// load map from json into map array and switch game state to game
						//temporary code ahead
						gameState = gameState::game;
					}
				}
			}
			else if (gameState == gameState::game) {
				if (event.is<sf::Event::KeyPressed>()) {
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1) && !tower.isActive()) {
						tower.reset(type::turret, window.mapPixelToCoords({ sf::Mouse::getPosition(window) }));
						tower.toggleActive();
						rangeIndicator.setRadius(tower.getRange());
						rangeIndicator.setOrigin(rangeIndicator.getGeometricCenter());
					}
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2) && !tower.isActive()) {
						tower.reset(type::rocket, window.mapPixelToCoords({ sf::Mouse::getPosition(window) }));
						tower.toggleActive();
						rangeIndicator.setRadius(tower.getRange());
						rangeIndicator.setOrigin(rangeIndicator.getGeometricCenter());
					}
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) && tower.isActive())
						tower.toggleActive();
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) && towerSelected) {
						towerSelected = false;
					}
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && !waveIsActive && activeWaveIndex < waves.size()) {
						waveIsActive = true;
					}
				}
				if (event.is<sf::Event::MouseButtonPressed>()) {
					if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
						if (tower.isActive()) {
							if (tower.place(tower.getPosition(), tiles.data(), spawnTile, towers, currency)) {
								tower.setAlpha(1);
								tower.setRed(1);
								tower.setState(state::ready);
								towers.push_back(tower);
								tower.toggleActive();
								tower.setAlpha(0.7f);
								tower.setState(state::held);
								switch (tower.getType()) {
								case type::turret:
									currency -= 5;
									break;
								case type::rocket:
									currency -= 10;
									break;
								}
							}
							else tower.setRed(0.5f);
						}
						else {
							for (Tower& tower : towers) {
								if (sf::Vector2f(tower.getPosition().x - sf::Mouse::getPosition(window).x, tower.getPosition().y - sf::Mouse::getPosition(window).y).length() < 15) {
									rangeIndicator.setRadius(tower.getRange());
									rangeIndicator.setOrigin(rangeIndicator.getGeometricCenter());
									rangeIndicator.setPosition(tower.getPosition());
									towerSelected = true;
								}
							}
						}
					}
				}
			}
		}
		switch (gameState) {
			case gameState::menu:
				window.clear(sf::Color::Black);
				window.draw(logoSprite);
				window.draw(buttonMapSelect);
				window.draw(mapSelectText);
				break;
			case gameState::mapSelect:
				window.clear(sf::Color::Black);
				window.draw(mapSelectDisplay);
				break;
			case gameState::endScreen:
				window.clear(sf::Color::Black);
				window.draw(endScreenText);
				break;
			case gameState::game:
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::G) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::M) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
					currency += 100;
				}
				//enemy.update(deltaTime, tiles.data(), 16, 16);
				if (waveIsActive) {
					waves[activeWaveIndex].update(enemies, deltaTime);
					if (waves[activeWaveIndex].checkStatus(enemies)) {
						activeWaveIndex++;
						waveIsActive = false;
						if (activeWaveIndex >= waves.size()) {
							waveIsActive = false;
							gameState = gameState::endScreen;
							endScreenText.setString("You Win!");
							endScreenText.setOrigin(endScreenText.getLocalBounds().size / 2.0f);
							endScreenText.setPosition({ window.getSize().x / 2.0f, window.getSize().y / 2.0f });
						}
					}
					if (lives <= 0) {
						waveIsActive = false;
						gameState = gameState::endScreen;
						endScreenText.setString("Game Over");
						endScreenText.setOrigin(endScreenText.getLocalBounds().size / 2.0f);
						endScreenText.setPosition({ window.getSize().x / 2.0f, window.getSize().y / 2.0f });
					}
				}
				if (tower.isActive()) {
					tower.setPosition(window.mapPixelToCoords({ sf::Mouse::getPosition(window) }));
					rangeIndicator.setPosition(tower.getPosition());
				}
				for (Enemy& enemy : enemies) {
					enemy.update(deltaTime, tiles.data(), 16, 16, lives);
				}
				for (Tower& towerI : towers) {
					if (waveIsActive)
						towerI.updateCooldown(deltaTime);
					//if (towerI.getType() == type::turret) {
						for (const Enemy& enemy : enemies) {
							if (enemy.getIsDead()) continue; // Skip dead enemies
							sf::Vector2f direction = enemy.getPosition() - towerI.getPosition();
							float distance = direction.length();

							if (distance <= towerI.getRange() && towerI.getType() == type::turret) {
								towerI.shoot(enemy.getPosition(), turretProjectiles, tileSet);

								std::cout << turretProjectiles.size() << std::endl;
								break;
							}
							else if (distance <= towerI.getRange() && towerI.getType() == type::rocket) {
								towerI.shoot(enemy.getPosition(), rocketProjectiles, tileSet);
								std::cout << rocketProjectiles.size() << std::endl;
								break;
							}
						}
					//}
				}
				window.clear(sf::Color::Black);
				window.draw(tileMap);
				for (Enemy& enemy : enemies) {
					window.draw(enemy);
				}
				//window.draw(enemy);
				for (int i = 0; i < towers.size(); i++) {
					window.draw(towers[i]);
				}
				for (Projectile& projectile : turretProjectiles) {
					if (projectile.isActive()) {
						CollisionResult result = projectile.update(deltaTime, enemies, currency, explosions, explosionTexture);

						if (result == CollisionResult::Hit) {
							// Hit logic handled inside update, nothing extra needed here
						}
						else if (result == CollisionResult::OutOfRange) {
							projectile.setActive(false);
						}

						window.draw(projectile);
					}
				}
				for (Projectile& projectile : rocketProjectiles) {
					if (projectile.isActive()) {
						CollisionResult result = projectile.update(deltaTime, enemies, currency, explosions, explosionTexture);
						if (result == CollisionResult::Hit) {
							// Hit logic handled inside update, nothing extra needed here
						}
						else if (result == CollisionResult::OutOfRange) {
							projectile.setActive(false);
						}
						window.draw(projectile);
					}
				}
				if (towerSelected && !tower.isActive()) {
					window.draw(rangeIndicator);
				}
				if (tower.isActive()) {
					if (!tower.place(tower.getPosition(), tiles.data(), spawnTile, towers, currency))
						tower.setRed(0.5);
					else tower.setRed(1);
					window.draw(rangeIndicator);
					window.draw(tower);
				}
				currencyText.setString("Currency: " + std::to_string(currency));
				window.draw(currencyText);
				waveText.setString("Wave: " + std::to_string(activeWaveIndex + 1) + "/" + std::to_string(waves.size()));
				window.draw(waveText);
				livesText.setString("Lives: " + std::to_string(lives));
				livesText.setOrigin({ livesText.getLocalBounds().size.x, 0 });
				window.draw(livesText);
				for (Explosion& explosion : explosions) {
					explosion.updateCooldown(deltaTime);
					window.draw(explosion);
				}
		}
		window.display();
	}
	return 0;
} 