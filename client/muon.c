#include "muon.h"
#include "view.h"
#include "ui.h"

int main() {
	if(darnitInit("Muon", "muon")==NULL) {
		fprintf(stderr, "lol\n");
		return 1;
	}
	platform=darnitPlatformGet();
	DARNIT_MOUSE mouse;
	
	view_init();
	
	UI_PANE *msgbox=ui_pane_create(10, 10, 256, 256, NULL);
	UI_WIDGET *msglabel=ui_widget_create_label(font_std, "din mamma.\npå pizza");
	ui_pane_set_root_widget(msgbox, msglabel);
	
	while(1) {
		mouse=darnitMouseGet();
		view_scroll(mouse);
		
		printf("%i           \r", darnitFPSGet());
		darnitRenderBegin();
		view_draw();
		ui_pane_render(msgbox);
		darnitRenderEnd();
		darnitLoop();
	}
	
	return 0;
}