#include <iostream>
#include "worm.hpp"
#include "vec2.hpp"

using namespace std;

void printWorm(WormGeometry &worm) {
    for (auto i = worm.linksg.begin(); i != worm.linksg.end(); i++) {
        cout << i->origin.x << ", " << i->origin.y << endl;
    }
}

int main()
{
#if 0
    cout << "hoge" << endl;

    Vec2 v(10, 20), w(20, 30);
    cout << v + w << endl;
#endif

    WormGeometry worm(1, 0.2, 1, 15);
    worm.speed = 1;
    worm.ease0.poly = Easing::QUAD;
    worm.ease0.in = true;
    worm.ease0.out = true;
    worm.ease1.poly = Easing::QUAD;
    worm.ease1.in = true;
    worm.ease1.out = true;
    worm.init();
    for (int i = 0; i < 15; i++) {
        printWorm(worm);
        worm.tick(0.1);
        cout << endl << endl;
    }
    worm.setPosition(2, -1);
    for (int i = 0; i < 10; i++) {
        printWorm(worm);
        worm.tick(0.1);
        cout << endl << endl;
    }
    
    return 0;
}
