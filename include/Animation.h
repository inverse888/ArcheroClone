#ifndef ANIMATION_H
#define ANIMATION_H

#include <SFML/Graphics.hpp>
#include <vector>

class Animation {
public:
    Animation() : m_currentFrame(0), m_elapsedTime(0.0f), m_frameTime(0.1f), m_loop(true) {}

    void addFrame(sf::IntRect rect) {
        m_frames.push_back(rect);
    }

    void setFrameTime(float time) {
        m_frameTime = time;
    }

    void setLoop(bool loop) {
        m_loop = loop;
    }

    void update(float dt) {
        if (m_frames.empty()) return;

        m_elapsedTime += dt;
        if (m_elapsedTime >= m_frameTime) {
            m_elapsedTime = 0.0f;
            m_currentFrame++;
            if (m_currentFrame >= m_frames.size()) {
                if (m_loop) m_currentFrame = 0;
                else m_currentFrame = m_frames.size() - 1;
            }
        }
    }

    sf::IntRect getCurrentFrame() const {
        if (m_frames.empty()) return sf::IntRect();
        return m_frames[m_currentFrame];
    }

    bool isFinished() const {
        return !m_loop && m_currentFrame == m_frames.size() - 1;
    }

    void reset() {
        m_currentFrame = 0;
        m_elapsedTime = 0.0f;
    }

private:
    std::vector<sf::IntRect> m_frames;
    size_t m_currentFrame;
    float m_elapsedTime;
    float m_frameTime;
    bool m_loop;
};

#endif
