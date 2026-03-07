#include "SoundManager.h"
#include <iostream>

SoundManager& SoundManager::getInstance() {
    static SoundManager instance;
    return instance;
}

SoundManager::SoundManager() : m_muted(false), m_musicLoaded(false) {
}

void SoundManager::cleanupFinishedSounds() {
    while (!m_activeSounds.empty() &&
           m_activeSounds.front().getStatus() == sf::SoundSource::Status::Stopped) {
        m_activeSounds.pop_front();
    }
}

void SoundManager::loadSound(SoundID id, const std::string& filename) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filename)) {
        std::cout << "Warning: failed to load sound: " << filename << std::endl;
        return;
    }

    m_buffers[id] = std::move(buffer);
}

void SoundManager::playSound(SoundID id) {
    if (m_muted) return;

    auto it = m_buffers.find(id);
    if (it == m_buffers.end()) return;

    cleanupFinishedSounds();
    m_activeSounds.emplace_back(it->second);
    m_activeSounds.back().play();
}

void SoundManager::loadMusic(const std::string& filename) {
    m_musicLoaded = m_music.openFromFile(filename);
    if (!m_musicLoaded) {
        std::cout << "Warning: failed to load music: " << filename << std::endl;
        return;
    }

    m_music.setLooping(true);
    m_music.setVolume(35.0f);
}

void SoundManager::playMusic() {
    if (m_muted || !m_musicLoaded) return;
    if (m_music.getStatus() != sf::SoundSource::Status::Playing) {
        m_music.play();
    }
}

void SoundManager::stopMusic() {
    if (m_music.getStatus() != sf::SoundSource::Status::Stopped) {
        m_music.stop();
    }
}

void SoundManager::setMuted(bool muted) {
    m_muted = muted;
    if (m_muted) {
        stopMusic();
    } else {
        playMusic();
    }
}
