#ifndef LOBBY_H
#define LOBBY_H

struct UI_PANE_LIST panelist_lobby, panelist_lobby_ready;
UI_WIDGET *lobby_countdown_label;
UI_WIDGET *lobby_progress_map;
char lobby_countdown_text[16];
UI_WIDGET *lobby_checkbox_ready;

void lobby_init();
void lobby_ready_checkbox_toggle(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
void lobby_set_map_progress(int progress);
void lobby_ready_checkbox_enable();
void lobby_ready_checkbox_disable();

#endif
