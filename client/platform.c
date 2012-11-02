#include "muon.h"
#include "platform.h"

void platform_init() {
	darnitInitPartial("muon");
	platform=darnitPlatformGet();
	if(platform.platform&DARNIT_PLATFORM_HANDHELD) {
		darnitInitRest("Muon", 800, 480, 1);
	} else {
		darnitInitRest("Muon", 800, 600, 0);
		DARNIT_INPUT_MAP keymap={
			KEY(UP),
			KEY(DOWN),
			KEY(LEFT),
			KEY(RIGHT),
			KEY(TAB),
			KEY(LALT),
			KEY(SPACE),
			KEY(BACKSPACE),
			KEY(ESCAPE),
			KEY(RETURN),
			KEY(F1),
			KEY(F2),
		};
		darnitButtonMappingSet(keymap);
	}
	platform=darnitPlatformGet();
}
