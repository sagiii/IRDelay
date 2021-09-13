#pragma once

#include "vec2.hpp"
#include <vector>

/**
 * 2次元の位置と姿勢を記述するクラス
 */
struct Pos2 {
    Vec2 origin; // リンクの原点の空間中での位置
    Vec2 axis; // リンク座標系のx軸の空間中での表現での(単位)ベクトル
    Vec2 toGlobal(Vec2 const &local) {
        return origin + axis * local.x + axis.rotated90() * local.y;
    }
    float angle() {
        return atan2(axis.y, axis.x);
    }
};

struct Link2 : public Pos2 {
    float width, height; // リンクの寸法
    std::vector<Vec2> fourPoints(void) {
        std::vector<Vec2> points;
        points.push_back(this->toGlobal(Vec2(width / 2, 0)));
        points.push_back(this->toGlobal(Vec2(0, height / 2)));
        points.push_back(this->toGlobal(Vec2(-width / 2, 0)));
        points.push_back(this->toGlobal(Vec2(0, -height / 2)));
        return points;
    }
};

struct View {
    Vec2 origin;
    float magnify;
    unsigned int width, height;
    struct Out {
        bool left, right, above, below;
    };
    View(float orgx, float orgy, float mag, unsigned int wid, unsigned int hei)
        : origin(orgx, orgy)
        , magnify(mag)
        , width(wid)
        , height(hei)
        {}
    /**
     * 論理座標 ( 便器上 localと呼ぶ ) をディスプレイ座標に変換
     */
    Vec2 toView(Vec2 const &local)
    {
        // ディスプレイ座標なのでY軸を反転
        return Vec2(origin.x + local.x * magnify, origin.y - local.y * magnify);
    }
    /**
     * ディスプレイ座標を論理座標に変換
     */
    Vec2 toLocal(Vec2 const &view)
    {
        return Vec2((view.x - origin.x) / magnify, (view.y - origin.y) / (- magnify));
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
