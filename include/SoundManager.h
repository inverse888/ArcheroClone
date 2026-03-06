#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <SFML/Audio.hpp>
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
    bool m_muted;
    std::map<SoundID, sf::SoundBuffer> m_buffers;
    std::unique_ptr<sf::Sound> m_sound;
    sf::Music m_music;
};

#endif
