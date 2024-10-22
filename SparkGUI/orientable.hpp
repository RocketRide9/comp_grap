#pragma once
#if !defined (SPARK_INSIDE) and !defined (SPARK_COMPILATION)
    #error "This header can't be included directly. Please include "SparkGUI/spark.hpp" instead."
#endif

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
