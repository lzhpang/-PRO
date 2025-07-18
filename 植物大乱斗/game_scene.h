#pragma once
#include "scene.h"
#include "util.h"
#include "platform.h"
#include "player.h"
#include "bullet.h"
#include "status_bar.h"
#include "healbuff.h"
#include "bluebuff.h"

#include <iostream>
#include <vector>
#include <random>  // ����������

extern IMAGE img_sky;
extern IMAGE img_hills;
extern IMAGE img_platform_large;
extern IMAGE img_platform_small;

extern IMAGE img_1P_winner;                            // 1P ��ʤ�ı�ͼƬ
extern IMAGE img_2P_winner;                            // 2P ��ʤ�ı�ͼƬ
extern IMAGE img_winner_bar;                        // ��ʤ����ı�����ͼƬ

extern Camera main_camera;
extern std::vector<Platform> platforms_list;
extern std::vector<Bullet*> bullet_list;

extern Player* player_1;
extern Player* player_2;
extern IMAGE* img_player1_avatar;
extern IMAGE* img_player2_avatar;

extern SceneManager scene_manager;

class GameScene : public Scene
{
public:
    GameScene() = default;
    ~GameScene() = default;

    // ��������ȡ���Buffλ�ã��ܿ�ƽ̨�ͱ�Ե��
    Vector2 get_random_buff_position() {
        // ���尲ȫ����
        const int MIN_X = 100;
        const int MAX_X = 1100;
        const int MIN_Y = 100;
        const int MAX_Y = 450;

        // ��������棨C++11�������������
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist_x(MIN_X, MAX_X);
        std::uniform_int_distribution<> dist_y(MIN_Y, MAX_Y);

        Vector2 pos;
        int attempts = 0;
        const int MAX_ATTEMPTS = 50;  // ��ೢ��50��

        do {
            pos.x = dist_x(gen);  // X: 40~1140
            pos.y = dist_y(gen);  // Y: 100~400
            attempts++;
        } while (!is_position_valid(pos) && attempts < MAX_ATTEMPTS);

        // �����γ���ʧ�ܣ�����һ��Ĭ�ϰ�ȫλ��
        if (attempts >= MAX_ATTEMPTS) {
            pos = { (MIN_X + MAX_X) / 2, (MIN_Y + MAX_Y) / 2 };  // ���ط�Χ���ĵ�
        }

        return pos;
    }

    // ���������λ���Ƿ���Ч���ܿ�ƽ̨��
    bool is_position_valid(const Vector2& pos) {
        // ����Ƿ�̫����ƽ̨
        for (const auto& platform : platforms_list) {
            float platform_center_x = platform.render_position.x + platform.img->getwidth() / 2;
            float platform_center_y = platform.render_position.y + platform.img->getheight() / 2;

            float distance = sqrt(
                pow(pos.x - platform_center_x, 2) +
                pow(pos.y - platform_center_y, 2)
            );

            if (distance < 150) {  // 150px�ڲ�������Buff
                return false;
            }
        }
        return true;
    }

    void on_enter()
    {
        is_game_over = false;
        is_slide_out_started = false;

        pos_img_winner_bar.x = -img_winner_bar.getwidth();
        pos_img_winner_bar.y = (getheight() - img_winner_bar.getheight()) / 2;
        pos_x_img_winner_bar_dst = (getwidth() - img_winner_bar.getwidth()) / 2;

        pos_img_winner_text.x = pos_img_winner_bar.x;
        pos_img_winner_text.y = (getheight() - img_1P_winner.getheight()) / 2;
        pos_x_img_winner_text_dst = (getwidth() - img_1P_winner.getwidth()) / 2;

        timer_winner_slide_in.restart();
        timer_winner_slide_in.set_wait_time(2500);
        timer_winner_slide_in.set_one_shot(true);
        timer_winner_slide_in.set_callback([&]() {
            is_slide_out_started = true;
            });

        timer_winner_slide_out.restart();
        timer_winner_slide_out.set_wait_time(1000);
        timer_winner_slide_out.set_one_shot(true);
        timer_winner_slide_out.set_callback([&]() {
            scene_manager.switch_to(SceneManager::SceneType::Menu);
            });

        status_bar_1P.set_avatar(img_player1_avatar);
        status_bar_2P.set_avatar(img_player2_avatar);
        status_bar_1P.set_position(235, 625);
        status_bar_2P.set_position(675, 625);

        player_1->set_position(200, 50);
        player_2->set_position(975, 50);

        pos_img_sky.x = (getwidth() - img_sky.getwidth()) / 2;
        pos_img_sky.y = (getheight() - img_sky.getheight()) / 2;
        pos_img_hills.x = (getwidth() - img_hills.getwidth()) / 2;
        pos_img_hills.y = (getheight() - img_hills.getheight()) / 2;

        platforms_list.resize(4);

        Platform& large_platform = platforms_list[0];
        large_platform.img = &img_platform_large;
        large_platform.render_position.x = 122;
        large_platform.render_position.y = 455;
        large_platform.shape.left = (float)large_platform.render_position.x + 30;
        large_platform.shape.right = (float)large_platform.render_position.x + img_platform_large.getwidth() - 30;
        large_platform.shape.y = (float)large_platform.render_position.y + 60;

        Platform& small_platform_1 = platforms_list[1];
        small_platform_1.img = &img_platform_small;
        small_platform_1.render_position.x = 175;
        small_platform_1.render_position.y = 360;
        small_platform_1.shape.left = (float)small_platform_1.render_position.x + 40;
        small_platform_1.shape.right = (float)small_platform_1.render_position.x + img_platform_small.getwidth() - 40;
        small_platform_1.shape.y = (float)small_platform_1.render_position.y + img_platform_small.getheight() / 2;

        Platform& small_platform_2 = platforms_list[2];
        small_platform_2.img = &img_platform_small;
        small_platform_2.render_position.x = 855;
        small_platform_2.render_position.y = 360;
        small_platform_2.shape.left = (float)small_platform_2.render_position.x + 40;
        small_platform_2.shape.right = (float)small_platform_2.render_position.x + img_platform_small.getwidth() - 40;
        small_platform_2.shape.y = (float)small_platform_2.render_position.y + img_platform_small.getheight() / 2;

        Platform& small_platform_3 = platforms_list[3];
        small_platform_3.img = &img_platform_small;
        small_platform_3.render_position.x = 515;
        small_platform_3.render_position.y = 225;
        small_platform_3.shape.left = (float)small_platform_3.render_position.x + 40;
        small_platform_3.shape.right = (float)small_platform_3.render_position.x + img_platform_small.getwidth() - 40;
        small_platform_3.shape.y = (float)small_platform_3.render_position.y + img_platform_small.getheight() / 2;

        mciSendString(_T("play bgm_game repeat from 0"), NULL, 0, NULL);
    };

    void on_update(int delta)
    {
        player_1->on_update(delta);
        player_2->on_update(delta);

        main_camera.on_update(delta);

        bullet_list.erase(std::remove_if(bullet_list.begin(), bullet_list.end(),
            [](const Bullet* bullet) {
                bool deletable = bullet->check_can_remove();
                if (deletable) delete bullet;
                return deletable;
            }), bullet_list.end());

        for (auto t : bullet_list)
            t->on_update(delta);

        const Vector2& position_player_1 = player_1->get_position();
        const Vector2& position_player_2 = player_2->get_position();
        if (position_player_1.y >= getheight()) player_1->set_hp(0);
        if (position_player_2.y >= getheight()) player_2->set_hp(0);
        if (player_1->get_hp() <= 0 || player_2->get_hp() <= 0)
        {
            if (!is_game_over)
            {
                mciSendString(_T("stop bgm_game"), NULL, 0, NULL);
                mciSendString(_T("play ui_win from 0"), NULL, 0, NULL);
            }
            is_game_over = true;
        }

        status_bar_1P.set_hp(player_1->get_hp());
        status_bar_1P.set_mp(player_1->get_mp());
        status_bar_2P.set_hp(player_2->get_hp());
        status_bar_2P.set_mp(player_2->get_mp());

        if (is_game_over)
        {
            pos_img_winner_bar.x += (int)(speed_winner_bar * delta);
            pos_img_winner_text.x += (int)(speed_winner_text * delta);
            if (!is_slide_out_started)
            {
                timer_winner_slide_in.on_update(delta);
                if (pos_img_winner_bar.x > pos_x_img_winner_bar_dst)
                    pos_img_winner_bar.x = pos_x_img_winner_bar_dst;
                if (pos_img_winner_text.x > pos_x_img_winner_text_dst)
                    pos_img_winner_text.x = pos_x_img_winner_text_dst;
            }
            else
                timer_winner_slide_out.on_update(delta);
        }

        // ����Buffˢ��
        heal_buff_timer += delta;
        if (heal_buff_timer >= HEAL_BUFF_INTERVAL) {
            heal_buff_timer = 0;
            place_heal_buff = true;
            // ������Ҫʱ������Buff
            if (!heal_buff || !heal_buff->active) {
                delete heal_buff;
                heal_buff = new HealBuff();
                heal_buff->position = get_random_buff_position();  // ʹ�øĽ������λ��
            }
        }

        // ��������Buff��ײ
        if (heal_buff && heal_buff->active) {
            heal_buff->check_collide(player_1);
            heal_buff->check_collide(player_2);
            heal_buff->on_pick(player_1);
            heal_buff->on_pick(player_2);
        }

        // ��Buffˢ��
        blue_buff_timer += delta;
        if (blue_buff_timer >= BLUE_BUFF_INTERVAL) {
            blue_buff_timer = 0;
            place_blue_buff = true;
            // ������Ҫʱ������Buff
            if (!blue_buff || !blue_buff->active) {
                delete blue_buff;
                blue_buff = new BlueBuff();
                blue_buff->position = get_random_buff_position();  // ʹ�øĽ������λ��
            }
        }

        // ��������Buff��ײ
        if (blue_buff && blue_buff->active) {
            blue_buff->check_collide(player_1);
            blue_buff->check_collide(player_2);
            blue_buff->on_pick(player_1);
            blue_buff->on_pick(player_2);
        }
    };

    void on_draw(const Camera& camera)
    {
        putimage_alpha(camera, pos_img_sky.x, pos_img_sky.y, &img_sky);
        putimage_alpha(camera, pos_img_hills.x, pos_img_hills.y, &img_hills);

        for (const Platform& platform : platforms_list)
            platform.on_draw(camera);

        if (is_debug)
        {
            settextcolor(RGB(255, 0, 0));
            outtextxy(15, 15, _T("�ѿ�������ģʽ����'Q'���ر�"));
        }

        player_1->on_draw(camera);
        player_2->on_draw(camera);

        for (auto t : bullet_list)
            t->on_draw(camera);

        if (is_game_over)
        {
            putimage_alpha(pos_img_winner_bar.x, pos_img_winner_bar.y, &img_winner_bar);
            putimage_alpha(pos_img_winner_text.x, pos_img_winner_text.y, player_1->get_hp() > 0 ? &img_1P_winner : &img_2P_winner);
        }
        else
        {
            status_bar_1P.on_draw();
            status_bar_2P.on_draw();
        }

        // ����Buff
        if (place_heal_buff && heal_buff && heal_buff->active)
            heal_buff->draw(camera);
        if (place_blue_buff && blue_buff && blue_buff->active)
            blue_buff->draw(camera);
    };

    void on_input(const ExMessage& msg)
    {
        player_1->on_input(msg);
        player_2->on_input(msg);

        switch (msg.message)
        {
        case WM_KEYUP:
            if (msg.vkcode == 0x51) // 'Q' key
                is_debug = !is_debug;
            break;
        default:
            break;
        };
    };

    void on_exit()
    {
        delete player_1; player_1 = nullptr;
        delete player_2; player_2 = nullptr;
        is_debug = false;
        bullet_list.clear();
        main_camera.reset();
    };

    bool place_heal_buff = false; // �Ƿ��������Buff
    bool place_blue_buff = false; // �Ƿ������Buff

private:
    const float speed_winner_bar = 3.0f;    // ���㶯Ч�����ƶ��ٶ�
    const float speed_winner_text = 1.5f;    // ���㶯Ч�ı��ƶ��ٶ�

    POINT pos_img_sky = { 0 };
    POINT pos_img_hills = { 0 };

    StatusBar status_bar_1P;    // ��� 1 ��״̬��
    StatusBar status_bar_2P;    // ��� 2 ��״̬��

    bool is_game_over = false;    // ��Ϸ�Ƿ����

    POINT pos_img_winner_bar = { 0 };    // ���㶯Ч����λ��
    POINT pos_img_winner_text = { 0 };    // ���㶯Ч�ı�λ��
    int pos_x_img_winner_bar_dst = 0;    // ���㶯Ч�����ƶ���Ŀ��λ��
    int pos_x_img_winner_text_dst = 0;    // ���㶯Ч�ı��ƶ���Ŀ��λ��
    Timer timer_winner_slide_in;        // ���㶯Ч���붨ʱ��
    Timer timer_winner_slide_out;        // ���㶯Ч������ʱ��
    bool is_slide_out_started = false;    // ���㶯Ч�Ƿ�ʼ����

    HealBuff* heal_buff = nullptr;
    BlueBuff* blue_buff = nullptr;
    int heal_buff_timer = 0; // ���ڶ�ʱˢ������Buff
    int blue_buff_timer = 3; // ���ڶ�ʱˢ����Buff
    const int HEAL_BUFF_INTERVAL = 4000; // 4��ˢ��һ��
    const int BLUE_BUFF_INTERVAL = 3000; // 3��ˢ��һ��
};