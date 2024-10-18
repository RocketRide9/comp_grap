project('lab1', 'cpp',
  version : '0.1',
  default_options : ['warning_level=2', 'cpp_std=c++20'])

lab_src = 'main.cpp'
ogl_dep = dependency('opengl')
glfw_dep = dependency('glfw3')

subdir('./SparkGUI')

executable('lab1',
           cpp_args: '-Db_sanitize=address',
           sources: lab_src,
           dependencies: [ogl_dep, glfw_dep],
           link_with: spark)
