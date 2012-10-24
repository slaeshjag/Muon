#include "muon.h"

int main() {
	if(darnitInit("Muon", "muon")==NULL) {
		fprintf(stderr, "lol\n");
		return 1;
	}
	platform=darnitPlatformGet();
	DARNIT_MAP *map=darnitMapLoad("maps/testmap.ldmz");
	DARNIT_MOUSE mouse;
	
	int map_w=map->layer->tilemap->w*map->layer->tile_w;
	int map_h=map->layer->tilemap->h*map->layer->tile_h;
	
	int scroll_x=0, scroll_y=0;
	
	while(1) {
		mouse=darnitMouseGet();
		if(mouse.x>platform.screen_w-SCROLL_OFFSET&&map->cam_x<map->layer->tilemap->w*map->layer->tile_w-platform.screen_w)
			darnitMapCameraMove(map, map->cam_x+SCROLL_SPEED, map->cam_y);
		if(mouse.x<SCROLL_OFFSET&&map->cam_x>0)
			darnitMapCameraMove(map, map->cam_x-SCROLL_SPEED, map->cam_y);
		if(mouse.y>platform.screen_h-SCROLL_OFFSET&&map->cam_y<map->layer->tilemap->h*map->layer->tile_h-platform.screen_h)
			darnitMapCameraMove(map, map->cam_x, map->cam_y+SCROLL_SPEED);
		if(mouse.y<SCROLL_OFFSET&&map->cam_y>0)
			darnitMapCameraMove(map, map->cam_x, map->cam_y-SCROLL_SPEED);
		
		//scroll_x=
		
		//darnitMapCameraMove(map, map->cam_x+scroll_x, map->cam_y+scroll_y);
		
		printf("%i           \r", darnitFPSGet());
		darnitRenderBegin();
		darnitRenderTilemap(map->layer->tilemap);
		darnitRenderEnd();
		darnitLoop();
	}
	
	return 0;
}