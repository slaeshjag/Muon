#include <string.h>

#include "muon.h"
#include "client.h"
#include "view.h"
#include "chat.h"

void chat_init() {
	panelist_chat.pane=ui_pane_create(16, platform.screen_h-256-16, 200, 256, NULL);
	panelist_chat.next=NULL;
	ui_pane_set_root_widget(panelist_chat.pane, ui_widget_create_vbox());
	ui_vbox_add_child(panelist_chat.pane->root_widget, ui_widget_create_label(font_std, "Chat"), 0);
	chat_listbox=ui_widget_create_listbox(font_std);
	ui_vbox_add_child(panelist_chat.pane->root_widget, chat_listbox, 1);
	chat_hbox=ui_widget_create_hbox();
	chat_button_send=ui_widget_create_button_text("Send");
	chat_entry=ui_widget_create_entry(font_std);
	ui_hbox_add_child(chat_hbox, chat_entry, 1);
	ui_hbox_add_child(chat_hbox, chat_button_send, 0);
	ui_vbox_add_child(panelist_chat.pane->root_widget, chat_hbox, 0);
	chat_button_send->event_handler->add(chat_button_send, chat_button_send_click, UI_EVENT_TYPE_UI);
	chat_entry->event_handler->add(chat_entry, chat_button_send_click, UI_EVENT_TYPE_KEYBOARD);
}

int chat_is_visible(struct UI_PANE_LIST *panelist) {
	struct UI_PANE_LIST *p;
	for(p=panelist; p; p=p->next)
		if(p==&panelist_chat)
			return 1;
	return 0;
}

void chat_show(struct UI_PANE_LIST *panelist) {
	if(!panelist)
		return;
	struct UI_PANE_LIST *p;
	for(p=panelist; p->next; p=p->next) {
		if(p->next==&panelist_chat||p==&panelist_chat) {
			ui_selected_widget=chat_entry;
			return;
		}
	}
	p->next=&panelist_chat;
	ui_selected_widget=chat_entry;
}

void chat_hide(struct UI_PANE_LIST *panelist) {
	if(!panelist)
		return;
	struct UI_PANE_LIST *p;
	for(p=panelist; p->next; p=p->next) {
		if(p->next==&panelist_chat) {
			//p->next=panelist_chat.next;
			p->next=NULL;
			ui_selected_widget=NULL;
			return;
		}
	}
}

void chat_toggle(struct UI_PANE_LIST *panelist) {
	struct UI_PANE_LIST *p;
	for(p=panelist; p; p=p->next) {
		if(p->next==&panelist_chat) {
			p->next=panelist_chat.next;
			ui_selected_widget=NULL;
			return;
		}
		if(p->next==NULL) {
			p->next=&panelist_chat;
			ui_selected_widget=chat_entry;
			return;
		}
	}
}

void chat_button_send_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(type!=UI_EVENT_TYPE_UI_WIDGET_ACTIVATE&&(type!=UI_EVENT_TYPE_KEYBOARD&&e->keyboard->keysym!=KEY(RETURN)))
		return;
	UI_PROPERTY_VALUE v;
	v=chat_entry->get_prop(chat_entry, UI_ENTRY_PROP_TEXT);
	if(strlen(v.p)==0)
		return;
	client_message_send(player_id, MSG_SEND_CHAT, 0, strlen(v.p), v.p);
	v.p="";
	chat_entry->set_prop(chat_entry, UI_ENTRY_PROP_TEXT, v);
}

void chat_recv(int player, char *buf, int len) {
	//buf must be at least one byte larger than len!
	char *chatmsg=(char *)malloc(len+36);
	buf[len]=0;
	sprintf(chatmsg, "<%s> %s\n", &player_names[player*32], buf);
	ui_listbox_add(chat_listbox, chatmsg);
	ui_listbox_scroll(chat_listbox, -1);
	free(chatmsg);
}

void chat_join(int player) {
	char *chatmsg=malloc(64);
	sprintf(chatmsg, " * %s joined", &player_names[player*32]);
	ui_listbox_add(chat_listbox, chatmsg);
	ui_listbox_scroll(chat_listbox, -1);
	free(chatmsg);
}

void chat_disconnect(int player) {
	char *chatmsg=malloc(64);
	sprintf(chatmsg, " * %s disconnected", &player_names[player*32]);
	ui_listbox_add(chat_listbox, chatmsg);
	ui_listbox_scroll(chat_listbox, -1);
	free(chatmsg);
}

void chat_countdown(int countdown) {
	char *chatmsg=malloc(32);
	sprintf(chatmsg, " * Game starts in %i", countdown);
	ui_listbox_add(chat_listbox, chatmsg);
	ui_listbox_scroll(chat_listbox, -1);
	free(chatmsg);
}
