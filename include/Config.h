#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace Config {
    // Пути к ресурсам (относительные)
    const std::string PLAYER_SPRITE = "assets/images/player.png";
    const std::string PLAYER_IDLE = "assets/craftpix-net-419402-free-base-4-direction-female-character-pixel-art/PNG/Sword/Without_shadow/Sword_Idle_without_shadow.png";
    const std::string PLAYER_RUN = "assets/craftpix-net-419402-free-base-4-direction-female-character-pixel-art/PNG/Sword/Without_shadow/Sword_Run_without_shadow.png";
    const std::string PLAYER_ATTACK = "assets/craftpix-net-419402-free-base-4-direction-female-character-pixel-art/PNG/Sword/Without_shadow/Sword_attack_without_shadow.png";
    const std::string PLAYER_HURT = "assets/craftpix-net-419402-free-base-4-direction-female-character-pixel-art/PNG/Sword/Without_shadow/Sword_Hurt_without_shadow.png";
    const std::string PLAYER_DEATH = "assets/craftpix-net-419402-free-base-4-direction-female-character-pixel-art/PNG/Sword/Without_shadow/Sword_Death_without_shadow.png";
    const std::string ENEMY_SIMPLE_IDLE = "assets/craftpix-net-208004-free-vampire-4-direction-pixel-character-sprite-pack/PNG/Vampires1/Without_shadow/Vampires1_Idle_without_shadow.png";
    const std::string ENEMY_SIMPLE_RUN = "assets/craftpix-net-208004-free-vampire-4-direction-pixel-character-sprite-pack/PNG/Vampires1/Without_shadow/Vampires1_Run_without_shadow.png";
    const std::string ENEMY_FAST_IDLE = "assets/craftpix-net-208004-free-vampire-4-direction-pixel-character-sprite-pack/PNG/Vampires2/Without_shadow/Vampires2_Idle_without_shadow.png";
    const std::string ENEMY_FAST_RUN = "assets/craftpix-net-208004-free-vampire-4-direction-pixel-character-sprite-pack/PNG/Vampires2/Without_shadow/Vampires2_Run_without_shadow.png";
    const std::string ENEMY_TANK_IDLE = "assets/craftpix-net-208004-free-vampire-4-direction-pixel-character-sprite-pack/PNG/Vampires3/Without_shadow/Vampires3_Idle_without_shadow.png";
    const std::string ENEMY_TANK_RUN = "assets/craftpix-net-208004-free-vampire-4-direction-pixel-character-sprite-pack/PNG/Vampires3/Without_shadow/Vampires3_Run_without_shadow.png";
    
    const std::string BUSH_SPRITE = "assets/craftpix-net-141354-free-top-down-bushes-pixel-art/PNG/Assets/Bush_simple1_1.png";
    const std::string CACTUS_SPRITE = "assets/craftpix-net-141354-free-top-down-bushes-pixel-art/PNG/Assets/Cactus1_1.png";
    const std::string TREE_SPRITE = "assets/craftpix-net-141354-free-top-down-bushes-pixel-art/PNG/Assets/Broken_tree1.png";
    const std::string BULLET_SPRITE = "assets/images/bullet.png";
    const std::string HEART_SPRITE = "assets/images/heart.png";
    const std::string OBSTACLE_SPRITE = "assets/images/obstacle.png";
    
    // Шрифты
    const std::string FONT_MAIN = "assets/fonts/Helvetica.ttc";
    const std::string FONT_BACKUP = "assets/fonts/Arial.ttf";
}

#endif