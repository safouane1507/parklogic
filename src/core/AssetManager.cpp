#include "core/AssetManager.hpp"
#include "core/Logger.hpp"

AssetManager::~AssetManager() { UnloadAll(); }

void AssetManager::LoadTexture(const std::string &name, const std::string &path) {
  if (textures.find(name) != textures.end()) {
    Logger::Warn("Texture already loaded: {}", name);
    return;
  }

  Texture2D tex = ::LoadTexture(path.c_str());
  if (tex.id == 0) {
    Logger::Error("Failed to load texture: {}", path);
    return;
  }

  textures[name] = tex;
  Logger::Info("Loaded texture: {}", name);
}

Texture2D AssetManager::GetTexture(const std::string &name) {
  if (textures.find(name) == textures.end()) {
    Logger::Warn("Texture not found: {}", name);
    // Return a default texture or empty
    return {0, 0, 0, 0, 0};
  }
  return textures[name];
}

void AssetManager::UnloadTexture(const std::string &name) {
  if (textures.find(name) != textures.end()) {
    ::UnloadTexture(textures[name]);
    textures.erase(name);
    Logger::Info("Unloaded texture: {}", name);
  }
}

void AssetManager::LoadSound(const std::string &name, const std::string &path) {
  if (sounds.find(name) != sounds.end()) {
    Logger::Warn("Sound already loaded: {}", name);
    return;
  }

  Sound snd = ::LoadSound(path.c_str());
  if (snd.frameCount == 0) {
    // Check if sound loaded correctly
  }

  sounds[name] = snd;
  Logger::Info("Loaded sound: {}", name);
}

Sound AssetManager::GetSound(const std::string &name) {
  if (sounds.find(name) == sounds.end()) {
    Logger::Warn("Sound not found: {}", name);
    return {0};
  }
  return sounds[name];
}

void AssetManager::UnloadSound(const std::string &name) {
  if (sounds.find(name) != sounds.end()) {
    ::UnloadSound(sounds[name]);
    sounds.erase(name);
    Logger::Info("Unloaded sound: {}", name);
  }
}

void AssetManager::UnloadAll() {
  for (auto &pair : textures) {
    ::UnloadTexture(pair.second);
  }
  textures.clear();

  for (auto &pair : sounds) {
    ::UnloadSound(pair.second);
  }
  sounds.clear();

  Logger::Info("Unloaded all assets.");
}
