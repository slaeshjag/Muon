#ifndef GAMEOVER_H
#define GAMEOVER_H

struct UI_PANE_LIST panelist_gameover_sidebar;
struct UI_PANE_LIST panelist_gameover_stats;

UI_WIDGET *gameover_sidebar_button_menu;
UI_WIDGET *gameover_stats_label_time;
char gameover_stats_label_time_text[32];
UI_WIDGET *gameover_stats_hbox;
UI_WIDGET *gameover_stats_vbox_stat[6];

struct GAMEOVER_STATLABEL {
	UI_WIDGET *built;
	char built_text[8];
	UI_WIDGET *lost;
	char lost_text[8];
	UI_WIDGET *destroyed;
	char destroyed_text[8];
	UI_WIDGET *efficiency;
	char efficiency_text[8];
	UI_WIDGET *score;
	char score_text[8];
	UI_WIDGET *name;
} *gameover_statlabel;

void gameover_init();
void gameover_update_stats();
void gameover_sidebar_button_menu_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

#endif
