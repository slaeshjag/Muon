#include <string.h>

#include "ui.h"

void ui_init() {
	ui_selected_widget=NULL;
	memset(&ui_e_m_prev, 0, sizeof(ui_e_m_prev));
}