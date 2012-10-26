#include "muon.h"
#include "view.h"
#include "ui/ui.h"

int main() {
	if(darnitInit("Muon", "muon")==NULL) {
		fprintf(stderr, "lol\n");
		return 1;
	}
	platform=darnitPlatformGet();
	DARNIT_MOUSE mouse;
	
	view_init();
	
	UI_PANE *msgbox=ui_pane_create(10, 10, 256, 256, NULL);
	struct UI_PANE_LIST panelist={msgbox, NULL};
	UI_WIDGET *msglabel1=ui_widget_create_label(font_std, "din mamma.\npå pizza");
	UI_WIDGET *button1=ui_widget_create_button_text("testknapp");
	UI_WIDGET *msglabel2=ui_widget_create_label(font_std, "lol");
	UI_WIDGET *msglabel3=ui_widget_create_label(font_std, "hej");
	UI_WIDGET *vbox=ui_widget_create_vbox();
	ui_pane_set_root_widget(msgbox, vbox);
	ui_vbox_add_child(vbox, msglabel1, 0);
	ui_vbox_add_child(vbox, button1, 0);
	ui_vbox_add_child(vbox, msglabel2, 0);
	ui_vbox_add_child(vbox, msglabel3, 1);
	
	while(1) {
		mouse=darnitMouseGet();
		view_scroll(mouse);
		
		printf("%i           \r", darnitFPSGet());
		ui_events(&panelist);
		
		darnitRenderBegin();
		view_draw();
		darnitRenderTint(0, 1, 0, 1);
		ui_pane_render(msgbox);
		darnitRenderTint(1, 1, 1, 1);
		darnitRenderEnd();
		darnitLoop();
	}
	
	return 0;
}