project('main', 'cpp',
  version : '1.0.0',
  license : 'MIT',
  default_options : ['cpp_std=c++20', 'optimization=3', 'warning_level=0'])

inc = [include_directories('include')]
thread_dep = dependency('threads')

subdir('include')
subdir('src')
subdir('test')
