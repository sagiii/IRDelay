#include <iostream>
#include <stdio.h>
#include "worm.hpp"
#include "vec2.hpp"

using namespace std;

std::vector<WormGeometry> worms;

void createWorms()
{
    for (int i = 0; i < 10; i++) {
        WormGeometry worm(1, 0.2, 1.2, 2, 10 + i);
        worms.push_back(worm);
    }
}

WormGeometry &first()
{
    return worms[0];
}

int main()
{
    createWorms();
    for (auto i = worms.begin(); i != worms.end(); i++) {
        cout << i->division << endl;
        printf("addr = %lu\n", (unsigned long) &(*i));
    }
    WormGeometry &w = first();
    printf("w addr = %lu\n", (unsigned long) &w);
    auto i = std::find_if(worms.begin(), worms.end(), [](WormGeometry& w){return w.division == 11;});
    if (i != worms.end()) {
        printf("found addr = %lu\n", (unsigned long) &(*i));
    }
    return 0;
}
