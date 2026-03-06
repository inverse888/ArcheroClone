#include "SoundManager.h"
#include <iostream>

SoundManager& SoundManager::getInstance() {
    static SoundManager instance;
    return instance;
}

SoundManager::SoundManager() : m_muted(false), m_sound(nullptr) {
}

void SoundManager::loadSound(SoundID id, const std::string& filename) {
}

void SoundManager::playSound(SoundID id) {
}

void SoundManager::loadMusic(const std::string& filename) {
}

void SoundManager::playMusic() {
}

void SoundManager::stopMusic() {
}

void SoundManager::setMuted(bool muted) {
    m_muted = muted;
}
