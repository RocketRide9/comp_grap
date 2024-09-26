#pragma once

namespace Spark {
    enum Orientation {
        VERTICAL,
        HORIZONTAL,
    };
    class Orientable {
        virtual void set_orientation(Orientation orientation) = 0;
        virtual Orientation get_orientation() = 0;
    };
}
