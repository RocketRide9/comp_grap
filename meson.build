project('lab1', 'cpp',
  version : '0.1',
  default_options : ['warning_level=2', 'cpp_std=c++20'])

lab_src = 'main.cpp'
frust_src = 'test_frust.cpp'
ogl_dep = dependency('opengl')
glu_dep = dependency('glu')
glfw_dep = dependency('glfw3')

subdir('./SparkGUI')

executable('lab1',
           cpp_args: '-Db_sanitize=address',
           sources: lab_src,
           dependencies: [ogl_dep, glfw_dep],
           link_with: spark
           )

executable('test_frust',
          cpp_args: '-Db_sanitize=address',
          sources: frust_src,
          dependencies: [ogl_dep, glfw_dep, glu_dep],
          link_with: spark
          )
