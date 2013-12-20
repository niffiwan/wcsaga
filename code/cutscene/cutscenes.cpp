/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell
 * or otherwise commercially exploit the source or things you created based on the
 * source.
 *
*/



#include "cutscene/cutscenes.h"
#include "ui/ui.h"
#include "gamesnd/gamesnd.h"
#include "gamesequence/gamesequence.h"
#include "freespace2/freespace.h"
#include "io/key.h"
#include "movie.h"
#include "popup/popup.h"
#include "menuui/mainhallmenu.h"
#include "globalincs/alphacolors.h"
#include "localization/localize.h"
#include "parse/parselo.h"


extern int Cmdline_nomovies;


char *Cutscene_bitmap_name[GR_NUM_RESOLUTIONS] = {
  "ViewFootage",
  "2_ViewFootage"
};

char *Cutscene_mask_name[GR_NUM_RESOLUTIONS] = {
  "ViewFootage-m",
  "2_ViewFootage-m"
};

size_t Num_cutscenes;
int Cutscenes_viewable;
int Description_index;
SCP_vector < cutscene_info > Cutscenes;

//extern int All_movies_enabled;        //  If set, all movies may be viewed.  Keyed off cheat code.
void cutscene_close()
{
  for (size_t i = 0; i < Num_cutscenes; i++)
    {
      if (Cutscenes[i].description)
        {
          vm_free(Cutscenes[i].description);
        }
    }
}

// initialization stuff for cutscenes
void cutscene_init()
{
  atexit(cutscene_close);
  char buf[MULTITEXT_LENGTH];
  int rval;
  cutscene_info cutinfo;

  // open localization
  lcl_ext_open();

  if ((rval = setjmp(parse_abort)) != 0)
    {
      mprintf(("TABLES: Unable to parse '%s'!  Error code = %i.\n",
               "cutscenes.tbl", rval));
      lcl_ext_close();
      return;
    }

  read_file_text("cutscenes.tbl", CF_TYPE_TABLES);
  reset_parse();

  // parse in all the cutscenes
  Num_cutscenes = 0;
  skip_to_string("#Cutscenes");
  ignore_white_space();

  while (required_string_either("#End", "$Filename:"))
    {
      memset(&cutinfo, 0, sizeof(cutscene_info));

      required_string("$Filename:");
      stuff_string(cutinfo.filename, F_PATHNAME, MAX_FILENAME_LEN);

      required_string("$Name:");
      stuff_string(cutinfo.name, F_NAME, NAME_LENGTH);

      required_string("$Description:");
      stuff_string(buf, F_MULTITEXT, sizeof(buf));
      drop_white_space(buf);
      compact_multitext_string(buf);
      cutinfo.description = vm_strdup(buf);

      required_string("$cd:");
      stuff_int(&cutinfo.cd);

      Cutscenes.push_back(cutinfo);
      Num_cutscenes++;
    }

  required_string("#End");

  Cutscenes_viewable = INTRO_CUTSCENE_FLAG;

  // close localization
  lcl_ext_close();
}

// function to return 0 based index of which CD a particular movie is on
// returns -1 on failure.
int cutscenes_get_cd_num(char *filename)
{
#if defined(OEM_BUILD)
  return 0;                     // only 1 cd for OEM
#else
  size_t i;

  for (i = 0; i < Num_cutscenes; i++)
    {
      if (!stricmp(Cutscenes[i].filename, filename))
        {
          return (Cutscenes[i].cd - 1);
        }
    }

  return -1;
#endif // defined(OEM_BUILD)
}

// marks a cutscene as viewable
void cutscene_mark_viewable(char *filename)
{
  size_t i;
  char cut_file[MAX_FILENAME_LEN];
  char file[MAX_FILENAME_LEN];

  Assert(filename != NULL);

  // strip off extension
  strcpy_s(file, filename);
  char *p = strchr(file, '.');
  if (p)
    {
      *p = 0;
    }

  // change to lower case
  strlwr(file);

  for (i = 0; i < Num_cutscenes; i++)
    {
      // change the cutscene file name to lower case
      strcpy_s(cut_file, Cutscenes[i].filename);
      strlwr(cut_file);

      // see if the stripped filename matches the cutscene filename
      if (strstr(cut_file, file) != NULL)
        {
          Cutscenes_viewable |= (1 << i);
          return;
        }
    }
}

#define NUM_BUTTONS             8

#define TECH_DATABASE_BUTTON    0
#define SIMULATOR_BUTTON        1
#define CUTSCENES_BUTTON        2
#define CREDITS_BUTTON          3

#define SCROLL_UP_BUTTON        4
#define SCROLL_DOWN_BUTTON      5
#define PLAY_BUTTON             6
#define EXIT_BUTTON             7

static int Num_files;
static SCP_vector < int >Cutscene_list;
//static int Stats_scroll_offset;  // not used - taylor
static int Selected_line = 0;   // line that is currently selected for binding
static int Scroll_offset;
static int Background_bitmap;
static UI_BUTTON List_region;
static UI_WINDOW Ui_window;

static ui_button_info Buttons[GR_NUM_RESOLUTIONS][NUM_BUTTONS] = {
  {
   // GR_640
   ui_button_info("TDB_00", 7, 3, 37, 7, 0),    // tech database 1
   ui_button_info("TDB_01", 7, 18, 37, 23, 1),  // tech database 2
   ui_button_info("TDB_02", 7, 34, 37, 38, 2),  // tech database 3
   ui_button_info("TDB_03", 7, 49, 37, 54, 3),  // tech database 4

   ui_button_info("VFB_04", 6, 318, -1, -1, 4), // scroll up
   ui_button_info("VFB_05", 36, 318, -1, -1, 5),        // scroll down
   ui_button_info("VFB_06", 578, 319, 587, 366, 6),     // play
   ui_button_info("VFB_07", 574, 431, 587, 413, 7),     // exit
   },
  {
   // GR_1024
   ui_button_info("2_TDB_00", 12, 5, 59, 12, 0),        // tech database 1
   ui_button_info("2_TDB_01", 12, 31, 59, 37, 1),       // tech database 2
   ui_button_info("2_TDB_02", 12, 56, 59, 62, 2),       // tech database 3
   ui_button_info("2_TDB_03", 12, 81, 59, 88, 3),       // tech database 4

   ui_button_info("2_VFB_04", 9, 509, -1, -1, 4),       // scroll up
   ui_button_info("2_VFB_05", 58, 509, -1, -1, 5),      // scroll down
   ui_button_info("2_VFB_06", 925, 511, 940, 586, 6),   // play
   ui_button_info("2_VFB_07", 918, 689, 940, 661, 7),   // exit
   }
};

// text
#define NUM_CUTSCENE_TEXT           6
UI_XSTR Cutscene_text[GR_NUM_RESOLUTIONS][NUM_CUTSCENE_TEXT] = {
  {
   // GR_640
   {
    "Technical Database",
    1055,
    37,
    7,
    UI_XSTR_COLOR_PINK,
    -1,
    &Buttons[0][TECH_DATABASE_BUTTON].button},
   {
    "Mission Simulator",
    1056,
    37,
    23,
    UI_XSTR_COLOR_PINK,
    -1,
    &Buttons[0][SIMULATOR_BUTTON].button},
   {
    "Cutscenes",
    1057,
    37,
    38,
    UI_XSTR_COLOR_PINK,
    -1,
    &Buttons[0][CUTSCENES_BUTTON].button},
   {
    "Credits",
    1058,
    37,
    54,
    UI_XSTR_COLOR_PINK,
    -1,
    &Buttons[0][CREDITS_BUTTON].button},

   {
    "Play",
    1335,
    587,
    366,
    UI_XSTR_COLOR_PINK,
    -1,
    &Buttons[0][PLAY_BUTTON].button},
   {
    "Exit",
    1419,
    587,
    413,
    UI_XSTR_COLOR_PINK,
    -1,
    &Buttons[0][EXIT_BUTTON].button},
   },
  {
   // GR_1024
   {
    "Technical Database",
    1055,
    59,
    12,
    UI_XSTR_COLOR_PINK,
    -1,
    &Buttons[1][TECH_DATABASE_BUTTON].button},
   {
    "Mission Simulator",
    1056,
    59,
    37,
    UI_XSTR_COLOR_PINK,
    -1,
    &Buttons[1][SIMULATOR_BUTTON].button},
   {
    "Cutscenes",
    1057,
    59,
    62,
    UI_XSTR_COLOR_PINK,
    -1,
    &Buttons[1][CUTSCENES_BUTTON].button},
   {
    "Credits",
    1058,
    59,
    88,
    UI_XSTR_COLOR_PINK,
    -1,
    &Buttons[1][CREDITS_BUTTON].button},

   {
    "Play",
    1335,
    956,
    586,
    UI_XSTR_COLOR_PINK,
    -1,
    &Buttons[1][PLAY_BUTTON].button},
   {
    "Exit",
    1419,
    956,
    671,
    UI_XSTR_COLOR_PINK,
    -1,
    &Buttons[1][EXIT_BUTTON].button},
   }
};

int Cutscene_list_coords[GR_NUM_RESOLUTIONS][4] = {
  {
   // GR_640
   9,
   117,
   700,
   198},
  {
   // GR_1024
   20,
   188,
   994,
   316}
};

int Cutscene_desc_coords[GR_NUM_RESOLUTIONS][4] = {
  {
   // GR_640
   9,
   378,
   484,
   73},
  {
   // GR_1024
   14,
   605,
   775,
   117}
};

#define MAX_TEXT_LINES      20
int Cutscene_max_text_lines[GR_NUM_RESOLUTIONS] = {
  10,
  MAX_TEXT_LINES
};

#define MAX_TEXT_LINE_LEN   256

static int Text_size;
static int Text_offset = 0;
static int Text_line_size[MAX_TEXT_LINES];
static char *Text_lines[MAX_TEXT_LINES];


int cutscenes_validate_cd(char *mve_name, int prompt_for_cd)
{
  int cd_present = 0;
  int cd_drive_num;
  int cd_mve_is_on;
  char volume_name[128];

#ifdef RELEASE_REAL
  int num_attempts = 0;
#endif

  while (1)
    {
      int path_set_ok;

      cd_mve_is_on = cutscenes_get_cd_num(mve_name);
      if ((cd_mve_is_on != 0) && (cd_mve_is_on != 1) && (cd_mve_is_on != 2))
        {
          cd_present = 0;
          break;
        }

#if defined(OEM_BUILD)
      sprintf(volume_name, NOX("FS2_OEM"));
#else
      sprintf(volume_name, NOX("FREESPACE2_%c"), '1' + cd_mve_is_on);
#endif


      cd_drive_num = find_freespace_cd(volume_name);
      path_set_ok = set_cdrom_path(cd_drive_num);

      if (path_set_ok)
        {
          cd_present = 1;
          break;
        }

#ifdef RELEASE_REAL
      if (!prompt_for_cd)
        {
          cd_present = 0;
          break;
        }

      // no CD found, so prompt user
      char popup_msg[256];
      int popup_rval;

#if defined(DVD_MESSAGE_HACK)
      sprintf(popup_msg,
              XSTR("Movie not found\n\nInsert FreeSpace DVD to continue",
                   203));
#else
      sprintf(popup_msg,
              XSTR("Movie not found\n\nInsert FreeSpace CD #%d to continue",
                   203), cd_mve_is_on + 1);
#endif

      popup_rval = popup(PF_BODY_BIG, 2, POPUP_CANCEL, POPUP_OK, popup_msg);
      if (popup_rval != 1)
        {
          cd_present = 0;
          break;
        }

      if (num_attempts++ > 5)
        {
          cd_present = 0;
          break;
        }
#else
      cd_present = 0;
      break;
#endif

    }

  return cd_present;
}

void cutscenes_screen_play()
{
  char name[MAX_FILENAME_LEN];  // *full_name
  int which_cutscene;

  Assert((Selected_line >= 0) && (Selected_line < Num_files));
  which_cutscene = Cutscene_list[Selected_line];

  strcpy_s(name, Cutscenes[which_cutscene].filename);
  //  full_name = cf_add_ext(name, NOX(".mve"));

  main_hall_stop_music();
  main_hall_stop_ambient();
  int rval = movie_play(name);
  main_hall_start_music();

  if (!rval)
    {
      char str[256];

      if (Cmdline_nomovies)
        sprintf(str, XSTR("Movies are currently disabled.", -1));
      else
        sprintf(str, XSTR("Unable to play movie %s.", 204),
                Cutscenes[which_cutscene].name);

      popup(0, 1, POPUP_OK, str);
    }

}

void cutscenes_screen_scroll_line_up()
{
  if (Selected_line)
    {
      Selected_line--;
      gamesnd_play_iface(SND_SCROLL);

    }
  else
    gamesnd_play_iface(SND_GENERAL_FAIL);

  if (Selected_line < Scroll_offset)
    Scroll_offset = Selected_line;
}

void cutscenes_screen_scroll_line_down()
{
  int h;

  if (Selected_line < Num_files - 1)
    {
      Selected_line++;
      gamesnd_play_iface(SND_SCROLL);

    }
  else
    gamesnd_play_iface(SND_GENERAL_FAIL);

  h = Cutscene_list_coords[gr_screen.res][3] / gr_get_font_height();
  if (Selected_line >= Scroll_offset + h)
    {
      Scroll_offset++;
    }
}

void cutscenes_screen_scroll_screen_up()
{
  int h;

  if (Scroll_offset)
    {
      Scroll_offset--;
      Assert(Selected_line > Scroll_offset);
      h = Cutscene_list_coords[gr_screen.res][3] / gr_get_font_height();
      while (Selected_line >= Scroll_offset + h)
        {
          Selected_line--;
        }

      gamesnd_play_iface(SND_SCROLL);

    }
  else
    {
      gamesnd_play_iface(SND_GENERAL_FAIL);
    }
}

void cutscenes_screen_scroll_screen_down()
{
  int h;

  h = Cutscene_list_coords[gr_screen.res][3] / gr_get_font_height();
  if (Scroll_offset + h < Num_files)
    {
      Scroll_offset++;
      if (Selected_line < Scroll_offset)
        {
          Selected_line = Scroll_offset;
        }

      gamesnd_play_iface(SND_SCROLL);
    }
  else
    {
      gamesnd_play_iface(SND_GENERAL_FAIL);
    }
}

int cutscenes_screen_button_pressed(int n)
{
  switch (n)
    {
    case TECH_DATABASE_BUTTON:
      gamesnd_play_iface(SND_SWITCH_SCREENS);
      gameseq_post_event(GS_EVENT_TECH_MENU);
      return 1;

    case SIMULATOR_BUTTON:
      gamesnd_play_iface(SND_SWITCH_SCREENS);
      gameseq_post_event(GS_EVENT_SIMULATOR_ROOM);
      return 1;

    case CREDITS_BUTTON:
      gamesnd_play_iface(SND_SWITCH_SCREENS);
      gameseq_post_event(GS_EVENT_CREDITS);
      return 1;

    case SCROLL_UP_BUTTON:
      cutscenes_screen_scroll_screen_up();
      break;

    case SCROLL_DOWN_BUTTON:
      cutscenes_screen_scroll_screen_down();
      break;

    case PLAY_BUTTON:
      cutscenes_screen_play();
      break;

    case EXIT_BUTTON:
      gamesnd_play_iface(SND_COMMIT_PRESSED);
      gameseq_post_event(GS_EVENT_MAIN_MENU);
      game_flush();
      break;
    }

  return 0;
}

void cutscenes_screen_init()
{
  int i;
  size_t j;
  ui_button_info *b;

  Ui_window.create(0, 0, gr_screen.max_w_unscaled, gr_screen.max_h_unscaled,
                   0);
  Ui_window.set_mask_bmap(Cutscene_mask_name[gr_screen.res]);

  for (i = 0; i < NUM_BUTTONS; i++)
    {
      b = &Buttons[gr_screen.res][i];

      b->button.create(&Ui_window, "", b->x, b->y, 60, 30, (i < 2), 1);
      // set up callback for when a mouse first goes over a button
      b->button.set_highlight_action(common_play_highlight_sound);
      b->button.set_bmaps(b->filename);
      b->button.link_hotspot(b->hotspot);
    }

  // add xstrs
  for (i = 0; i < NUM_CUTSCENE_TEXT; i++)
    {
      Ui_window.add_XSTR(&Cutscene_text[gr_screen.res][i]);
    }

  Buttons[gr_screen.res][EXIT_BUTTON].button.
    set_hotkey(KEY_CTRLED | KEY_ENTER);
  Buttons[gr_screen.res][SCROLL_UP_BUTTON].button.set_hotkey(KEY_PAGEUP);
  Buttons[gr_screen.res][SCROLL_DOWN_BUTTON].button.set_hotkey(KEY_PAGEDOWN);

  List_region.create(&Ui_window, "", Cutscene_list_coords[gr_screen.res][0],
                     Cutscene_list_coords[gr_screen.res][1],
                     Cutscene_list_coords[gr_screen.res][2],
                     Cutscene_list_coords[gr_screen.res][3], 0, 1);
  List_region.hide();

  // set up hotkeys for buttons so we draw the correct animation frame when a key is pressed
  Buttons[gr_screen.res][SCROLL_UP_BUTTON].button.set_hotkey(KEY_PAGEUP);
  Buttons[gr_screen.res][SCROLL_DOWN_BUTTON].button.set_hotkey(KEY_PAGEDOWN);

  Background_bitmap = bm_load(Cutscene_bitmap_name[gr_screen.res]);
  Scroll_offset = Selected_line = 0;
  Description_index = -1;

  // when doing a debug version, just put all of the movie files here.
#ifndef NDEBUG
  //Cutscenes_viewable = 0xffffffff;          // makes all cutscenes viewble.
#endif

  //      if (All_movies_enabled)
  //          Cutscenes_viewable = 0xffffffff;        //  Cheat code enables all movies.

  Num_files = 0;
  Cutscene_list.clear();
  for (j = 0; j < Num_cutscenes; j++)
    {
      if (Cutscenes_viewable & (1 << j))
        {
          Cutscene_list.push_back((int) j);
          Num_files++;
        }
    }
}

void cutscenes_screen_close()
{
  if (Background_bitmap)
    bm_release(Background_bitmap);

  Ui_window.destroy();
}

void cutscenes_screen_do_frame()
{
  int i, k, y, z;
  int font_height = gr_get_font_height();
  int select_tease_line = -1;

  k = Ui_window.process();
  switch (k)
    {
    case KEY_DOWN:             // select next line
      cutscenes_screen_scroll_line_down();
      break;

    case KEY_UP:               // select previous line
      cutscenes_screen_scroll_line_up();
      break;

    case KEY_TAB:
    case KEY_CTRLED | KEY_DOWN:
      cutscenes_screen_button_pressed(CREDITS_BUTTON);
      break;

    case KEY_SHIFTED | KEY_TAB:
    case KEY_CTRLED | KEY_UP:
      cutscenes_screen_button_pressed(SIMULATOR_BUTTON);
      break;

    case KEY_ENTER:
      cutscenes_screen_play();
      break;

    case KEY_ESC:              // cancel
      gameseq_post_event(GS_EVENT_MAIN_MENU);
      game_flush();
      break;

    case KEY_F1:               // show help overlay
      break;

    case KEY_F2:               // goto options screen
      gameseq_post_event(GS_EVENT_OPTIONS_MENU);
      break;

#ifndef NDEBUG
      // the "show-all" hotkey
    case KEY_CTRLED | KEY_SHIFTED | KEY_S:
      {
        Num_files = 0;
        Cutscene_list.clear();
        for (size_t t = 0; t < Num_cutscenes; t++)
          {
            Cutscene_list.push_back((int) t);
            Num_files++;
          }

        break;
      }
#endif

    }                           // end switch

  for (i = 0; i < NUM_BUTTONS; i++)
    {
      if (Buttons[gr_screen.res][i].button.pressed())
        {
          if (cutscenes_screen_button_pressed(i))
            {
              return;
            }
        }
    }

  if (List_region.button_down())
    {
      List_region.get_mouse_pos(NULL, &y);
      z = Scroll_offset + y / font_height;
      if ((z >= 0) && (z < Num_files))
        select_tease_line = z;
    }

  if (List_region.pressed())
    {
      List_region.get_mouse_pos(NULL, &y);
      z = Scroll_offset + y / font_height;
      if ((z >= 0) && (z < Num_files))
        Selected_line = z;
    }

  GR_MAYBE_CLEAR_RES(Background_bitmap);
  if (Background_bitmap >= 0)
    {
      gr_set_bitmap(Background_bitmap);
      gr_bitmap(0, 0);
    }

  Ui_window.draw();

  for (i = TECH_DATABASE_BUTTON; i <= CREDITS_BUTTON; i++)
    {
      if (Buttons[gr_screen.res][i].button.button_down())
        {
          break;
        }
    }

  if (i > CREDITS_BUTTON)
    {
      Buttons[gr_screen.res][CUTSCENES_BUTTON].button.draw_forced(2);
    }

  y = 0;
  z = Scroll_offset;
  while (y + font_height <= Cutscene_list_coords[gr_screen.res][3])
    {
      if (z >= Num_files)
        {
          break;
        }

      if (z == Selected_line)
        {
          gr_set_color_fast(&Color_text_selected);
        }
      else if (z == select_tease_line)
        {
          gr_set_color_fast(&Color_text_subselected);
        }
      else
        {
          gr_set_color_fast(&Color_text_normal);
        }

      gr_printf(Cutscene_list_coords[gr_screen.res][0],
                Cutscene_list_coords[gr_screen.res][1] + y,
                Cutscenes[Cutscene_list[z]].name);

      y += font_height;
      z++;
    }

  if (Description_index != Selected_line)
    {
      char *src = NULL;

      Description_index = Selected_line;
      Text_size = 0;
      if (Description_index < (int) Cutscene_list.size() &&
          (int) Cutscene_list[Description_index] < (int) Cutscenes.size())
        {
          src = Cutscenes[Cutscene_list[Description_index]].description;
          if (src)
            {
              Text_size =
                split_str(src, Cutscene_desc_coords[gr_screen.res][2],
                          Text_line_size, Text_lines,
                          Cutscene_max_text_lines[gr_screen.res]);
              Assert(Text_size >= 0
                     && Text_size < Cutscene_max_text_lines[gr_screen.res]);
            }
        }
    }

  if (Description_index >= 0)
    {
      int len;
      char line[MAX_TEXT_LINE_LEN + 1];

      gr_set_color_fast(&Color_text_normal);

      y = 0;
      z = Text_offset;
      while (y + font_height <= Cutscene_desc_coords[gr_screen.res][3])
        {
          if (z >= Text_size)
            break;

          len = Text_line_size[z];
          if (len > MAX_TEXT_LINE_LEN)
            len = MAX_TEXT_LINE_LEN;

          strncpy(line, Text_lines[z], len);
          line[len] = 0;
          gr_string(Cutscene_desc_coords[gr_screen.res][0],
                    Cutscene_desc_coords[gr_screen.res][1] + y, line);

          y += font_height;
          z++;
        }
    }

  gr_flip();
}
