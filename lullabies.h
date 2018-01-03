/*
 *      Copyright 2017 Particle Industries, Inc.
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 *
 */
#include "pitches.h"

#define SONG_COUNT                          4

#define SONG_BAABAA                         0
#define SONG_MARY                           1
#define SONG_TWINKLE                        2
#define SONG_WHITENOISE                     3

/*
#define   Do        523/2
#define  Do_        1046/2
#define   Re        587/2
#define  Re_        1175/2
#define   Mi        659/2
#define  Mi_        1318/2
#define   Fa        698/2
#define  Fa_        1397/2
#define   So        784/2
#define   La        880/2
#define   Si        988/2
#define  _Si        494/2
#define  _La        440/2
#define  _So        392/2
 
#define   bt        323
#define   bt2       bt*2
#define   bt3       bt*3
#define   bt6       bt*6
#define   bt1_2     bt/2
#define   bt3_2     (bt/2)*3
#define   bt1_4     bt/4
*/

/*
 *      Credit:
 *          Midi to Arduino Converter
 *              - Andy Tran (extramaster), 2015
 *          https://www.extramaster.net/tools/midiToArduino/
 *
 */

/* Song List */
String songList = "Baa Baa Black Sheep, Mary Had A Little Lamb, Twinkle Twinkle Little Star, White Noise Maker";

/* An indicator to exit the current song */
bool changeSong = false;

bool delaySong(unsigned duration)
{
    while (duration > 0 && !changeSong) {
        unsigned thisDelay = duration > 100 ? 100 : duration;
        delay(thisDelay);
        duration -= thisDelay;
    }
    return changeSong;
}

/*
 *
 *      Song definitionss
 *
 */

#define NOTE_COUNT_BAABAA 54
#define baabaa_TEMPO 1200

/* Baa Baa Black Sheep (Have you any wool)*/
int baabaa_melody[] = {
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4,
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_A4, NOTE_G4,
  NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4,
  NOTE_D4, NOTE_D4, NOTE_C4,
   
  NOTE_G4, NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_F4,
  NOTE_E4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4,
  NOTE_G4, NOTE_G4, NOTE_G4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_F4, 
  NOTE_E4, NOTE_E4, NOTE_E4, NOTE_D4,
 
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4,
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_A4, NOTE_G4,
  NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4,
  NOTE_D4, NOTE_D4, NOTE_C4,
};
   
int baabaa_melodyNoteDurations[] = {
  4,  4,  4,  4,
  8,  8,  8,  8,  2,
  4,  4,  4,  4,
  4,  4,  2,
  4,  8,  8,  4,  8, 8,
  4,  8,  8,  3,  8,
  4,  8,  8,  8,  8, 8, 8,
  4,  8,  8,  2,
  4,  4,  4,  4,
  8,  8,  8,  8,  2,
  4,  4,  4,  4,
  4,  4,  2,
};


/*
 Mary Had a Little Lamb v2
 
 created 21 Jan 2010
 modified 30 Aug 2011
 made awesome 14 May 2012
 by Tom Igoe and Brad Osgood 
 modified 2 Jan 2018 by David Bradway
This example code is in the public domain.
 
 http://arduino.cc/en/Tutorial/Tone
 
 */
#define NOTE_COUNT_MARY 26

//http://www.true-piano-lessons.com/mary-had-a-little-lamb.html
// Key of F, notes in the melody:
int MARY_melody[] = {
  NOTE_A3, NOTE_G3, NOTE_F3, NOTE_G3, NOTE_A3, NOTE_A3, NOTE_A3,
  NOTE_G3, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_C4, NOTE_C4,
  NOTE_A3, NOTE_G3, NOTE_F3, NOTE_G3, NOTE_A3, NOTE_A3, NOTE_A3,
  NOTE_A3, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, NOTE_F3
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int MARY_noteDurations[] = {
  4,4,4,4,
  4,4,2,
  4,4,2,
  4,4,2,
  4,4,4,4,
  4,4,4,4,
  4,4,4,4,
  2
};

void MARY_chord(int note, int duration) {
  // to calculate the note duration, take one second 
  // divided by the note type.
  //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
  // int noteDuration = 1000 / noteDurations[thisNote];
  int noteDuration = 1000 / duration;
  tone(PIN_BUZZER, note, noteDuration);

  // to distinguish the notes, set a minimum time between them.
  // the note's duration + 30% seems to work well:
  delay(noteDuration * 1.30);

  // stop the tone playing:
  noTone(PIN_BUZZER);
}


/* Twinkle Twinkle Little Star*/
int twinkle[] = {
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4,
  NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4,
  NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4,
  NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4,
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4,
  NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4,
};

// note durations: 4 = quarter note, 8 = eighth note, 2 = half note, 1 = whole note etc.:
int twinkleDurations[] = {
  4, 4, 4, 4, 4, 4, 2,
  4, 4, 4, 4, 4, 4, 2,
  4, 4, 4, 4, 4, 4, 2,
  4, 4, 4, 4, 4, 4, 2,
  4, 4, 4, 4, 4, 4, 2,
  4, 4, 4, 4, 4, 4, 2,
};


/* White Noise Maker */
unsigned long lastClick = micros();

/* initialize with any 32 bit non-zero  unsigned long value. */
#define LFSR_INIT  0xfeedfaceUL
/* Choose bits 32, 30, 26, 24 from  http://arduino.stackexchange.com/a/6725/6628
 *  or 32, 22, 2, 1 from 
 *  http://www.xilinx.com/support/documentation/application_notes/xapp052.pdf
 *  or bits 32, 16, 3,2  or 0x80010006UL per http://users.ece.cmu.edu/~koopman/lfsr/index.html 
 *  and http://users.ece.cmu.edu/~koopman/lfsr/32.dat.gz
 */  
#define LFSR_MASK  ((unsigned long)( 1UL<<31 | 1UL <<15 | 1UL <<2 | 1UL <<1  ))

unsigned int generateNoise(){ 
  // See https://en.wikipedia.org/wiki/Linear_feedback_shift_register#Galois_LFSRs
   static unsigned long int lfsr = LFSR_INIT;  /* 32 bit init, nonzero */
   /* If the output bit is 1, apply toggle mask.
                                    * The value has 1 at bits corresponding
                                    * to taps, 0 elsewhere. */
   if(lfsr & 1) { lfsr =  (lfsr >>1) ^ LFSR_MASK ; return(1);}
   else         { lfsr >>= 1;                      return(0);}
}


/*
 *
 *      Play the Songs
 *
 */
void playBaabaa() {
    for (int thisNote = 0; thisNote < NOTE_COUNT_BAABAA; thisNote++) {
        if (changeSong)
            return;

       int noteDuration = baabaa_TEMPO/baabaa_melodyNoteDurations[thisNote];
       tone(PIN_BUZZER, baabaa_melody[thisNote],noteDuration);
       int pauseBetweenNotes = noteDuration * 1.30;
       delay(pauseBetweenNotes);
       noTone(PIN_BUZZER);
    }
    if (!repeatSong)
        nextSong();
}

void playMary() {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < NOTE_COUNT_MARY; thisNote++) {
    if (changeSong)
        return;

    int note = MARY_melody[thisNote];
    int duration = MARY_noteDurations[thisNote];
    MARY_chord(note, duration);
  }
  if (!repeatSong)
    nextSong();
}

void playTwinkle() {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < (sizeof(twinkleDurations)/2); thisNote++) {
    if (changeSong)
        return;

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int twinkleDuration = 1000/twinkleDurations[thisNote];
    tone(PIN_BUZZER, twinkle[thisNote], twinkleDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = twinkleDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(PIN_BUZZER);
  }
  if (!repeatSong)
    nextSong();
}

// Work in Progress
void playWhiteNoise() {
    while (1) { 
        if (changeSong)
            return;

        if ((micros() - lastClick) > 10 ) { // Changing this value changes the frequency.
            lastClick = micros();
            digitalWrite (PIN_BUZZER, generateNoise());
        }
    }
    // Note funcitonality below is broken due to the while(1)
    if (!repeatSong)
        nextSong();
}
