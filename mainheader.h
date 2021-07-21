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
#define MAX_TOKENS 4
#define MAX_ETYPES 8
#define MAX_MIXER_CHANNELS 8
//important globals
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Event even;
extern TTF_Font* strd_font;
extern FILE* logfile;
extern Uint8* keyboard; //just in case we need quick keyboard input
extern char entry_symbol[MAX_BUFFER_SIZE], textmain_symbol[MAX_BUFFER_SIZE], start_symbol[MAX_BUFFER_SIZE];
extern char entry_file[MAX_BUFFER_SIZE], scripts_path[MAX_BUFFER_SIZE], program_name[MAX_BUFFER_SIZE];
extern char input_buf[MAX_INPUT_SIZE], *e_typenames[MAX_ETYPES];
extern int resolution_x, resolution_y;
extern int main_ref, textmain_ref, start_ref;
extern int text_background;
//most important global, lua itself!
extern lua_State* L;
//important custom types
typedef enum e_types e_types;
typedef struct s_data s_data;
typedef struct s_interaction s_interaction;
typedef int lua_callback;

enum e_types {
	T_UNKNOWN = -1, //this type will probably never be used but just in case
	T_NIL = 0, //different from T_UNKNOWN as we know this type is 'garabage'
	T_TEXTURE = 1, //this is an image that is loaded on VRAM
	T_AUDIO = 2, //this is an audio stream that is loaded in ram
	T_MUSIC = 3,
	T_SCENE = 4,
	T_LOCATION = 5,
	T_THING = 6
};
struct s_data {
	void* data;
	e_types type;
};
struct s_interaction {
	lua_callback call; //the rest of the tokens will be sent to this function
	char* keyword; //keyword that triggers this interaction
	s_interaction* next;
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
	lua_callback callback_enter, callback_exit, callback_examine;
	s_data* locations; //this s_data thing is annoying but we're doing it for safety
	UT_hash_handle hh;
};
//TO DO: make locations similar to how interactions work, they can be set from tables but cannot be read, but def think about the 'write-only' idea for the location since reading it might be important
//def make sure everything is consistant too
struct s_location {
	char* examine, *on_enter;  //locations don't have names, they have descriptions and things within them
	lua_callback callback_enter, callback_exit, callback_invalid; //invalid callback is when player tries to go out of bounds, this can be used to switch scenes or something
	s_location* north, *south, *west, *east;
	s_location* orginal_map;
	int w, h; //referring to the dimensions of the locationmap
	s_data* things; //linked list of s_things, but we have to make it s_data for type checking
};
struct s_thing {
	char* name, *examine;
	s_interaction* interactions; //linked list of interactions, the s_data thing really is triggering me but once again we are doing it for saftey
};
extern s_scene* scenes, *cur_scene;
//important global functions
extern inline void AUX_Load_Libraries(void);
extern void AUX_Handle_GameLoop(void);
extern void AUX_RenderTextProccesing(void);
extern void AUX_Display(char* buffer); //display something at the bottom of the screen
extern void flogf(char* format, ...); //special function that writes to stdout and to log.txt
//lua functions
extern int LUAPROC_Load_Texture(lua_State* L);

extern int LUAPROC_Load_Audio(lua_State* L);
extern int LUAPROC_Load_Music(lua_State* L);

extern int LUAPROC_Set_Background(lua_State* L);
extern int LUAPROC_Display(lua_State* L);
extern int LUAPROC_Log(lua_State* L);

extern int LUAPROC_Create_Scene(lua_State* L);

extern int LUAPROC_Create_LocationMap(lua_State* L);
extern int LUAPROC_Destroy_Location(lua_State* L);

extern int LUAPROC_Wait(lua_State* L);
extern int LUAPROC_Set_CurrentScene(lua_State* L);

extern int LUAPROC_Create_Thing(lua_State* L);
//metamethods
extern int METAPROC_Load_Scene(lua_State* L);
extern int METAPROC_Unload_Scene(lua_State* L);
extern int METAPROC_Gc_Scene(lua_State* L);
extern int METAPROC_Getter_Scene(lua_State* L);
extern int METAPROC_Setter_Scene(lua_State* L);

extern int METAPROC_Gc_Audio(lua_State* L);
extern int METAPROC_Gc_Music(lua_State* L);

extern int METAPROC_Play_Audio(lua_State* L);
extern int METAPROC_Stop_Audio(lua_State* L);
extern int METAPROC_Play_Music(lua_State* L);
extern int METAPROC_Stop_Music(lua_State* L);

extern int METAPROC_Gc_Texture(lua_State* L);

extern int METAPROC_Gc_Thing(lua_State* L);
extern int METAPROC_Getter_Thing(lua_State* L);
extern int METAPROC_Setter_Thing(lua_State* L);