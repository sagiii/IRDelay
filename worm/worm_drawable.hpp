#include <LovyanGFX.hpp>

#include "worm.hpp"
#include "color.hpp"

/**
 * 描画処理を記述する層
 */
struct WormDrawable : public WormGeometry {
    Color head_color, eye_color, pupil_color, body_color0, body_color1;
    float eye_x, eye_y, eye_r, pupil_r, nose_y_top, nose_y_bottom, nose_x;
    enum BodyColorMode {COLOR0, GRADATION, RAINBOW, RANDOM, STRIPE} body_color_mode;
    float rainbow_phase;
    float rainbow_speed; // phase/sec
    unsigned int sprite_size;
    View view;
    LGFX_Sprite sprite;
    std::vector<Color> body_colors;
    std::vector<Link2> linksd;
    WormDrawable(WormGeometry const &worm, View const &view_)
        : WormGeometry(worm)
        , head_color(200, 0, 0)
        , eye_color(255, 255, 0)
        , pupil_color(80, 200, 140)
        , eye_x(.4), eye_y(.3), eye_r(.3), pupil_r(.2), nose_y_top(-.1), nose_y_bottom(-0.4), nose_x(.15)
        , body_color0(10, 170, 30)
        , body_color1(10, 80, 20)
        , body_color_mode(GRADATION)
        , rainbow_phase(0)
        , rainbow_speed(.3)
        , sprite_size(80)
        , view(view_)
        {
            sprite.createSprite(sprite_size, sprite_size);
            sprite.setPivot(sprite_size / 2, sprite_size / 2);
            sprite.setColorDepth(24);
        }
    ~WormDrawable()
    {
        sprite.deleteSprite();
    }
    void draw(LovyanGFX &gfx, float dt)
    {
        // tick
        tick(dt);
        rainbow_phase += rainbow_speed * dt * 2 * PI;
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
        // coordinate conversion (ground -> view)
        linksd.resize(division);
        for (int i = 0; i < division; i++) {
            linksd[i].origin = view.toView(linksg[i].origin);
            linksd[i].width = linksg[i].width * view.magnify;
            linksd[i].height = linksg[i].height * view.magnify;
            linksd[i].axis.x = linksg[i].axis.x;
            linksd[i].axis.y = -linksg[i].axis.y;
        }
        // draw
        for (int i = 0; i < division; i++) {
            sprite.clear();
            if (i == division - 1) { // head
                sprite.setColor(head_color.to24Bit());
                float mag = sprite_size / 2;
                float x0 = mag, y0 = mag;
                // 顔座標は中心を原点とするx,yとも±1で記述している。
                // sprite座標系への変換式は、p_sprite = p_face * mag + p_0
                sprite.fillCircle(sprite_size / 2, sprite_size / 2, sprite_size / 2);
                sprite.setColor(eye_color.to24Bit());
                sprite.fillCircle(eye_x * mag + x0, eye_y * mag + y0, eye_r * mag);
                sprite.fillCircle(-eye_x * mag + x0, eye_y * mag + y0, eye_r * mag);
                sprite.setColor(pupil_color.to24Bit());
                sprite.fillCircle(eye_x * mag + x0, eye_y * mag + y0, pupil_r * mag);
                sprite.fillCircle(-eye_x * mag + x0, eye_y * mag + y0, pupil_r * mag);
                sprite.fillTriangle(
                    x0, nose_y_top * mag + y0, 
                    nose_x * mag + x0, nose_y_bottom * mag + y0,
                    -nose_x * mag + x0, nose_y_bottom * mag + y0);
                // FIXME : リンクの姿勢が進行方向におうじて反転してしまっている（ベクトルで求めているので）
                // ダーティフィックスとして、進行方向に180°かけて足す。
                sprite.pushRotateZoom(&gfx, linksd[i].origin.x, linksd[i].origin.y, rad2deg(linksd[i].angle() + (direction + 1)/2*PI), linksd[i].width / sprite_size, linksd[i].height / sprite_size, TFT_BLACK);
            } else { // body
                sprite.setColor(body_colors[i].to24Bit());
                sprite.fillCircle(sprite_size / 2, sprite_size / 2, sprite_size / 2);
                sprite.pushRotateZoom(&gfx, linksd[i].origin.x, linksd[i].origin.y, rad2deg(linksd[i].angle() + (direction + 1)/2*PI), linksd[i].width / sprite_size, linksd[i].height / sprite_size, TFT_BLACK);
            }
        }
    }
};

/** 
 * 挙動を記述する層
 * 
 * - 画面外に出たときの挙動
 *      - 止まる
 *      - 折り返す
 *      - 反対から出てくる
 * - 状態の記述
 *      - 徐々に太って遅くなって（虹色になって退出する）
 */
struct WormBehavioral : public WormDrawable {
    enum WormStatus {
        NONE, RUNNING, FINISHED
    } status;
    enum OutBehavior {
        FINISH, // FINISHEDに移行する
        REVERSE, // 折り返す
        LOOP // 反対からまた出てくる
    } out_behavior;
    float reverse_margin; // 折返し時にあおむしがセットされるのが画面の外に画面幅の何倍行ったところか？
    bool randomize_out_behavior;
    float warn_life; // 警告が始まる寿命[s]
    float finish_life; // 終了に向かい始める寿命[s]
    float life; // 残り寿命[s]
    float life_duration; // 寿命の長さ（初期値）[s]
    WormBehavioral(WormGeometry const &worm, View const &view_)
        : WormDrawable(worm, view_)
        , status(NONE)
        , out_behavior(REVERSE)
        , reverse_margin(0.1)
        , randomize_out_behavior(false)
        , warn_life(30)
        , finish_life(10)
        {}
    void start(float life_)
    {
        life = life_;
        life_duration = life_;
        status = RUNNING;
    }
    void finish()
    {
        status = FINISHED;
    }
    void outCheck()
    {
        bool out_right = direction > 0 && view.isOutD(linksd[0]).right;
        bool out_left  = direction < 0 && view.isOutD(linksd[0]).left;
        if (!out_right && !out_left) return;
        if (out_behavior == FINISH) {
          status = FINISHED;
          out_behavior = REVERSE; // 次回のためにout_behaviorを戻しておく
          return;
        }
        if (randomize_out_behavior && (out_right || out_left)) {
            if (random(2) == 0) {
                out_behavior = REVERSE;
            } else {
                out_behavior = LOOP;
            }
        }
        float xg1 = view.toLocal(Vec2(view.width, 0)).x;
        float xg0 = view.toLocal(Vec2(0, 0)).x;
        float margin = (xg1 - xg0) * reverse_margin;
        Serial.printf("out_right = %d, out_left = %d, out_behavior = %d\n", out_right, out_left, out_behavior);
        if (out_right && out_behavior == REVERSE || out_left && out_behavior == LOOP) {
            setPosition(xg1 + margin, -1);
        } else {
            setPosition(xg0 - margin, 1);
        }
    }
    void draw(LovyanGFX &gfx, float dt)
    {
        if (life < 0) {
            status = FINISHED;
        } else {
            life -= dt;
        }
        if (status == NONE || status == FINISHED) return;
        if (life < finish_life) { // 終了に向かう
            randomize_out_behavior = false;
            out_behavior = FINISH;
        } else if (life < warn_life) { // 警告状態
            body_color_mode = RAINBOW;
            rainbow_speed = fmap(life, warn_life, 0, .2, 1);
        } else { // 通常状態
            // TODO : 線形だけどもうちょっと工夫する
            length += 1. / (life_duration - warn_life) * dt;
            thick += .6 / (life_duration - warn_life) * dt;
            head_magnify -= .4 / (life_duration - warn_life) * dt;
            width_magnify += 2 / (life_duration - warn_life) * dt;
            speed -= 0.6 / (life_duration - warn_life) * dt;
        }
        WormDrawable::draw(gfx, dt);
        outCheck();
    }
};
