#pragma once
#include "player.h"
#include <iostream>
#include "bullet.h"
#include "gloomshroom_bullet.h"
#include <cmath>
#include <vector>
#include <algorithm>

// 蘑菇角色相关动画资源
extern Atlas atlas_gloomshroom_run_left;
extern Atlas atlas_gloomshroom_run_right;
extern Atlas atlas_gloomshroom_idle_left;
extern Atlas atlas_gloomshroom_idle_right;
extern Atlas atlas_gloomshroom_attack_ex_left;
extern Atlas atlas_gloomshroom_attack_ex_right;
extern Atlas atlas_gloomshroom_die_left;
extern Atlas atlas_gloomshroom_die_right;

extern Camera main_camera;

// 蘑菇角色类，继承自Player
class GloomshroomPlayer : public Player {
public:
    // 构造函数，初始化动画、尺寸、攻击计时器等
    GloomshroomPlayer() {
        // 设置各方向动画资源
        animation_idle_left.set_atlas(&atlas_gloomshroom_idle_left);
        animation_idle_right.set_atlas(&atlas_gloomshroom_idle_right);
        animation_run_left.set_atlas(&atlas_gloomshroom_run_left);
        animation_run_right.set_atlas(&atlas_gloomshroom_run_right);
        animation_attack_ex_left.set_atlas(&atlas_gloomshroom_attack_ex_left);
        animation_attack_ex_right.set_atlas(&atlas_gloomshroom_attack_ex_right);
        animation_die_left.set_atlas(&atlas_gloomshroom_die_left);
        animation_die_right.set_atlas(&atlas_gloomshroom_die_right);

        // 设置动画帧间隔
        animation_idle_left.set_interval(75);
        animation_idle_right.set_interval(75);
        animation_run_left.set_interval(75);
        animation_run_right.set_interval(75);
        animation_die_left.set_interval(150);
        animation_die_right.set_interval(150);
        animation_attack_ex_left.set_interval(50);
        animation_attack_ex_right.set_interval(50);

        // 设置角色尺寸
        size.x = 96;
        size.y = 96;

        // 初始化特殊攻击计时器
        timer_attack_ex.set_wait_time(1000);
        timer_attack_ex.set_one_shot(true);
        timer_attack_ex.set_callback([&]() { is_attacking_ex = false; });

        attack_cd = 250; // 普通攻击冷却时间
    }

    // 析构函数，清理所有影子子弹
    ~GloomshroomPlayer() {
        // 清理所有的影子子弹
        for (auto bullet : shadow_bullets) {
            bullet->can_remove = true;
        }
        shadow_bullets.clear();
    }

    // 每帧更新，处理角色和影子子弹
    void on_update(int delta) {
        Player::on_update(delta); // 调用基类更新
        timer_attack_ex.on_update(delta);
        // 移除已失效的影子子弹
        shadow_bullets.erase(
            std::remove_if(shadow_bullets.begin(), shadow_bullets.end(),
                [](GloomshroomBullet* bullet) {
                    return bullet->check_can_remove();
                }),
            shadow_bullets.end());
        
    }

    // 普通攻击，生成影子子弹
    void on_attack(){
        if (!can_attack) return; // 冷却中不能攻击
        can_attack = false;
        timer_attack_cd.restart(); // 重置普通攻击冷却

        // 播放攻击动画
        is_facing_right ? animation_attack_ex_right.reset() : animation_attack_ex_left.reset();

        // 创建影子子弹
        GloomshroomBullet* bullet = new GloomshroomBullet();
        bullet->set_shadow(true); // 标记为影子弹幕

        // 设置弹幕位置（居中于角色底部）
        bullet->set_position(
            position.x + (size.x - bullet->get_size().x) / 2,
            position.y + size.y - bullet->get_size().y
        );

        bullet->set_velocity(0, 0); // 初始静止
        bullet->set_collide_target(id == PlayerID::P1 ? PlayerID::P2 : PlayerID::P1); // 设置碰撞目标
        bullet->set_callback([&]() { mp += 25; }); // 命中后增加能量

        bullet_list.push_back(bullet);      // 加入弹幕列表
        shadow_bullets.push_back(bullet);   // 加入影子弹幕列表

        // 限制影子弹幕数量，超出则移除最早的
        if (shadow_bullets.size() > MAX_SHADOW_BULLETS) {
            GloomshroomBullet* oldest = shadow_bullets.front();
            oldest->can_remove = true;
            shadow_bullets.erase(shadow_bullets.begin());
        }

        // 播放弹幕音效
        mciSendString(L"play gloomshroom_bubbles from 0", NULL, 0, NULL);
    }

    // 特殊攻击，激活所有影子子弹并清空
    void on_attack_ex(){
        if (mp < 100) return; // 能量不足不能发动
        is_attacking_ex = true;
        timer_attack_ex.restart(); // 启动特殊攻击计时器

        // 播放特殊攻击动画
        is_facing_right ? animation_attack_ex_right.reset() : animation_attack_ex_left.reset();

        // 激活所有影子弹幕，使其移动
        for (GloomshroomBullet* bullet : shadow_bullets) {
            if (!bullet->is_activated()) {
                bullet->activate();
            }
        }

        shadow_bullets.clear(); // 清空影子弹幕列表

        // 播放特殊攻击音效
        mciSendString(L"play gloomshroom_ex_bubbles from 0", NULL, 0, NULL);
        mp = 0; // 能量归零

    }
    
private:
    Timer timer_attack_ex; // 特殊攻击计时器
    std::vector<GloomshroomBullet*> shadow_bullets; // 影子弹幕列表
    static const int MAX_SHADOW_BULLETS = 4; // 最大影子弹幕数量
    
};