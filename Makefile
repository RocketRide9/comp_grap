BUILDDIR = build
ODIR = objs/spark
SDIR = SparkGUI

_OBJS = slider.o box.o button.o label.o rect.o \
	side_pane.o spark_core.o widget.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

default: main.out

obj_dir:
	mkdir -p objs/spark

$(ODIR)/%.o: $(SDIR)/%.cpp $(SDIR)/%.hpp
	g++ -c -g $< -o $@

objs/main.o: main.cpp
	g++ -c -g main.cpp -o objs/main.o

main.out: $(OBJS) objs/main.o
	g++ $^ -fsanitize=address -Wall -g -lglfw -lGL -o main.out

clean:
	gio trash $(OBJS) main.out objs/main.o
