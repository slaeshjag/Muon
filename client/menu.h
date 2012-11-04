#ifndef MENU_H
#define MENU_H

void menu_init();

//Main menu
struct UI_PANE_LIST panelist_input_name;
UI_WIDGET *input_name_entry;
UI_WIDGET *input_name_button;

struct UI_PANE_LIST panelist_connect_server;
UI_WIDGET *connect_server_entry_host;
UI_WIDGET *connect_server_entry_port;
UI_WIDGET *connect_server_button;

struct UI_PANE_LIST panelist_connecting;
UI_WIDGET *connecting_button_cancel;

void input_name_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void connect_server_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void connecting_button_cancel_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

//In-game menu
struct UI_PANE_LIST panelist_game_menu;
UI_WIDGET *game_menu_button[3];

void game_menu_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

#endif
