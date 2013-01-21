/* Muon - a new kind of rts
 * Copyright 2012 Steven Arnow <s@rdw.se> and Axel Isaksson (mr3d/h4xxel)
 * 
 * This file is part of Muon.
 * 
 * Muon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Muon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Muon.  If not, see <http://www.gnu.org/licenses/>.
 */
 
 #include "ui.h"
 #include "../platform.h"

void ui_messagebox(DARNIT_FONT *font, const char *text) {
	static UI_PANE *pane=NULL;
	static UI_WIDGET *label=NULL, *button=NULL, *vbox=NULL;
	if(label)
		label->destroy(label);
	if(button)
		button->destroy(button);
	if(vbox)
		vbox->destroy(vbox);
	if(pane)
		ui_pane_destroy(pane);
	label=ui_widget_create_label(font, text);
	button=ui_widget_create_button_text(font, T("OK"));
	button->event_handler->add(button, ui_messagebox_button_click, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	vbox=ui_widget_create_vbox();
	ui_panelist_dialogue.pane=pane=ui_pane_create(platform.screen_w/2-128, platform.screen_h/2-64, 256, 128, vbox);
	ui_vbox_add_child(vbox, label, 1);
	ui_vbox_add_child(vbox, button, 0);
}

void ui_messagebox_button_click(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	ui_panelist_dialogue.pane=NULL;
}
