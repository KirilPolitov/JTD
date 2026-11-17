#pragma once
#include <SFML/Graphics.hpp>

class MapDisplay : public sf::Drawable, public sf::Transformable {
	sf::VertexArray m_vertices;
	sf::Texture m_images;
public:
	bool load(const std::filesystem::path& tileset, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height, float scale = 1.0f) {
		if (!m_images.loadFromFile(tileset)) {
			return false;
		}
		m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
		m_vertices.resize(width * height * 6);

		for (unsigned int i = 0; i < width; ++i) {
			for (unsigned int j = 0; j < height; ++j) {
				int tileNumber = tiles[i + j * width];

				int tu = tileNumber % (m_images.getSize().x / tileSize.x); //column in texture file
				int tv = tileNumber / (m_images.getSize().x / tileSize.x); //row in texture file

				sf::Vertex* triangles = &m_vertices[(i + j * width) * 6];

				// define the 2 triangles that make up the tile
				triangles[0].position = sf::Vector2f(i * tileSize.x * scale + (i * 20) * scale + 20 * scale, j * tileSize.y * scale + (j * 20) * scale + 20 * scale);
				triangles[1].position = sf::Vector2f((i + 1) * tileSize.x * scale + (i * 20) * scale + 20 * scale, j * tileSize.y * scale + (j * 20) * scale + 20 * scale);
				triangles[2].position = sf::Vector2f(i * tileSize.x * scale + (i * 20) * scale + 20 * scale, (j + 1) * tileSize.y * scale + (j * 20) * scale + 20 * scale);

				triangles[3].position = sf::Vector2f(i * tileSize.x * scale + (i * 20) * scale + 20 * scale, (j + 1) * tileSize.y * scale + (j * 20) * scale + 20 * scale);
				triangles[4].position = sf::Vector2f((i + 1) * tileSize.x * scale + (i * 20) * scale + 20 * scale, j * tileSize.y * scale + (j * 20) * scale + 20 * scale);
				triangles[5].position = sf::Vector2f((i + 1) * tileSize.x * scale + (i * 20) * scale + 20 * scale, (j + 1) * tileSize.y * scale + (j * 20) * scale + 20 * scale);

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
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		states.transform *= getTransform();
		states.texture = &m_images;
		target.draw(m_vertices, states);
	}
};