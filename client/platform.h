#ifndef PLATFORM_H
#define PLATFORM_H

DARNIT_PLATFORM platform;

enum CONFIG_OPTIONS {
	CONFIG_OPTION_SCREEN_W,
	CONFIG_OPTION_SCREEN_H,
	CONFIG_OPTION_FULLSCREEN,
	CONFIG_OPTION_PLASMA,
	CONFIG_OPTION_PLAYER_NAME,
} options;

struct CONFIG {
	int screen_w;
	int screen_h;
	int fullscreen;
	int plasma;
	char player_name[32];
} config;

struct CONFIG_PARSER {
	const char *option;
	void (*parser_func)(void *opt, char *p);
	void (*writer_func)(void *opt, char *p);
	void *config_opt;
};

DARNIT_VIDEOMODE **videomodes;

void platform_init();

void platform_config_read();
void platform_config_write();

void platform_option_read_int(void *opt, char *p);
void platform_option_read_string(void *opt, char *p);
void platform_option_write_int(void *opt, char *p);
void platform_option_write_string(void *opt, char *p);

#endif
