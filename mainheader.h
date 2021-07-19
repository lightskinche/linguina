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
//include the datastructures we will be using
#include "utlist.h"
#include "uthash.h"
//some macros
#define MAX_BUFFER_SIZE 512 //512 is an all-around good amount for most char arrays
#define MAX_INPUT_SIZE 64
//important globals
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Event even;
extern TTF_Font* strd_font;
extern FILE* logfile;
extern char entry_symbol[MAX_BUFFER_SIZE], textmain_symbol[MAX_BUFFER_SIZE], start_symbol[MAX_BUFFER_SIZE];
extern char entry_file[MAX_BUFFER_SIZE], scripts_path[MAX_BUFFER_SIZE], program_name[MAX_BUFFER_SIZE];
extern char input_buf[MAX_INPUT_SIZE];
extern int resolution_x, resolution_y;
extern int main_ref, textmain_ref, start_ref;
extern int text_background;
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
typedef struct s_renderered_text s_renderered_text;

struct s_renderered_text { //this is for rendered text, meaning text that has already been created as a texture and just needs to be renderered
	SDL_Texture* text;
	int w, h;
};
extern s_renderered_text g_text; 
extern s_renderered_text g_err_text; //special text that is meant for use with 'textmain' when it denies the user the ability to submit or press enter
extern SDL_Texture* background;
//important strucutres
typedef struct s_scene s_scene;
typedef struct s_location s_location;
typedef struct s_thing s_thing;

struct s_scene {
	char* name, *on_enter, *on_exit, *examine;
	s_location* locations;
	UT_hash_handle hh;
};
struct s_location {
	char* examine, *on_enter;  //locations don't have names, they have descriptions and things within them
	s_location* north, *south, *west, *east;
	int map;
	s_thing* things;
};
struct s_thing {
	char* name, *examine;
};
extern s_scene* unloaded_scenes, *scenes;
//important global functions
extern inline void AUX_Load_Libraries(void);
extern void AUX_Handle_GameLoop(void);
extern void flogf(char* format, ...); //special function that writes to stdout and to log.txt
//lua functions
extern int LUAPROC_Load_Texture(lua_State* L);
extern int LUAPROC_Destroy_Texture(lua_State* L);
extern int LUAPROC_Set_Background(lua_State* L);

extern int LUAPROC_Display(lua_State* L);
extern int LUAPROC_Log(lua_State* L);

extern int LUAPROC_Create_Scene(lua_State* L);
extern int LUAPROC_Load_Scene(lua_State* L);
extern int LUAPROC_Unload_Scene(lua_State* L);
extern int LUAPROC_Destroy_Scene(lua_State* L);
extern int LUAPROC_Find_Scene_Unloaded(lua_State* L);

extern int LUAPROC_Create_LocationMap(lua_State* L);
extern int LUAPROC_Create_Location(lua_State* L);
extern int LUAPROC_Destroy_Location(lua_State* L);
//metamethods
extern int METAPROC_Index_Scene(lua_State* L);
extern int METAPROC_NewIndex_Scene(lua_State* L);
extern int METAPROC_Tostring_Scene(lua_State* L);

extern int METAPROC_Index_LocationMap(lua_State* L);

extern int METAPROC_Index_Location(lua_State* L);

extern int METAPROC_NewIndex_LocationS(lua_State* L);
extern int METAPROC_Tostring_LocationS(lua_State* L);