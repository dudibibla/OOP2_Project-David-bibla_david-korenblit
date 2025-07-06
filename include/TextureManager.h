#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <memory>

class TextureManager {
private:
    static std::unique_ptr<TextureManager> s_instance;
    std::map<std::string, sf::Texture> m_textures;

    TextureManager() = default;

public:
    static TextureManager& getInstance();
    static void shutdown();

    void loadTexture(const std::string& textureId, const std::string& filename);
    void loadTextureWithTransparency(const std::string& textureId, const std::string& filename, const sf::Color& transparentColor = sf::Color::White);
    const sf::Texture& getTexture(const std::string& textureId);
    bool hasTexture(const std::string& textureId) const;
    void unloadTexture(const std::string& textureId);
    void unloadAllTextures();
    void loadAllGameTextures();

    size_t getLoadedTexturesCount() const { return m_textures.size(); }
};