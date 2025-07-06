#include "TextureManager.h"
#include "GameException.h"
#include <iostream>

std::unique_ptr<TextureManager> TextureManager::s_instance = nullptr;

TextureManager& TextureManager::getInstance() {
    if (!s_instance) {
        s_instance = std::unique_ptr<TextureManager>(new TextureManager());
    }
    return *s_instance;
}

void TextureManager::shutdown() {
    //m_scoreTimeManager.reset();

    if (s_instance) {
        s_instance->unloadAllTextures();
        s_instance.reset();
    }
}

void TextureManager::loadTexture(const std::string& textureId, const std::string& filename) {
    if (hasTexture(textureId)) {
        return;
    }

    sf::Texture texture;
    if (!texture.loadFromFile(filename)) {
        throw ResourceNotFoundException(textureId, filename);
    }

    texture.setSmooth(true);
    m_textures[textureId] = std::move(texture);
}

void TextureManager::loadTextureWithTransparency(const std::string& textureId, const std::string& filename, const sf::Color& transparentColor) {
    if (hasTexture(textureId)) {
        return;
    }

    sf::Image image;
    if (!image.loadFromFile(filename)) {
        throw ResourceNotFoundException(textureId, filename);
    }

    sf::Vector2u size = image.getSize();
    for (unsigned int x = 0; x < size.x; ++x) {
        for (unsigned int y = 0; y < size.y; ++y) {
            sf::Color pixel = image.getPixel(x, y);
            if (pixel.r > 240 && pixel.g > 240 && pixel.b > 240) {
                image.setPixel(x, y, sf::Color::Transparent);
            }
        }
    }

    sf::Texture texture;
    if (!texture.loadFromImage(image)) {
        throw ResourceNotFoundException(textureId, "Failed to create texture from image");
    }

    texture.setSmooth(true);
    m_textures[textureId] = std::move(texture);
}

const sf::Texture& TextureManager::getTexture(const std::string& textureId) {
    auto it = m_textures.find(textureId);
    if (it != m_textures.end()) {
        return it->second;
    }

    throw ResourceNotFoundException(textureId, "Texture not found in manager");
}

bool TextureManager::hasTexture(const std::string& textureId) const {
    return m_textures.find(textureId) != m_textures.end();
}

void TextureManager::unloadTexture(const std::string& textureId) {
    auto it = m_textures.find(textureId);
    if (it != m_textures.end()) {
        m_textures.erase(it);
    }
}

void TextureManager::unloadAllTextures() {
    m_textures.clear();
}

void TextureManager::loadAllGameTextures() {
    std::vector<std::pair<std::string, std::string>> textureConfigs = {
        {"level_0", "level_0.png"},       
        {"level_1", "level_1.png"},
		{"level_2", "level_2.png"},
		//{"background", "background.png"},
		{"car_player_yellow", "car_yellow.png"},
        {"car_player", "car_aqua.png"},
        {"parked_car_red", "car_red.png"},
        {"parked_car_blue", "car_blue.png"},
        {"parked_car_green", "car_orange.png"},
        {"moving_car", "car_pink.png"},
        {"traffic_cone", "cone.png"},
        {"parking_spot", "parking_spot.png"},
        {"youwin", "youwin.png"},
        {"gameover", "gameover.png"},
       // {"menu_background", "menu_background.png"},
        {"blank","blank.png"}
        //,
       // {"gameover", "gameover.png"}
    };

    for (const auto& config : textureConfigs) {
        if (config.first == "background" || config.first == "youwin" ||
            config.first == "menu_background" || config.first == "gameover") {
            loadTexture(config.first, config.second);
        }
        else {
            loadTextureWithTransparency(config.first, config.second);
        }
    }
}