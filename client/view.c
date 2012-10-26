#include "view.h"

void view_init() {
	map=darnitMapLoad("maps/map.ldmz");
	map_w=map->layer->tilemap->w*map->layer->tile_w;
	map_h=map->layer->tilemap->h*map->layer->tile_h;
	
	font_std=darnitFontLoad("../res/FreeMonoBold.ttf", 12, 512, 512);
	
	sidebar=darnitRenderRectAlloc(1);
	sidebar_lines=darnitRenderLineAlloc(5, 2);
	sidebar_text_title=darnitTextSurfaceAlloc(font_std, 16, SIDEBAR_WIDTH-32, platform.screen_w-SIDEBAR_WIDTH+16, 8);
	sidebar_text_buildings=darnitTextSurfaceAlloc(font_std, 16, SIDEBAR_WIDTH-32, platform.screen_w-SIDEBAR_WIDTH+16, 64-12-4);
	darnitRenderRectSet(sidebar, 0, platform.screen_w-SIDEBAR_WIDTH, platform.screen_h, platform.screen_w, 0);
	darnitRenderLineMove(sidebar_lines, 0, platform.screen_w-SIDEBAR_WIDTH+2, 0, platform.screen_w-SIDEBAR_WIDTH+2, platform.screen_h);
	
	darnitRenderLineMove(sidebar_lines, 1, platform.screen_w-SIDEBAR_WIDTH+8, 64, platform.screen_w-8, 64);
	darnitRenderLineMove(sidebar_lines, 2, platform.screen_w-SIDEBAR_WIDTH+8, platform.screen_h-32, platform.screen_w-8, platform.screen_h-32);
	darnitRenderLineMove(sidebar_lines, 3, platform.screen_w-SIDEBAR_WIDTH+8, 64, platform.screen_w-SIDEBAR_WIDTH+8, platform.screen_h-32);
	darnitRenderLineMove(sidebar_lines, 4, platform.screen_w-8, 64, platform.screen_w-8, platform.screen_h-32);
	
	darnitTextSurfaceStringAppend(sidebar_text_title, "fancy sidebar");
	darnitTextSurfaceStringAppend(sidebar_text_buildings, "buildings");
	
}

void view_scroll(DARNIT_MOUSE mouse) {
		register int scroll_x=0, scroll_y=0;
		
		if(mouse.x<SCROLL_OFFSET&&map->cam_x>0)
			scroll_x=-SCROLL_SPEED;
		else if(mouse.x>platform.screen_w-SIDEBAR_WIDTH-SCROLL_OFFSET&&mouse.x<platform.screen_w-SIDEBAR_WIDTH&&map->cam_x<map_w-platform.screen_w+SIDEBAR_WIDTH)
			scroll_x=SCROLL_SPEED;
		if(mouse.y<SCROLL_OFFSET&&map->cam_y>0)
			scroll_y=-SCROLL_SPEED;
		else if(mouse.y>platform.screen_h-SCROLL_OFFSET&&map->cam_y<map_h-platform.screen_h)
			scroll_y=SCROLL_SPEED;
		darnitMapCameraMove(map, map->cam_x+scroll_x, map->cam_y+scroll_y);
}

void view_draw() {
	darnitRenderTilemap(map->layer->tilemap);
	darnitRenderTint(0, 0, 0, 1);
	darnitRenderRectDraw(sidebar, 1);
	darnitRenderTint(1, 0, 0, 1);
	darnitRenderLineDraw(sidebar_lines, 5);
	darnitRenderBlendingEnable();
	darnitTextSurfaceDraw(sidebar_text_title);
	darnitTextSurfaceDraw(sidebar_text_buildings);
	darnitRenderBlendingDisable();
	darnitRenderTint(1, 1, 1, 1);
}