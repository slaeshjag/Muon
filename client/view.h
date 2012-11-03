#ifndef VIEW_H
#define VIEW_H

#include "ui/ui.h"

#define SIDEBAR_WIDTH 128

void view_init();
void view_draw(DARNIT_MOUSE *mouse);
void view_scroll(DARNIT_MOUSE *mouse, DARNIT_KEYS *buttons);

DARNIT_FONT *font_std;
DARNIT_TILESHEET *mouse_tilesheet;

DARNIT_LINE *map_border;

struct UI_PANE_LIST panelist_input_name;
UI_WIDGET *input_name_entry;
UI_WIDGET *input_name_button;

struct UI_PANE_LIST panelist_connect_server;
UI_WIDGET *connect_server_entry_host;
UI_WIDGET *connect_server_entry_port;
UI_WIDGET *connect_server_button;

struct UI_PANE_LIST panelist_connecting;
UI_WIDGET *connecting_button_cancel;

struct UI_PANE_LIST panelist_lobby, panelist_lobby_ready;
UI_WIDGET *lobby_countdown_label;
UI_WIDGET *lobby_progress_map;
char lobby_countdown_text[16];
UI_WIDGET *lobby_checkbox_ready;

struct UI_PANE_LIST panelist_game_sidebar;
UI_WIDGET *game_sidebar_button_build[4];
UI_WIDGET *game_sidebar_label_build[4];
UI_WIDGET *game_sidebar_progress_build;
UI_WIDGET *game_sidebar_progress_shield;
UI_WIDGET *game_sidebar_progress_health;

DARNIT_LINE *powergrid;
int powergrid_lines;
DARNIT_LINE *selected_border;
int selected_index;
int selected_building;

struct UI_PANE_LIST panelist_game_menu;
UI_WIDGET *game_menu_button[2];

void view_scroll_to(int x, int y);

void input_name_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void connect_server_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void connecting_button_cancel_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void ready_checkbox_toggle(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_sidebar_button_build_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_menu_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

#endif
