/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/



#ifndef __SOUND_H__
#define __SOUND_H__

#include "globalincs/pstypes.h"

// Used for keeping track which low-level sound library is being used
#define SOUND_LIB_DIRECTSOUND		0
#define SOUND_LIB_RSX				1

#define GAME_SND_USE_DS3D			(1<<1)
#define GAME_SND_VOICE				(1<<2)

// Priorities that can be passed to snd_play() functions to limit how many concurrent sounds of a 
// given type are played.
#define SND_PRIORITY_MUST_PLAY				0
#define SND_PRIORITY_SINGLE_INSTANCE		1
#define SND_PRIORITY_DOUBLE_INSTANCE		2
#define SND_PRIORITY_TRIPLE_INSTANCE		3

typedef struct game_snd
{
	int sig;						// number of sound in sounds.tbl (not used)
	char filename[MAX_FILENAME_LEN];
	float default_volume;		// range: 0.0 -> 1.0
	int min, max;				// min: distance at which sound will stop getting louder  max: distance at which sound is inaudible
	int preload;					// preload sound (ie read from disk before mission starts)
	int id;						// index into Sounds[], where sound data is stored
	int id_sig;					// signature of Sounds[] element
	int flags;

	game_snd()
		: sig (0),
		  default_volume(0),
		  preload(0),
		  id(0),
		  id_sig(0),
		  flags(0)
	{
		filename[ 0 ] = NULL;
		min = 0;
		max = 0;
	}
} game_snd;

typedef struct sound_env
{
	unsigned long id;
	float volume;
	float damping;
	float decay;
} sound_env;

extern int Sound_enabled;
extern float Master_sound_volume;		// 0 -> 1.0
extern float Master_voice_volume;		// 0 -> 1.0
extern int Snd_sram;					// System memory consumed by sound data	
extern int Snd_hram;					// Soundcard memory consumed by sound data
extern ushort UserSampleRate, UserSampleBits;

//int	snd_load( char *filename, int hardware=0, int three_d=0, int *sig=NULL );
int snd_load(game_snd* gs, int allow_hardware_load = 0);

int snd_unload(int sndnum);
void snd_unload_all();

// Plays a sound with volume between 0 and 1.0, where 0 is the
// inaudible and 1.0 is the loudest sound in the game.
// Pan goes from -1.0 all the way left to 0.0 in center to 1.0 all the way right.
int snd_play(game_snd* gs, float pan=0.0f, float vol_scale=1.0f, int priority = SND_PRIORITY_SINGLE_INSTANCE,
			 bool voice_message = false);

// Play a sound directly from index returned from snd_load().  Bypasses
// the sound management process of using game_snd.
int snd_play_raw(int soundnum, float pan, float vol_scale=1.0f, int priority = SND_PRIORITY_MUST_PLAY);

// Plays a sound with volume between 0 and 1.0, where 0 is the
// inaudible and 1.0 is the loudest sound in the game.  It scales
// the pan and volume relative to the current viewer's location.
int snd_play_3d(game_snd* gs, vec3d* source_pos, vec3d* listen_pos, float radius=0.0f, vec3d* vel = NULL, int looping =
				0, float vol_scale=1.0f, int priority = SND_PRIORITY_SINGLE_INSTANCE, vec3d* sound_fvec = NULL,
				float range_factor = 1.0f, int force = 0);

// update the given 3d sound with a new position
void snd_update_3d_pos(int soudnnum, game_snd* gs, vec3d* new_pos, float radius = 0.0f, float range_factor = 1.0f);

// Use these for looping sounds.
// Returns the handle of the sound. -1 if failed.
// If startloop or stoploop are not -1, then then are used.
int snd_play_looping(game_snd* gs, float pan=0.0f, int start_loop=-1, int stop_loop=-1, float vol_scale=1.0f,
					 int priority = SND_PRIORITY_MUST_PLAY, int force = 0);

void snd_stop(int snd_handle);

// Sets the volume of a sound that is already playing.
// The volume is between 0 and 1.0, where 0 is the
// inaudible and 1.0 is the loudest sound in the game.
void snd_set_volume(int snd_handle, float volume);

// Sets the panning location of a sound that is already playing.
// Pan goes from -1.0 all the way left to 0.0 in center to 1.0 all the way right.
void snd_set_pan(int snd_handle, float pan);

// Sets the pitch (frequency) of a sound that is already playing
// Valid values for pitch are between 100 and 100000
void snd_set_pitch(int snd_handle, int pitch);
int snd_get_pitch(int snd_handle);

// Stops all sounds from playing, even looping ones.
void snd_stop_all();

// determines if the sound handle is still palying
int snd_is_playing(int snd_handle);

// change the looping status of a sound that is playing
void snd_chg_loop_status(int snd_handle, int loop);

// return the time in ms for the duration of the sound
int snd_get_duration(int snd_id);

// get a 3D vol and pan for a particular sound
int snd_get_3d_vol_and_pan(game_snd* gs, vec3d* pos, float* vol, float* pan, float radius=0.0f, float range_factor=
						   1.0f);

int snd_init(int use_a3d, int use_eax, unsigned int sample_rate, unsigned short sample_bits);
void snd_close();

// Return 1 or 0 to show that sound system is inited ok
int snd_is_inited();

// Returns a pointer to the direct sound object
uint sound_get_ds();

void snd_update_listener(vec3d* pos, vec3d* vel, matrix* orient);

void snd_use_lib(int lib_id);

int snd_num_playing();

int snd_get_data(int handle, char* data);
int snd_size(int handle, int* size);
void snd_do_frame();

// repositioning of the sound buffer pointer
void snd_rewind(int snd_handle, game_snd* sg, float seconds);					// rewind N seconds from the current position
void snd_ffwd(int snd_handle, game_snd* sg, float seconds);						// fast forward N seconds from the current position
void snd_set_pos(int snd_handle, game_snd* sg, float val, int as_pct);		// set the position val as either a percentage (if as_pct) or as a # of seconds into the sound

void snd_get_format(int handle, int* bits_per_sample, int* frequency);
int snd_time_remaining(int handle);

int snd_get_samples_per_measure(char* filename, float num_measures);


// sound environment

enum
{
	SND_ENV_GENERIC,
	SND_ENV_PADDEDCELL,
	SND_ENV_ROOM,
	SND_ENV_BATHROOM,
	SND_ENV_LIVINGROOM,
	SND_ENV_STONEROOM,
	SND_ENV_AUDITORIUM,
	SND_ENV_CONCERTHALL,
	SND_ENV_CAVE,
	SND_ENV_ARENA,
	SND_ENV_HANGAR,
	SND_ENV_CARPETEDHALLWAY,
	SND_ENV_HALLWAY,
	SND_ENV_STONECORRIDOR,
	SND_ENV_ALLEY,
	SND_ENV_FOREST,
	SND_ENV_CITY,
	SND_ENV_MOUNTAINS,
	SND_ENV_QUARRY,
	SND_ENV_PLAIN,
	SND_ENV_PARKINGLOT,
	SND_ENV_SEWERPIPE,
	SND_ENV_UNDERWATER,
	SND_ENV_DRUGGED,
	SND_ENV_DIZZY,
	SND_ENV_PSYCHOTIC
};

int sound_env_set(sound_env* se);
int sound_env_get(sound_env* se);
int sound_env_disable();
int sound_env_supported();

#endif
