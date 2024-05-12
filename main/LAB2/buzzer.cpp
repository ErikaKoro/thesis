#include "buzzer.hpp"
#include "Arduino.h"

void setBuzzer(notes notes){

    for (int thisNote = 0; thisNote < 31; thisNote++) {

        int noteDuration = 3000 / notes.duration[thisNote];
        tone(BUZZER_PIN, notes.note[thisNote], noteDuration);

        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        noTone(BUZZER_PIN);
    }
}

notes fur_elise(){
    static int melody[] = {NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_DS5, NOTE_E5,
                    NOTE_B4, NOTE_D5, NOTE_C5, NOTE_A4, 0, NOTE_C4, NOTE_E4,
                    NOTE_A4, NOTE_B4, 0, NOTE_E4, NOTE_GS4, NOTE_B4, NOTE_C5,
                    0, NOTE_E4, NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_DS5,
                    NOTE_E5, NOTE_B4, NOTE_D5, NOTE_C5, NOTE_A4, 0};

    int noteDurations[] = {
                          16, 16, 16, 16, 16, 16,
                          16, 16, 8, 16, 16, 16,
                          16, 8, 16, 16, 16, 16,
                          8, 16, 16, 16, 16, 16,
                          16, 16, 16, 16, 16, 8, 16
                          };
    
    notes fur_elise;
    for(int i = 0; i < 31; i++){
        fur_elise.note[i] = melody[i];
        fur_elise.duration[i] = noteDurations[i];
    }

    return fur_elise;
}