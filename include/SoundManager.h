#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <SFML/Audio.hpp>
#include <deque>
#include <map>
#include <string>

enum class SoundID {
    Hit,
    Damage,
    EnemyDeath,
    PlayerDeath,
    Victory,
    BonusPickUp
};

class SoundManager {
public:
    static SoundManager& getInstance();

    void loadSound(SoundID id, const std::string& filename);
    void playSound(SoundID id);
    
    void loadMusic(const std::string& filename);
    void playMusic();
    void stopMusic();
    
    void setMuted(bool muted);
    bool isMuted() const { return m_muted; }

private:
    SoundManager();
    void cleanupFinishedSounds();
    bool m_muted;
    bool m_musicLoaded;
    std::map<SoundID, sf::SoundBuffer> m_buffers;
    std::deque<sf::Sound> m_activeSounds;
    sf::Music m_music;
};

#endif
