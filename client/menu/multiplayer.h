#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

struct UI_PANE_LIST panelist_multiplayer_host;
UI_WIDGET *multiplayer_host_vbox;
UI_WIDGET *multiplayer_host_listbox_maps;
UI_WIDGET *multiplayer_host_entry_port;
UI_WIDGET *multiplayer_host_slider_players;
UI_WIDGET *multiplayer_host_slider_gamespeed;
UI_WIDGET *multiplayer_host_button_host;

struct UI_PANE_LIST panelist_multiplayer_join;
UI_WIDGET *multiplayer_join_entry_host;
UI_WIDGET *multiplayer_join_entry_port;
UI_WIDGET *multiplayer_join_button;

struct UI_PANE_LIST panelist_multiplayer_connecting;
UI_WIDGET *multiplayer_connecting_button_cancel;

void menu_multiplayer_init();

void multiplayer_join_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void multiplayer_connecting_button_cancel_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

#endif
