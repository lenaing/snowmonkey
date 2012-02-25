#include "freedesktop.h"

void
comply_to_freedesktop_specs()
{
    comply_to_environment_variables();
}

void
comply_to_environment_variables()
{
    char *value;

    value = getenv("HOME");
    if (NULL == value) {
        /* Wait, WHAT? */
        fprintf(stderr, "No HOME variable defined? REALLY?");
        fprintf(stderr, "Defaulting to '/'... meh.");
        putenv("HOME=/");
    }

    value = getenv("XDG_DATA_HOME");
    if (NULL == value) {
        putenv("XDG_DATA_HOME=$HOME/.local/share");
    }

    value = getenv("XDG_DATA_CONFIG");
    if (NULL == value) {
        putenv("XDG_DATA_CONFIG=$HOME/.config");
    }

    value = getenv("XDG_DATA_DIRS");
    if (NULL == value) {
        putenv("XDG_DATA_DIRS=/usr/local/share/:/usr/share/");
    }

    value = getenv("XDG_CONFIG_DIRS");
    if (NULL == value) {
        putenv("XDG_CONFIG_DIRS=/etc/xdg/");
    }

    value = getenv("XDG_CACHE_HOME");
    if (NULL == value) {
        putenv("XDG_CACHE_HOME=$HOME/.cache");
    }

}
