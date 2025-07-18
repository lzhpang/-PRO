#pragma once
#include "camera.h"
#include "vector2.h"
#include "player.h"
#include <graphics.h>

// 补给Buff图标资源（需在主程序加载图片）

extern IMAGE img_buff_icon_blue;
// 补给Buff类，碰到后回血
class BlueBuff {
public:
    Vector2 position;      // Buff位置
    Vector2 size;          // Buff尺寸
    bool active = true;    // 是否可拾取
    BlueBuff() {};

    // 检查玩家是否碰到Buff
    bool check_collide(Player* player) {
        const Vector2& player_pos = player->get_position();
        const Vector2& player_size = player->get_size();
        return active &&
            player_pos.x < position.x + size.x &&
            player_pos.x + player_size.x > position.x &&
            player_pos.y < position.y + size.y &&
            player_pos.y + player_size.y > position.y;
    }

    // 玩家拾取Buff，回血并禁用Buff
    void on_pick(Player* player) {
        if (check_collide(player)) {
            player->blue(25); // +25
            active = false;
        }
    }

    // 绘制Buff
    void draw(const Camera& camera) const {
        if (active) {

            putimage_alpha(
                (int)position.x - camera.get_position().x, (int)position.y - camera.get_position().y,
                (int)size.x, (int)size.y,
                &img_buff_icon_blue, 0, 0
            );
        }
    }
};