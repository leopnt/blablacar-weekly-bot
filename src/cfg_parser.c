#include "cfg_parser.h"

char *cfg_line_clear_var_name(char *line_buf)
{
    size_t cursor = 0;
    size_t len = strlen(line_buf);
    while (line_buf[cursor] != '=')
        cursor++;

    line_buf[strcspn(line_buf, "\n")] = '\0';
    memmove(line_buf, line_buf + cursor + 1, len - cursor + 2);

    return line_buf;
}