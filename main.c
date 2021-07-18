#include "mainheader.h"
//define some externs
char entry_symbol[MAX_BUFFER_SIZE], entry_file[MAX_BUFFER_SIZE], scripts_path[MAX_BUFFER_SIZE], program_name[MAX_BUFFER_SIZE];
char input_buf[MAX_BUFFER_SIZE];
int resolution_x = 0, resolution_y = 0, main_ref = 0; lua_State* L = NULL;
SDL_Window* window = NULL; SDL_Renderer* renderer = NULL; 
SDL_Event even; TTF_Font* strd_font = NULL;
int main(void) {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	//open engine.config
	{
		FILE* fp = fopen("engine.config", "r");
		if (fp) {
			//get everything we need from the config
			fscanf(fp, "%s\n%s\n%s\n%s\n%d %d\n", entry_symbol, entry_file, scripts_path, program_name, &resolution_x, &resolution_y);
			fclose(fp);
		}
		else {
			ERR_FLE_MIS("engine.config");
			return 1;
		}
	}
	//now set up 
	window = SDL_CreateWindow(program_name, 0, 80, resolution_x, resolution_y, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	strd_font = TTF_OpenFont("resources/fonts/arial.ttf", 25);
	//now lets set up our lua context
	L = lua_open();
	AUX_Load_Libraries();
	if(luaL_loadfile(L, entry_file) || lua_pcall(L, 0, 0, 0)){
		printf("LUA ERROR: %s\n", lua_tostring(L,-1));
		return 1;
	}
	lua_getglobal(L, entry_symbol);
	main_ref = luaL_ref(L, LUA_REGISTRYINDEX);
	if (main_ref == -1) {
		printf("LUA ERROR: Could not find entry point '%s' in %s", entry_symbol, entry_file);
		return 1;
	}
	//make sure we clear the input buf, we don't want any junk
	memset(input_buf, 0, MAX_BUFFER_SIZE);
	while (1) {
		AUX_Handle_GameLoop();
	}
	return 0;
}