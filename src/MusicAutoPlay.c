#include "MusicAutoPlay.h"
//#include "Lib_songs.h"
#include "definitions.h"

extern T_Song *cur_Song;

uint8_t Music_auto_play(void) {
  static uint16_t note_interval = 0;
  //T_Song *t_song = cur_Song;
  // (T_Song *) cur_Song

  if (cur_Song == NULL)
  {
    OCMP2_Disable();
    return 0;
  }
  

  if (0 == note_interval) { //load a new note
    uint16_t note = cur_Song->t_script[cur_Song->note_index].Note;
    note_interval = cur_Song->t_script[cur_Song->note_index].Time;

    // Control the buzzeer
    if (0 == note) {
        OCMP2_Disable();
    } else {
        TMR2_PeriodSet(10000000 / note);
        OCMP2_CompareSecondaryValueSet(TMR2_PeriodGet() / 20);
        OCMP2_Enable();
    }

    // Music final action
    if (++(cur_Song->note_index) >= cur_Song->note_num) {
      // Music_deinit(auto_play_task, t_song);
      cur_Song->note_index = 0;
      cur_Song = NULL;
      note_interval = 0;
      OCMP2_Disable();
    }
  } else {
    note_interval--;
  }
  return 1;
}