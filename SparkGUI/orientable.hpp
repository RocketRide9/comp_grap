#pragma once

namespace Spark {
    enum Orientation {
        VERTICAL,
        HORIZONTAL,
    };
    // интерфейс виджета, имеющего направление
    // например контейнер может распологать в себе виджеты
    // горизонтально или вертикально
    class Orientable {
        virtual void set_orientation(Orientation orientation) = 0;
        virtual Orientation get_orientation() = 0;
    };
}
