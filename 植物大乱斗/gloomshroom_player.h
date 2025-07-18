#pragma once
#include "player.h"
#include <iostream>
#include "bullet.h"
#include "gloomshroom_bullet.h"
#include <cmath>
#include <vector>
#include <algorithm>

// Ģ����ɫ��ض�����Դ
extern Atlas atlas_gloomshroom_run_left;
extern Atlas atlas_gloomshroom_run_right;
extern Atlas atlas_gloomshroom_idle_left;
extern Atlas atlas_gloomshroom_idle_right;
extern Atlas atlas_gloomshroom_attack_ex_left;
extern Atlas atlas_gloomshroom_attack_ex_right;
extern Atlas atlas_gloomshroom_die_left;
extern Atlas atlas_gloomshroom_die_right;

extern Camera main_camera;

// Ģ����ɫ�࣬�̳���Player
class GloomshroomPlayer : public Player {
public:
    // ���캯������ʼ���������ߴ硢������ʱ����
    GloomshroomPlayer() {
        // ���ø����򶯻���Դ
        animation_idle_left.set_atlas(&atlas_gloomshroom_idle_left);
        animation_idle_right.set_atlas(&atlas_gloomshroom_idle_right);
        animation_run_left.set_atlas(&atlas_gloomshroom_run_left);
        animation_run_right.set_atlas(&atlas_gloomshroom_run_right);
        animation_attack_ex_left.set_atlas(&atlas_gloomshroom_attack_ex_left);
        animation_attack_ex_right.set_atlas(&atlas_gloomshroom_attack_ex_right);
        animation_die_left.set_atlas(&atlas_gloomshroom_die_left);
        animation_die_right.set_atlas(&atlas_gloomshroom_die_right);

        // ���ö���֡���
        animation_idle_left.set_interval(75);
        animation_idle_right.set_interval(75);
        animation_run_left.set_interval(75);
        animation_run_right.set_interval(75);
        animation_die_left.set_interval(150);
        animation_die_right.set_interval(150);
        animation_attack_ex_left.set_interval(50);
        animation_attack_ex_right.set_interval(50);

        // ���ý�ɫ�ߴ�
        size.x = 96;
        size.y = 96;

        // ��ʼ�����⹥����ʱ��
        timer_attack_ex.set_wait_time(1000);
        timer_attack_ex.set_one_shot(true);
        timer_attack_ex.set_callback([&]() { is_attacking_ex = false; });

        attack_cd = 250; // ��ͨ������ȴʱ��
    }

    // ������������������Ӱ���ӵ�
    ~GloomshroomPlayer() {
        // �������е�Ӱ���ӵ�
        for (auto bullet : shadow_bullets) {
            bullet->can_remove = true;
        }
        shadow_bullets.clear();
    }

    // ÿ֡���£������ɫ��Ӱ���ӵ�
    void on_update(int delta) {
        Player::on_update(delta); // ���û������
        timer_attack_ex.on_update(delta);
        // �Ƴ���ʧЧ��Ӱ���ӵ�
        shadow_bullets.erase(
            std::remove_if(shadow_bullets.begin(), shadow_bullets.end(),
                [](GloomshroomBullet* bullet) {
                    return bullet->check_can_remove();
                }),
            shadow_bullets.end());
        
    }

    // ��ͨ����������Ӱ���ӵ�
    void on_attack(){
        if (!can_attack) return; // ��ȴ�в��ܹ���
        can_attack = false;
        timer_attack_cd.restart(); // ������ͨ������ȴ

        // ���Ź�������
        is_facing_right ? animation_attack_ex_right.reset() : animation_attack_ex_left.reset();

        // ����Ӱ���ӵ�
        GloomshroomBullet* bullet = new GloomshroomBullet();
        bullet->set_shadow(true); // ���ΪӰ�ӵ�Ļ

        // ���õ�Ļλ�ã������ڽ�ɫ�ײ���
        bullet->set_position(
            position.x + (size.x - bullet->get_size().x) / 2,
            position.y + size.y - bullet->get_size().y
        );

        bullet->set_velocity(0, 0); // ��ʼ��ֹ
        bullet->set_collide_target(id == PlayerID::P1 ? PlayerID::P2 : PlayerID::P1); // ������ײĿ��
        bullet->set_callback([&]() { mp += 25; }); // ���к���������

        bullet_list.push_back(bullet);      // ���뵯Ļ�б�
        shadow_bullets.push_back(bullet);   // ����Ӱ�ӵ�Ļ�б�

        // ����Ӱ�ӵ�Ļ�������������Ƴ������
        if (shadow_bullets.size() > MAX_SHADOW_BULLETS) {
            GloomshroomBullet* oldest = shadow_bullets.front();
            oldest->can_remove = true;
            shadow_bullets.erase(shadow_bullets.begin());
        }

        // ���ŵ�Ļ��Ч
        mciSendString(L"play gloomshroom_bubbles from 0", NULL, 0, NULL);
    }

    // ���⹥������������Ӱ���ӵ������
    void on_attack_ex(){
        if (mp < 100) return; // �������㲻�ܷ���
        is_attacking_ex = true;
        timer_attack_ex.restart(); // �������⹥����ʱ��

        // �������⹥������
        is_facing_right ? animation_attack_ex_right.reset() : animation_attack_ex_left.reset();

        // ��������Ӱ�ӵ�Ļ��ʹ���ƶ�
        for (GloomshroomBullet* bullet : shadow_bullets) {
            if (!bullet->is_activated()) {
                bullet->activate();
            }
        }

        shadow_bullets.clear(); // ���Ӱ�ӵ�Ļ�б�

        // �������⹥����Ч
        mciSendString(L"play gloomshroom_ex_bubbles from 0", NULL, 0, NULL);
        mp = 0; // ��������

    }
    
private:
    Timer timer_attack_ex; // ���⹥����ʱ��
    std::vector<GloomshroomBullet*> shadow_bullets; // Ӱ�ӵ�Ļ�б�
    static const int MAX_SHADOW_BULLETS = 4; // ���Ӱ�ӵ�Ļ����
    
};