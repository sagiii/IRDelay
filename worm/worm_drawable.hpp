#include <LovyanGFX.hpp>

#include "worm.hpp"
#include "color.hpp"

struct Display {
    Vec2 origin;
    float magnify;
    unsigned int width, height;
    struct Out {
        bool left, right, above, below;
    };
    Display(float orgx, float orgy, float mag, unsigned int wid, unsigned int hei)
        : origin(orgx, orgy)
        , magnify(mag)
        , width(wid)
        , height(hei)
        {}
    /**
     * 論理座標 ( 便器上 localと呼ぶ ) をディスプレイ座標に変換
     */
    Vec2 toDisplay(Vec2 const &local)
    {
        // ディスプレイ座標なのでY軸を反転
        return Vec2(origin.x + local.x * magnify, origin.y - local.y * magnify);
    }
    /**
     * ディスプレイ座標を論理座標に変換
     */
    Vec2 toLocal(Vec2 const &disp)
    {
        return Vec2((disp.x - origin.x) / magnify, (disp.y - origin.y) / (- magnify));
    }
    /**
     * 与えられたディスプレイ座標の点がディスプレイの外にいるか判定
     */
    Out isOutD(Vec2 const &pointd)
    {
        Out out;
        out.left = pointd.x < 0;
        out.right = pointd.x > width;
        out.above = pointd.y < 0;
        out.below = pointd.y > height;
        return out;
    }
    /**
     * 与えられたディスプレイ座標のリンクがディスプレイの外にいるか判定
     * リンクのwidthとheightの大きい方を用いる（回転を考慮してマージンをとる）
     */
    Out isOutD(Link2 const &linkd)
    {
        Out out;
        float r = max(linkd.width, linkd.height) / 2;
        out.left = linkd.origin.x + r < 0;
        out.right = linkd.origin.x - r > width;
        out.above = linkd.origin.y + r < 0;
        out.below = linkd.origin.y - r > height;
        return out;
    }
};

struct WormDrawable : public WormGeometry {
    Color head_color, eye_color, body_color0, body_color1;
    enum BodyColorMode {COLOR0, GRADATION, RAINBOW, RANDOM, STRIPE} body_color_mode;
    float rainbow_phase;
    float rainbow_speed; // phase/sec
    unsigned int sprite_size;
    Display display;
    LGFX_Sprite sprite;
    std::vector<Color> body_colors;
    std::vector<Link2> linksd;
    WormDrawable(WormGeometry &worm, Display &display_)
        : WormGeometry(worm)
        , head_color(255, 0, 0)
        , eye_color(255, 255, 0)
        , body_color0(0, 170, 0)
        , body_color1(0, 80, 0)
        , body_color_mode(RAINBOW)
        , rainbow_phase(0)
        , rainbow_speed(2)
        , sprite_size(80)
        , display(display_)
        {
            sprite.createSprite(sprite_size, sprite_size);
            sprite.setPivot(sprite_size / 2, sprite_size / 2);
        }
    void draw(LovyanGFX &gfx, float dt)
    {
        // tick
        tick(dt);
        rainbow_phase += rainbow_speed * dt;
        // calculate color
        body_colors.resize(division - 1);
        for (int i = 0; i < division - 1; i++) {
            switch (body_color_mode) {
                case COLOR0:
                body_colors[i] = body_color0;
                break;
                case GRADATION:
                body_colors[i] = body_color0.mix(body_color1, ratios[i]);
                break;
                case RAINBOW:
                {
                    float phase = 2 * PI * ratios[i] + rainbow_phase;
                    body_colors[i] = Color(cos_n(phase) * 255, cos_n(phase + 2. / 3 * PI) * 255, cos_n(phase + 4. / 3 * PI) * 255);
                }
                break;
                case RANDOM:
                body_colors[i] = Color(frand() * 255, frand() * 255, frand() * 255); // FIXME: もうちょっと長持ちする乱数がいい
                break;
                case STRIPE:
                body_colors[i] = (i % 2 == 0) ? body_color0 : body_color1;
                default:
                break;
            }
        }
        // coordinate conversion (ground -> display)
        linksd.resize(division);
        for (int i = 0; i < division; i++) {
            linksd[i].origin = display.toDisplay(linksg[i].origin);
            linksd[i].width = linksg[i].width * display.magnify;
            linksd[i].height = linksg[i].height * display.magnify;
            linksd[i].axis.x = linksg[i].axis.x;
            linksd[i].axis.y = -linksg[i].axis.y;
        }
        // draw
        // 一旦ラフ版仕上げる
        for (int i = 0; i < division; i++) {
            sprite.clear();
            if (i == division - 1) { // head
                sprite.setColor(head_color.to24Bit());
                sprite.fillCircle(sprite_size / 2, sprite_size / 2, sprite_size / 2);
                sprite.pushRotateZoom(&gfx, linksd[i].origin.x, linksd[i].origin.y, rad2deg(linksd[i].angle()), linksd[i].width / sprite_size, linksd[i].height / sprite_size, TFT_BLACK);
            } else { // body
                sprite.setColor(body_colors[i].to24Bit());
                sprite.fillCircle(sprite_size / 2, sprite_size / 2, sprite_size / 2);
                sprite.pushRotateZoom(&gfx, linksd[i].origin.x, linksd[i].origin.y, rad2deg(linksd[i].angle()), linksd[i].width / sprite_size, linksd[i].height / sprite_size, TFT_BLACK);
            }
            //gfx.fillCircle(linksd[i].origin.x, linksd[i].origin.y, 3, head_color.to24Bit());
        }
    }
};
