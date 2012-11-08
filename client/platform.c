#include <string.h>

#include "muon.h"
#include "platform.h"

struct CONFIG_PARSER parsers[]={
	{"width", platform_option_read_int, platform_option_write_int, &config.screen_w},
	{"height", platform_option_read_int, platform_option_write_int, &config.screen_h},
	{"fullscreen", platform_option_read_int, platform_option_write_int, &config.fullscreen},
	{"plasma", platform_option_read_int, platform_option_write_int, &config.plasma},
	{"name", platform_option_read_string, platform_option_write_string, &config.player_name},
};


void platform_option_read_int(void *opt, char *p) {
	*((int *)opt)=atoi(p);
}

void platform_option_read_string(void *opt, char *p) {
	strcpy((char *)opt, p);
}

void platform_option_write_int(void *opt, char *p) {
	sprintf(p, "%i", *((int *)opt));
}

void platform_option_write_string(void *opt, char *p) {
	strcpy(p, (char *)opt);
}

void platform_config_init_defaults() {
	if(platform.platform&DARNIT_PLATFORM_HANDHELD) {
		config.screen_w=800;
		config.screen_h=480;
		config.fullscreen=1;
	} else {
		config.screen_w=800;
		config.screen_h=600;
		config.fullscreen=0;
		DARNIT_INPUT_MAP keymap={
			KEY(w),
			KEY(s),
			KEY(a),
			KEY(d),
			KEY(TAB),
			KEY(LSHIFT),
			KEY(SPACE),
			KEY(DELETE),
			KEY(ESCAPE),
			KEY(RETURN),
			KEY(F1),
			KEY(F2),
		};
		darnitButtonMappingSet(keymap);
	}
	config.plasma=1;
	strcpy(config.player_name, "player");
}

void platform_config_read() {
	char buf[128];
	int len, i;
	DARNIT_FILE *f;
	if(!(f=darnitFileOpen("muon.cfg", "r"))) {
		platform_config_write();
		return;
	}
	while((len=darnitFileLineGet(buf, 128, f))) {
		if(buf[0]=='#')
			continue;
		for(i=0; i<sizeof(parsers)/sizeof(struct CONFIG_PARSER); i++) {
			int optlen=strlen(parsers[i].option);
			if(memcmp(buf, parsers[i].option, optlen)||buf[optlen]!='=')
				continue;
			buf[len]=0;
			parsers[i].parser_func(parsers[i].config_opt, &buf[optlen+1]);
			break;
		}
	}
	darnitFileClose(f);
}

void platform_config_write() {
	char buf[128];
	int i;
	DARNIT_FILE *f;
	if(!(f=darnitFileOpen("muon.cfg", "w")))
		return;
	darnitFileWrite("#Muon\n", 6, f);
	for(i=0; i<sizeof(parsers)/sizeof(struct CONFIG_PARSER); i++) {
		strcpy(buf, parsers[i].option);
		strcat(buf, "=");
		parsers[i].writer_func(parsers[i].config_opt, &buf[strlen(buf)]);
		strcat(buf, "\n");
		darnitFileWrite(buf, strlen(buf), f);
	}
	darnitFileClose(f);
}

void platform_init() {
	darnitInitPartial("muon");
	platform=darnitPlatformGet();
	platform_config_init_defaults();
	platform_config_read();
	darnitInitRest("Muon", config.screen_w, config.screen_h, config.fullscreen);
	platform=darnitPlatformGet();
}
