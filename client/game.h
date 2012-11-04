#ifndef GAME_H
#define GAME_H

#define SIDEBAR_WIDTH 128
#define SCROLL_OFFSET 8
#define SCROLL_SPEED 4

struct UI_PANE_LIST panelist_game_sidebar;
UI_WIDGET *game_sidebar_button_build[4];
UI_WIDGET *game_sidebar_label_build[4];
UI_WIDGET *game_sidebar_progress_build;
UI_WIDGET *game_sidebar_progress_shield;
UI_WIDGET *game_sidebar_progress_health;

int building_place;

void game_view_init();
void game_sidebar_button_build_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_view_key_press(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_view_buttons(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_view_mouse_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_view_mouse_move(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_update_building_status();
void game_view_scroll_to(int x, int y);
void game_set_building_progress(int building, int progress);
void game_reset_building_progress();
void game_view_draw();
void game_draw_mouse(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

#endif
