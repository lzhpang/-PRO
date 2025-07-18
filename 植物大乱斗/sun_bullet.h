#pragma once
#include "bullet.h"
#include "animation.h"

extern Atlas atlas_sun;
extern Atlas atlas_sun_explode;

extern Camera main_camera;

class SunBullet : public Bullet
{
public:
	SunBullet()
	{
		size.x = 96, size.y = 96; // �չ�ը���Ĵ�С
		damage = 15; // �չ�ը�����˺�

		animation_idle.set_atlas(&atlas_sun);
		animation_idle.set_interval(50);

		animation_explode.set_atlas(&atlas_sun_explode);
		animation_explode.set_interval(50);
		animation_explode.set_loop(false);
		animation_explode.set_callback([&]() {
			can_remove = true; // ��ը����������Ϻ�����Ƴ�
			});

		IMAGE* frame_idle = animation_idle.get_frame();
		IMAGE* frame_explode = animation_explode.get_frame();
		explode_render_offset.x = (frame_idle->getwidth() - frame_explode->getwidth()) / 2.0f;
		explode_render_offset.y = (frame_idle->getheight() - frame_explode->getheight()) / 2.0f;
	}

	~SunBullet() = default;

	void on_collide()
	{
		Bullet::on_collide(); // ���ø������ײ������
		main_camera.shake(5, 250); // ���������
		mciSendString(_T("play sun_explode from 0"), NULL, 0, NULL); // ���ű�ը��Ч
	}

	void on_update(int delta)
	{
		if (valid)
		{
			velocity.y += gravity * delta; // Ӧ���������ٶ�
			position += velocity * (float)delta; // ����λ��
		}

		if (!valid)
			animation_explode.on_update(delta); // �����Ч�����±�ը����
		else
			animation_idle.on_update(delta); // �������Ĭ�϶���
		if (check_if_exceeds_screen())
			can_remove = true; // ���������Ļ��Χ��������Ƴ�
	}

	void on_draw(const Camera& camera) const
	{
		if (valid)
		{
			animation_idle.on_draw(camera, (int)position.x, (int)position.y); // ����Ĭ�϶���
		}
		else
		{
			animation_explode.on_draw(camera, (int)(position.x + explode_render_offset.x),
				(int)(position.y + explode_render_offset.y)); // ���Ʊ�ը����
		}
		Bullet::on_draw(camera);
	}
private:
	const float gravity = 1e-3f; // �չ�ը�����������ٶ�

private:
	Animation animation_idle; // �չ�ը����Ĭ�϶���
	Animation animation_explode; // �չ�ը���ı�ը����
	Vector2 explode_render_offset; // ��ը��������Ⱦƫ����
};
