#pragma once
#include "camera.h"
#include "vector2.h"
#include "player.h"
#include <graphics.h>

// ����Buffͼ����Դ���������������ͼƬ��

extern IMAGE img_buff_icon_blue;
// ����Buff�࣬�������Ѫ
class BlueBuff {
public:
    Vector2 position;      // Buffλ��
    Vector2 size;          // Buff�ߴ�
    bool active = true;    // �Ƿ��ʰȡ
    BlueBuff() {};

    // �������Ƿ�����Buff
    bool check_collide(Player* player) {
        const Vector2& player_pos = player->get_position();
        const Vector2& player_size = player->get_size();
        return active &&
            player_pos.x < position.x + size.x &&
            player_pos.x + player_size.x > position.x &&
            player_pos.y < position.y + size.y &&
            player_pos.y + player_size.y > position.y;
    }

    // ���ʰȡBuff����Ѫ������Buff
    void on_pick(Player* player) {
        if (check_collide(player)) {
            player->blue(25); // +25
            active = false;
        }
    }

    // ����Buff
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