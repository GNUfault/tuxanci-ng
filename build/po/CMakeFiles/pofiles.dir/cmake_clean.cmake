FILE(REMOVE_RECURSE
  "CMakeFiles/pofiles"
  "de_DE.gmo"
  "cs_CZ.gmo"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/pofiles.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
