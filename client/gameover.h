#ifndef GAMEOVER_H
#define GAMEOVER_H

struct UI_PANE_LIST panelist_gameover_sidebar;
struct UI_PANE_LIST panelist_gameover_stats;

UI_WIDGET *gameover_sidebar_button_menu;

void gameover_init();
void gameover_sidebar_button_menu_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

#endif
