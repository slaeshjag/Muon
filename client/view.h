#ifndef VIEW_H
#define VIEW_H

#include "ui/ui.h"

#define SIDEBAR_WIDTH 128

void view_init();
void view_move(DARNIT_MOUSE mouse);
void view_draw();
void view_scroll(DARNIT_MOUSE mouse);

DARNIT_FONT *font_std;

DARNIT_RECT *sidebar;
DARNIT_LINE *sidebar_lines;
DARNIT_TEXT_SURFACE *sidebar_text_title;
DARNIT_TEXT_SURFACE *sidebar_text_buildings;

struct UI_PANE_LIST panelist_input_name;
UI_WIDGET *input_name_entry;
UI_WIDGET *input_name_button;

struct UI_PANE_LIST panelist_connect_server;
UI_WIDGET *connect_server_entry_host;
UI_WIDGET *connect_server_entry_port;
UI_WIDGET *connect_server_button;

void input_name_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void connect_server_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

#endif
