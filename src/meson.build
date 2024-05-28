autodarkmode_sources = [
  'main.c',
  'sun.c',
  'extString.c',
  'configuration.c',
  'iniparser/src/dictionary.c',
  'iniparser/src/iniparser.c',
]

autodarkmode_deps = [
  dependency('glib-2.0'),
  dependency('libgeoclue-2.0'),
]

executable('autodarkmode', autodarkmode_sources,
  dependencies: autodarkmode_deps,
  install: true,
  link_args: '-lm',
)

