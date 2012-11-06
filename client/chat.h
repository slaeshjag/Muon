#ifndef CHAT_H
#define CHAT_H

struct UI_PANE_LIST panelist_chat;
UI_WIDGET *chat_listbox;
UI_WIDGET *chat_hbox;
UI_WIDGET *chat_entry;
UI_WIDGET *chat_button_send;

void chat_init();
void chat_button_send_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e);
int chat_is_visible(struct UI_PANE_LIST *panelist);
void chat_show(struct UI_PANE_LIST *panelist);
void chat_hide(struct UI_PANE_LIST *panelist);
void chat_toggle(struct UI_PANE_LIST *panelist);
void chat_recv(int player, char *buf, int len);
void chat_join(int player);
void chat_leave(int player);
void chat_defeated(int player);
void chat_countdown(int countdown);
void chat_clear();

#endif
