#pragma once
#include "bullet.h"
#include "animation.h"
#include <cmath>
#include <functional>
#include"player.h"

extern Atlas atlas_gloomshroom_idle_left;
extern Atlas atlas_gloomshroom_bubbles;

class GloomshroomBullet : public Bullet {
public:
    GloomshroomBullet()
    {
        size.x = 96, size.y = 96;
        damage = 15;

        animation_idle.set_atlas(&atlas_gloomshroom_idle_left);
        animation_idle.set_interval(50);

        animation_explode.set_atlas(&atlas_gloomshroom_bubbles);
        animation_explode.set_interval(50);
        animation_explode.set_loop(false);
        animation_explode.set_callback([&]() {
            can_remove = true;
            });
        IMAGE* frame_idle = animation_idle.get_frame();
        IMAGE* frame_explode = animation_explode.get_frame();

        explode_render_offset.x = (frame_idle->getwidth() - frame_explode->getwidth()) / 2.0f;
        explode_render_offset.y = (frame_idle->getheight() - frame_explode->getheight()) / 2.0f;
        velocity.x = 0;
        velocity.y = 0;
    }
    ~GloomshroomBullet() = default;

    void on_collide(){
        Bullet::on_collide();
        is_explode = true;
        mciSendString(_T("play gloomshroom_bubbles from 0"), NULL, 0, NULL);
    }
   
    void on_update(int delta) {
        if (is_explode) {
            animation_explode.on_update(delta);
            if (animation_explode.check_finished()) {
                can_remove = true;
            }
        }
        else {
            animation_idle.on_update(delta);

            if (is_shadow && activated) {
                position.x += velocity.x * delta;

                if (check_if_exceeds_screen()) {
                    can_remove = true;
                }
            }
        }
    }

    void on_draw(const Camera& camera) const{
        if (is_explode) {
            animation_explode.on_draw(camera,
                (int)(position.x + explode_render_offset.x),
                (int)(position.y + explode_render_offset.y));
        }
        else {
            animation_idle.on_draw(camera, (int)position.x, (int)position.y);
        }

        Bullet::on_draw(camera);
    }

    void set_shadow(bool shadow) { is_shadow = shadow; }
    bool is_shadow_bullet() const { return is_shadow; }

    void activate() {
        activated = true;
        velocity.x = 0.5f;
    }

    bool is_activated() const { return activated; }

private:
    Animation animation_idle;
    Animation animation_explode;
    Vector2 explode_render_offset;
    bool is_explode = false;
    bool is_shadow = false;     
    bool activated = false;     
};