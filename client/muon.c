#include <string.h>

#include "muon.h"
#include "../server/server_api/server.h"

#include "view.h"
#include "ui/ui.h"

UI_WIDGET *chatbuf;
UI_WIDGET *textentry;

char textbuf[256]={};
char *textp=textbuf;

void sendbutton_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(type!=UI_EVENT_TYPE_UI_WIDGET_ACTIVATE)
		return;	
	UI_PROPERTY_VALUE v;
	struct UI_ENTRY_PROPERTIES *p=textentry->properties;
	strncpy(textp, p->text, 256-(textp-textbuf));
	textp+=strlen(p->text);
	*textp='\n';
	textp++;
	v.p=textbuf;
	chatbuf->set_prop(chatbuf, UI_LABEL_PROP_TEXT, v);
}

int main() {
	if(darnitInit("Muon", "muon")==NULL) {
		fprintf(stderr, "lol\n");
		return 1;
	}
	platform=darnitPlatformGet();
	DARNIT_MOUSE mouse;
	
	serverInit("map.ldi", 1, 1337);
	view_init();
	ui_init();
	
	UI_PANE *chatpane=ui_pane_create(10, 10, 256, 256, NULL);
	struct UI_PANE_LIST panelist={chatpane, NULL};
	chatbuf=ui_widget_create_label(font_std, "");
	UI_WIDGET *vbox=ui_widget_create_vbox();
	UI_WIDGET *sendbutton=ui_widget_create_button_text("Send");
	textentry=ui_widget_create_entry(font_std);
	ui_pane_set_root_widget(chatpane, vbox);
	ui_vbox_add_child(vbox, ui_widget_create_label(font_std, "Muon Chat"), 0);
	ui_vbox_add_child(vbox, chatbuf, 1);
	ui_vbox_add_child(vbox, textentry, 0);
	ui_vbox_add_child(vbox, sendbutton, 0);
	sendbutton->event_handler->add(sendbutton, sendbutton_click , UI_EVENT_TYPE_UI);
	
	while(1) {
		serverLoop(darnitTimeLastFrameTook());
		
		mouse=darnitMouseGet();
		view_scroll(mouse);
		
		//printf("%i           \r", darnitFPSGet());
		ui_events(&panelist);
		
		darnitRenderBegin();
		view_draw();
		darnitRenderTint(0, 1, 0, 1);
		ui_pane_render(chatpane);
		darnitRenderTint(1, 1, 1, 1);
		darnitRenderEnd();
		darnitLoop();
	}
	
	return 0;
}