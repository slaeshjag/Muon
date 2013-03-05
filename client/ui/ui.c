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

void ui_init(int padding) {
	ui_padding=padding;
	ui_selected_widget=NULL;
	ui_e_m_prev.x=ui_e_m_prev.y=ui_e_m_prev.buttons=ui_e_m_prev.wheel=0;
	ui_panelist_dialogue.next=NULL; ui_panelist_dialogue.pane=NULL;
}

void *ui_widget_destroy(UI_WIDGET *widget) {
	struct UI_EVENT_HANDLER_LIST *h, *next;
	if(widget->event_handler)
		for(h=widget->event_handler->handlers; h; h=next) {
			next=h->next;
			free(h);
		}
	free(widget->event_handler);
	free(widget->properties);
	free(widget);
	return NULL;
}
