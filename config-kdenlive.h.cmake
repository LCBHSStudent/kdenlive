#ifndef CONFIG_H
#define CONFIG_H

#define SMARTIP_EDITOR_VERSION "@SMARTIP_EDITOR_VERSION_STRING@"

#define MLT_PREFIX "@MLT_PREFIX@"
#define MLT_MIN_MAJOR_VERSION @MLT_MIN_MAJOR_VERSION@
#define MLT_MIN_MINOR_VERSION @MLT_MIN_MINOR_VERSION@
#define MLT_MIN_PATCH_VERSION @MLT_MIN_PATCH_VERSION@

#define FFMPEG_SUFFIX "@FFMPEG_SUFFIX@"

#define SMARTIP_PWD "@SMARTIP_PWD@"

#cmakedefine HAVE_MALLOC_H 1
#cmakedefine HAVE_PTHREAD_H 1

#endif
