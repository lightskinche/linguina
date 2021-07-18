#define _CRT_SECURE_NO_WARNINGS
#define  STB_IMAGE_IMPLEMENTATION //for stbi_image to work
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//SDL which we will use for renderering, audio, image loading, and other stuff
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "SDL_image.h"
//lua which will be the engine's target langauge
#include "luaconf.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#define MAX_BUFFER_SIZE 512 //512 is an all-around good amount for most char arrays
//some macros 
#define ERR_FLE_MIS(filename) printf("ERROR: File '%s' was not found. Stopping.", filename) //used so engine errors are uniform
//important globals
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Event even;
extern TTF_Font* strd_font;
extern char entry_symbol[MAX_BUFFER_SIZE], entry_file[MAX_BUFFER_SIZE], scripts_path[MAX_BUFFER_SIZE], program_name[MAX_BUFFER_SIZE];
extern char input_buf[MAX_BUFFER_SIZE];
extern int resolution_x, resolution_y, main_ref;
//most important global, lua itself!
extern lua_State* L;
//important custom types
typedef enum e_types e_types;

enum e_types {
	T_UNKNOWN = -1, //this type is unknown and should be discarded
	T_NULL = 0, //different from T_UNKNOWN as we know this type is 'garabage'
	T_SURFACE = 1, //this is an image that is loaded in ram
	T_TEXTURE = 2, //this is an image that is loaded on VRAM
	T_AUDIO = 3, //this is an audio stream that is loaded in ram
	T_FONT = 4 //self-explainatory, this is a font
};
//important global functions
extern inline void AUX_Load_Libraries(void);
extern void AUX_Handle_GameLoop(void);
//lua functions
extern int LUAPROC_Load_Image(lua_State* L);
extern int LUAPROC_Create_Texture(lua_State* L);