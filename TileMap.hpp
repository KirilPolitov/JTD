#pragma once
#include <filesystem>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>


class TileMap : public sf::Drawable, public sf::Transformable {
public:
	bool load(const std::filesystem::path& tileset, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height, float scale = 1.0f) {
		if (!m_tileset.loadFromFile(tileset)) {
			return false;
		}
		m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
		m_vertices.resize(width * height * 6);

		for (unsigned int i = 0; i < width; ++i) {
			for (unsigned int j = 0; j < height; ++j) {
				int tileNumber = tiles[i + j * width];

				int tu = tileNumber % (m_tileset.getSize().x / tileSize.x); //column in texture file
				int tv = tileNumber / (m_tileset.getSize().x / tileSize.x); //row in texture file

				sf::Vertex* triangles = &m_vertices[(i + j * width) * 6];
				
				// define the 2 triangles that make up the tile
				triangles[0].position = sf::Vector2f(i * tileSize.x * scale, j * tileSize.y * scale);
				triangles[1].position = sf::Vector2f((i + 1) * tileSize.x * scale, j * tileSize.y * scale);
				triangles[2].position = sf::Vector2f(i * tileSize.x * scale, (j + 1) * tileSize.y * scale);

				triangles[3].position = sf::Vector2f(i * tileSize.x * scale, (j + 1) * tileSize.y * scale);
				triangles[4].position = sf::Vector2f((i + 1) * tileSize.x * scale, j * tileSize.y * scale);
				triangles[5].position = sf::Vector2f((i + 1) * tileSize.x * scale, (j + 1) * tileSize.y * scale);

				// define the texture coordinates
				triangles[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
				triangles[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
				triangles[2].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);

				triangles[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
				triangles[4].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
				triangles[5].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
			}
		}
		return true;
	}
	bool updateTile(int index, int tileNumber, sf::Vector2u tileSize) {
		if (index < 0 || index >= m_vertices.getVertexCount() / 6) {
			return false; // Invalid index
		}
		int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
		int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);
		sf::Vertex* triangles = &m_vertices[index * 6];
		// Update the texture coordinates for the two triangles
		triangles[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
		triangles[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
		triangles[2].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);

		triangles[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
		triangles[4].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
		triangles[5].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
		return true;
	}
	int getTileNumber(int index, sf::Vector2u tileSize) const {
		if (index < 0 || index >= m_vertices.getVertexCount() / 6) {
			return -1; // Invalid index
		}
		const sf::Vertex* triangles = &m_vertices[index * 6];
		int tu = static_cast<int>(triangles[0].texCoords.x / tileSize.x);
		int tv = static_cast<int>(triangles[0].texCoords.y / tileSize.y);
		return tu + tv * (m_tileset.getSize().x / tileSize.x);
	}
private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		states.transform *= getTransform();
		states.texture = &m_tileset;
		target.draw(m_vertices, states);
	}
	sf::VertexArray m_vertices;
	sf::Texture m_tileset;
};