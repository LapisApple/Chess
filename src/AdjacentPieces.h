//
// Created by timap on 28.03.2024.
//

#pragma once

#include "Types/Vec2.h"

/**
 * forEachAdjacentField(int8_t adjacent_pos);
*/
template<typename fn>
constexpr void forAllAdjacentPieces(Vec2 pos, fn forEachAdjacentField) {
    for (int8_t x = -1; x <= 1; ++x) {
        for (int8_t y = -1; y <= 1; ++y) {
            Vec2 offset = Vec2(x, y);
            Vec2 capturePos = pos + offset;

            if (capturePos.outsideBoard()) continue;
            forEachAdjacentField(capturePos.getPos());
        }
    }
}