sources = files(
  'box.cpp',
  'button.cpp',
  'label.cpp',
  'rect.cpp',
  'side_pane.cpp',
  'slider.cpp',
  'spark_core.cpp',
  'widget.cpp',
)

spark = library('spark',
                cpp_args: ['-DSPARK_COMPILATION', '-Db_sanitize=address'],
                sources: sources,
                dependencies: [ogl_dep, glfw_dep])
