#include "rect.hpp"

namespace Spark {
    std::ostream& operator<<(std::ostream& os, const Rect& rect) {
        return os << "{\n(x1, y1): (" << rect.x1 << "," << rect.y1 << ")\n"
            << "{\n(x2, y2): (" << rect.x2 << ", " << rect.y2 << ")\n}";
    }
}
