#include "mainheader.h"
//define some externs
char main_symbol[MAX_BUFFER_SIZE], textmain_symbol[MAX_BUFFER_SIZE], start_symbol[MAX_BUFFER_SIZE];
char entry_file[MAX_BUFFER_SIZE], scripts_path[MAX_BUFFER_SIZE], program_name[MAX_BUFFER_SIZE];
char input_buf[MAX_INPUT_SIZE];

int resolution_x = 0, resolution_y = 0; lua_State* L = NULL;
int main_ref = 0, textmain_ref = 0, start_ref = 0;
int text_background = 0;

SDL_Window* window = NULL; SDL_Renderer* renderer = NULL; 
SDL_Event even; TTF_Font* strd_font = NULL; Uint8* keyboard = NULL;

s_renderered_text g_text, g_err_text;
s_scene* unloaded_scenes = NULL, *scenes = NULL, *cur_scene = NULL;
FILE* logfile; SDL_Texture* background = NULL;

int main(void) {
	logfile = fopen("log.txt", "w"); //should close automatically on program termination; there is no point throwing fclose(logfile) everywhere
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	keyboard = SDL_GetKeyboardState(NULL); //do this early because why not
	//open engine.config
	{
		FILE* fp = fopen("engine.config", "r");
		if (fp) {
			//get everything we need from the config
			fscanf(fp, "%s\n%s\n%s\n%s\n%s\n%s\n%d %d\n", start_symbol, textmain_symbol, main_symbol, entry_file, scripts_path, program_name, &resolution_x, &resolution_y);
			fclose(fp);
		}
		else {
			flogf("ERROR: File 'engine.config' was not found. Stopping.");
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
		flogf("LUA ERROR: %s\n", lua_tostring(L,-1));
		return 1;
	}
	//load important symbols
	lua_getglobal(L, main_symbol);
	main_ref = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_getglobal(L, textmain_symbol);
	textmain_ref = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_getglobal(L, start_symbol);
	start_ref = luaL_ref(L, LUA_REGISTRYINDEX);
	if (main_ref == -1) {
		flogf("LUA ERROR: Could not find function '%s' in %s\n", main_symbol, entry_file);
		return 1;
	}
	if (textmain_ref == -1) {
		flogf("LUA ERROR: Could not find function '%s' in %s\n", textmain_symbol, entry_file);
		return 1;
	}
	if (start_ref == -1) {
		flogf("LUA ERROR: Could not find function '%s' in %s\n", start_symbol, entry_file);
	}
	//let's run the start function
	lua_rawgeti(L, LUA_REGISTRYINDEX, start_ref);
	lua_call(L, 0, 2);
	if (!lua_toboolean(L, -2)) {
		flogf("ERROR: %s\n", lua_tostring(L, -1));
		return 1;
	}
	lua_settop(L, 0);
	//make sure we clear the input buf, we don't want any junk
	memset(input_buf, 0, MAX_INPUT_SIZE);
	while (1) {
		AUX_Handle_GameLoop();
	}
	return 0;
}