#pragma once

#include <cli/cli.h>
#include <dialogs/dialogs.h>
#include <furi.h>
#include <gui/gui.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include <stdio.h>
#include <toolbox/stream/file_stream.h>
#include <storage/storage.h>
#include <u8g2_glue.h>

#include <gui/modules/text_input.h>
#include <gui/view_dispatcher.h>

#include "flizzer_tracker_hal.h"
#include "sound_engine/freqs.h"
#include "sound_engine/sound_engine_defs.h"
#include "sound_engine/sound_engine_filter.h"
#include "tracker_engine/tracker_engine_defs.h"

#define FLIZZER_TRACKER_FOLDER "/ext/flizzer_tracker"
#define FILE_NAME_LEN 64

typedef enum
{
    EventTypeInput,
    EventTypeSaveSong,
} EventType;

typedef struct
{
    EventType type;
    InputEvent input;
    uint32_t period;
} FlizzerTrackerEvent;

typedef enum
{
    PATTERN_VIEW,
    INST_EDITOR_VIEW,
    EXPORT_WAV_VIEW,
} TrackerMode;

typedef enum
{
    EDIT_PATTERN,
    EDIT_SEQUENCE,
    EDIT_SONGINFO,
    EDIT_INSTRUMENT,
    EDIT_PROGRAM,
} TrackerFocus;

typedef enum
{
    SI_PATTERNPOS,
    SI_SEQUENCEPOS,
    SI_SONGSPEED,
    SI_SONGRATE,
    SI_MASTERVOL,

    SI_SONGNAME,
    SI_CURRENTINSTRUMENT,
    SI_INSTRUMENTNAME,
    /* ========  */
    SI_PARAMS,
} SongInfoParam;

typedef enum
{
    INST_CURRENTINSTRUMENT,
    INST_INSTRUMENTNAME,

    INST_CURRENT_NOTE,
    INST_FINETUNE,

    INST_SLIDESPEED,
    INST_SETPW,
    INST_PW,
    INST_SETCUTOFF,

    INST_WAVE_NOISE,
    INST_WAVE_PULSE,
    INST_WAVE_TRIANGLE,
    INST_WAVE_SAWTOOTH,
    INST_WAVE_NOISE_METAL,
    INST_WAVE_SINE,

    INST_ATTACK,
    INST_DECAY,
    INST_SUSTAIN,
    INST_RELEASE,
    INST_VOLUME,

    INST_ENABLEFILTER,
    INST_FILTERCUTOFF,
    INST_FILTERRESONANCE,

    INST_FILTERTYPE,
    INST_ENABLERINGMOD,
    INST_RINGMODSRC,
    INST_ENABLEHARDSYNC,
    INST_HARDSYNCSRC,

    INST_RETRIGGERONSLIDE,
    INST_ENABLEKEYSYNC,

    INST_ENABLEVIBRATO,
    INST_VIBRATOSPEED,
    INST_VIBRATODEPTH,
    INST_VIBRATODELAY,

    INST_ENABLEPWM,
    INST_PWMSPEED,
    INST_PWMDEPTH,
    INST_PWMDELAY,

    INST_PROGRAMEPERIOD,
    /* ========= */
    INST_PARAMS,
} InstrumentParam;

typedef struct
{
    View *view;
    void *context;
} TrackerView;

typedef enum
{
    VIEW_TRACKER,
    VIEW_KEYBOARD,
    VIEW_SUBMENU_PATTERN,
    VIEW_SUBMENU_INSTRUMENT,
    VIEW_FILE_OVERWRITE,
} FlizzerTrackerViews;

typedef enum
{
    SUBMENU_PATTERN_RETURN,
    SUBMENU_PATTERN_LOAD_SONG,
    SUBMENU_PATTERN_SAVE_SONG,
    SUBMENU_PATTERN_EXIT,
} PatternSubmenuParams;

typedef enum
{
    SUBMENU_INSTRUMENT_RETURN,
    SUBMENU_INSTRUMENT_EXIT,
} InstrumentSubmenuParams;

typedef struct
{
    NotificationApp *notification;
    FuriMessageQueue *event_queue;
    Gui *gui;
    TrackerView *tracker_view;
    ViewDispatcher *view_dispatcher;
    TextInput *text_input;
    Storage* storage;
    Stream *stream;
    FuriString* filepath;
    DialogsApp *dialogs;
    Submenu *pattern_submenu;
    Submenu *instrument_submenu;
    Widget* overwrite_file_widget;
    char filename[FILE_NAME_LEN + 1];
    bool was_it_back_keypress;
    uint32_t current_time;
    uint32_t period;

    SoundEngine sound_engine;
    TrackerEngine tracker_engine;

    TrackerSong song;

    uint8_t selected_param;

    uint8_t mode, focus;
    uint8_t patternx, current_channel, current_digit, program_position, current_program_step, current_instrument, current_note;

    uint8_t inst_editor_shift;

    bool editing;
    bool was_editing;

    bool is_loading;
    bool is_saving;

    bool quit;
} FlizzerTrackerApp;

typedef struct
{
    FlizzerTrackerApp *tracker;
} TrackerViewModel;

void draw_callback(Canvas *canvas, void *ctx);
bool input_callback(InputEvent *input_event, void *ctx);