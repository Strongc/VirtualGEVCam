#include "color_print.h"

static int cp_apply(cp_state_ptr cp)
{
#ifdef WINDOWS
    return SetConsoleTextAttribute(cp->std_output, cp->current_color) == TRUE ? 0:(-1);
#else
    if (cp->current_color == CP_DEF)
        return (fprintf(cp->std_output, "\e[0m") > 0) ? 0:(-1);

    int cp_fg = ((cp->current_color & 0x0f) < 0x0f) ? (cp->current_color & 0x0f)+29 : 0;
    int cp_bg = (((cp->current_color >> 4) & 0x0f) < 0x0f) ? ((cp->current_color >> 4) & 0x0f)+39 : 0;
    if (cp_fg > 0)
        if (fprintf(cp->std_output, "\e[%dm\e[%dm", cp_fg, cp_bg) < 0)
            return (-1);
    if (cp_bg > 0)
        if (fprintf(cp->std_output, "\e[%dm", cp_fg) < 0)
            return (-1);
#endif
}

cp_state_ptr cp_init()
{
    cp_state_ptr cp;
#ifdef WINDOWS
    HANDLE                     std_output;
    CONSOLE_SCREEN_BUFFER_INFO screen_buff;

    cp = (cp_state_ptr)malloc(sizeof(struct cp_state));
    assert(cp);

    std_output = GetStdHandle(STD_OUTPUT_HANDLE);

    if (std_output == INVALID_HANDLE_VALUE)
        return 0;

    if(!GetConsoleScreenBufferInfo(std_output, &screen_buff))
        return 0;

    cp->std_output = std_output;
    cp->default_color = cp->current_color = screen_buff.wAttributes;
#else
    cp = (cp_state_ptr)malloc(sizeof(struct cp_state));
    assert(cp);

    cp->std_output = stdout;
    cp->default_color = cp->current_color = CP_DEF;
#endif

    return cp;
}

int cp_print(cp_state_ptr cp, enum cp_color color, const char * text)
{
    int ret;
    assert(cp);

    cp->current_color = color;
#ifdef WINDOWS
    if (color == CP_DEF)
        cp->current_color = cp->default_color;
#endif
    cp_apply(cp);

    ret = printf("%s", text);

    cp_reset(cp);

    return ret;
}

void cp_reset(cp_state_ptr cp)
{
    assert(cp);

    cp->current_color = cp->default_color ;
    cp_apply(cp);
}

void cp_close(cp_state_ptr cp)
{
    assert(cp);

    cp_reset(cp);

    if (cp)
    {
        free(cp);
        cp = NULL;
    }
}
