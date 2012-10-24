#include "muon.h"

int main() {
	if(darnitInit("Muon", "muon")==NULL) {
		fprintf(stderr, "lol\n");
		return 1;
	}
	platform=darnitPlatformGet();
	DARNIT_MOUSE mouse;
	
	view_init();
	
	while(1) {
		mouse=darnitMouseGet();
		view_scroll(mouse);
		
		printf("%i           \r", darnitFPSGet());
		darnitRenderBegin();
		view_draw();
		darnitRenderEnd();
		darnitLoop();
	}
	
	return 0;
}