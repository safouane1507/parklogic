#pragma once
#include "raylib.h"
#include <map>
#include <string>

/**
 * @file AssetManager.hpp
 * @brief Manages loading, caching, and unloading of game assets.
 *
 * Currently handles Textures and Sounds (placeholder).
 * Implements the Singleton pattern for global access.
 */
class AssetManager {
public:
  /**
   * @brief Retrieves the singleton instance.
   * @return Reference to the AssetManager.
   */
  static AssetManager &Get() {
    static AssetManager instance;
    return instance;
  }

  AssetManager(const AssetManager &) = delete;
  AssetManager &operator=(const AssetManager &) = delete;

  // --- Textures ---
  /**
   * @brief Loads a texture from disk and caches it.
   * @param name Unique string identifier for the asset.
   * @param path File system path to the image.
   */
  void LoadTexture(const std::string &name, const std::string &path);

  /**
   * @brief Retrieves a cached texture.
   * @param name The unique identifier.
   * @return The Raylib Texture2D object. Returns an empty/invalid texture if not found.
   */
  Texture2D GetTexture(const std::string &name);

  /**
   * @brief Unloads a specific texture from GPU memory.
   * @param name The unique identifier.
   */
  void UnloadTexture(const std::string &name);

  // --- General ---
  /**
   * @brief Unloads all managed assets (Textures, Sounds) and clears caches.
   */
  void UnloadAll();

private:
  AssetManager() = default;
  ~AssetManager();

  std::map<std::string, Texture2D> textures;
  std::map<std::string, Sound> sounds;
};
