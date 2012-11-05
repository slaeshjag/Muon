#ifndef GAME_H
#define GAME_H

#define SIDEBAR_WIDTH 128
#define SCROLL_OFFSET 8
#define SCROLL_SPEED 4

struct UI_PANE_LIST panelist_game_sidebar;
UI_WIDGET *game_sidebar_minimap;
UI_WIDGET *game_sidebar_button_build[5];
UI_WIDGET *game_sidebar_label_build[5];
UI_WIDGET *game_sidebar_progress_build;
UI_WIDGET *game_sidebar_progress_shield;
UI_WIDGET *game_sidebar_progress_health;

int building_place;

struct GAME_ATTACKLIST {
	int index;
	int target;
	struct GAME_ATTACKLIST *next;
} *game_attacklist;
int game_attacklist_length;
DARNIT_LINE *game_attacklist_lines;
unsigned int game_attacklist_blink_semaphore;

void game_view_init();
void game_sidebar_minimap_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_sidebar_button_build_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_view_key_press(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_view_buttons(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_view_mouse_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_view_mouse_move(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void game_update_building_status();
void game_view_scroll_to(int x, int y);
void game_set_building_progress(int building, int progress);
void game_reset_building_progress();

void game_attacklist_lines_recalculate();
void game_attacklist_add(int index);
void game_attacklist_remove(int index);
void game_attacklist_clear();
void game_attacklist_untarget(int target);
void game_attacklist_target(int index, int target);

void game_view_draw();
void game_draw_mouse(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);

#endif
