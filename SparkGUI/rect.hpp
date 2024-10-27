#pragma once
#if !defined (SPARK_INSIDE) and !defined (SPARK_COMPILATION)
    // #error "This header can't be included directly. Please include "SparkGUI/spark.hpp" instead."
#endif

#include <cassert>
#include <ostream>

namespace Spark {
    class Coordinate {
        public:
        // Coordinate(int x, int y): x(x), y(y) {}
        int x, y;
    };
    class Rect {
        public:
        Rect(int x, int y, int width, int height) {
            assert(width >= 0);
            assert(height >= 0);
            x1 = x;
            y1 = y;
            x2 = x + width;
            y2 = y + height;
        }
        // Rect(const Rect &rect) {
        //     x1 = rect.x1;
        //     y1 = rect.y1;
        //     x2 = rect.x2;
        //     y2 = rect.y2;
        // }
        Rect() {
            x1 = 0;
            y1 = 0;
            x2 = 0;
            y2 = 0;
        }
        bool contains(int x, int y) const {
            return x1 <= x and x <= x2 and
                y1 <= y and y <= y2;
        }
        int get_width() const {
            return x2 - x1;
        }
        int get_height() const {
            return y2 - y1;
        }
        void place_at(int x, int y) {
            auto dx = x - x1;
            auto dy = y - y1;
            x1 = x;
            x2 += dx;
            y1 = y;
            y2 += dy;
        }
        void place_at(Coordinate coord) {
            place_at(coord.x, coord.y);
        }

        int x1 = 0, y1 = 0;
        int x2 = 0, y2 = 0;
        private:
    };
    std::ostream& operator<<(std::ostream& os, const Rect& rect);
}
