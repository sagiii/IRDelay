#pragma once 

#include "easing.hpp"
#include "kinematics.hpp"
#include "general.hpp"
#include <vector>

using namespace std;

/** 
 * 座標系
 * x, y に接尾辞をつけて表現。
 * 接尾辞：
 * w = worm座標：wormの形状を規定する座標空間
 * g = ground座標：地面に固定された座標空間
 * d = display座標：画面のピクセル単位の座標空間 // 描画特有
 */

/**
 * あおむしの幾何情報を記述するクラス
 * 色情報など描画特有の情報は除外（子クラスで付与予定）
 */
struct WormGeometry {
    // shape (worm座標系)
    float length;
    float thick;
    float head_magnify;
    unsigned int division;
    float width_magnify; // 全体節の幅に定数をかけて、体節を重複させて自然に見せる。
    // movement
    int direction; // +1 = +x, -1 = -x.
    float head_xg; // 接地点の位置。yは常に0なので省略。
    float tail_xg; // 接地点の位置。yは常に0なので省略。
    float phase; // 0でまっすぐ、0.5で最大屈曲、1でまっすぐ。0~0.5は頭が接地、0.5~1は尾が接地。
    float speed; // phase / sec
    float bend; // 屈曲状態。0 ~ 1が標準。1.1が限界。
    Easing ease0, ease1; // phaseからbendに変換する関係性。0 = 屈曲時、 1 = 伸展時。
    std::vector<Link2> linksw; // 直前に算出されたリンク位置。尾がインデックス[0]。
    std::vector<Link2> linksg; // linksのground座標系
    std::vector<float> ratios; // 頭から尾までのリンクの媒介変数値リスト。こちらは0 = 頭、1 = 尾。
    WormGeometry(float length_, float thick_, float head_magnify_, float width_magnify_, unsigned int division_, int direction_ = 1, float head_xg_ = 0, float tail_xg_ = 0, float phase_ = 0)
        : length(length_)
        , thick(thick_)
        , head_magnify(head_magnify_)
        , division(division_)
        , width_magnify(width_magnify_)
        , direction(direction_)
        , head_xg(head_xg_)
        , tail_xg(tail_xg_)
        , phase(phase_)
        , speed(1)
        {
            ease0.poly = Easing::QUART;
            ease0.in = true;
            ease0.out = true;
            ease1.poly = Easing::QUAD;
            ease1.in = true;
            ease1.out = true;
        }
    virtual void init()
    {
        tick(0);
    }
    void tick(float dt)
    {
        // prep
        bool is_head_grounded = phase < 0.5;
        phase += dt * speed;
        phase -= floor(phase); // 常にphaseは[0:1)
        if (phase < 0.5) {
            bend = ease0.v(2 * phase);
        } else {
            bend = 1 - ease1.v(2 * (phase - 0.5));
        }
        // 屈曲形状関数の準備
        float a = .9 + (.4 - .9) * bend + .7 * bend * (1 - bend);
        float b = 0 + (.3 - 0) * bend - .3 * bend * (1 - bend);
        // 拡大係数 (lengthになるように調整する係数)
        float mag = length / (2 * PI * .9);
        // worm座標系での更新
        linksw.resize(division);
        ratios.resize(division);
        for (int i = 0; i < division; i++) {
            float t = fmap(i, 0, division - 1, -.5 * PI, 1.5 * PI);
            linksw[i].origin.x = mag * (a * t - b * sin(2 * t));
            linksw[i].origin.y = mag * bend * sin(t);
            Vec2 axis(a - b * 2 * cos(2 * t), bend * cos(t));
            linksw[i].axis = axis.normalized();
            // リンクの寸法
            linksw[i].width = length / division * width_magnify;
            linksw[i].height = thick;
            if (i == division - 1) { // 頭だけ拡大する
                linksw[i].width *= head_magnify;
                linksw[i].height *= head_magnify;
            }
            ratios[i] = fmap(i, division - 1, 0, 0, 1);
        }
        Vec2 headw, tailw; // worm座標での頭と尾の接地点位置。
        tailw = linksw[0].toGlobal(Vec2(0, -thick / 2));
        headw = linksw[division - 1].toGlobal(Vec2(0, -thick / 2));
        //tailw = linksw[0].origin - linksw[0].axis.rotated90() * thick / 2;
        //headw = linksw[division - 1].origin - linksw[division - 1].axis.rotated90() * thick / 2;
        // ground座標系での更新
        Vec2 groundedg = Vec2(is_head_grounded ? head_xg : tail_xg, 0);
        Vec2 groundedw = is_head_grounded ? headw : tailw;
        linksg.resize(division);
        for (int i = 0; i < division; i++) {
            linksg[i].axis = linksw[i].axis;
            linksg[i].axis.x *= direction;
            Vec2 tmp = linksw[i].origin - groundedw;
            tmp.x *= direction;
            linksg[i].origin = tmp + groundedg;
            linksg[i].width = linksw[i].width;
            linksg[i].height = linksw[i].height;
        }
        head_xg = linksg[division - 1].origin.x;
        tail_xg = linksg[0].origin.x;
    }
    void setPosition(float xg, int dir)
    {
        head_xg = xg;
        direction = dir;
        tail_xg = xg - dir * linksw[0].origin.distanceFrom(linksw[division - 1].origin);
    }
};
