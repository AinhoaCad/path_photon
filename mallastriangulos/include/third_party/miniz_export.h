#pragma once

#ifndef MINIZ_EXPORT
#  if defined(_WIN32)
#    if defined(MINIZ_EXPORTS)
#      define MINIZ_EXPORT __declspec(dllexport)
#    elif defined(MINIZ_IMPORTS)
#      define MINIZ_EXPORT __declspec(dllimport)
#    else
#      define MINIZ_EXPORT
#    endif
#  elif defined(__GNUC__) && __GNUC__ >= 4
#    define MINIZ_EXPORT __attribute__((visibility("default")))
#  else
#    define MINIZ_EXPORT
#  endif
#endif

#ifndef MINIZ_DEPRECATED
#  if defined(_WIN32)
#    define MINIZ_DEPRECATED __declspec(deprecated)
#  elif defined(__GNUC__)
#    define MINIZ_DEPRECATED __attribute__((deprecated))
#  else
#    define MINIZ_DEPRECATED
#  endif
#endif
