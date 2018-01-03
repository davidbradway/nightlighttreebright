// This #include statement was automatically added by the Particle IDE.
#include <dotstar.h>
#include "nightlighttreebright.h"       // All macros (#define) are in .h

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

/*
 *      Project: Xmas Tree
 *
 *      Description:
 *          This is a Xmas tree with 25 LEDs, a piezo buzzer and a 5-way joystick designed for Particle Photon
 *          and Electron connected to the cloud which demostrates how to:
 *              1.  define cloud functions for controlling the tree over the Particle Cloud.
 *              2.  generate tones using the piezo buzzer
 *              3.  create beautiful effects using the LEDs
 *              4.  control the tree state using the 5-way joystick.
 *
 */

/* Project name and version */
#define WHOAMI              "NightLightTreeBright"
#define VERSION             1

/* Project name and version */
const char projName[] = WHOAMI;
const int version = VERSION;

/* Current state of the effect when pressing the center button */
int currentState = STATE_NONE;

/* Current brightness of the LEDs */
/* Warning: less than 80 is highly recommended for safety and for your eyes */
int currentBrightness = 30;

/* Set to TRUE to play the current song repeatedly */
/* FALSE doesn't really work due to the white noise maker */
bool repeatSong = TRUE;

#include "lullabies.h"                  // The no. of songs and melodies are defined in lullabies.h
#include "pitches.h"
int songCount = SONG_COUNT;

/* Current song selected */
int currentSong = SONG_BAABAA;

/* Current button down */
volatile int buttonState = BUTTON_NONE;

/* Create an instance for the xmasTree LEDs, each LED controlled by two pins, DATA and Clock */
Adafruit_DotStar leds = Adafruit_DotStar(TOTAL_LED, PIN_LED_DATA, PIN_LED_CLOCK, DOTSTAR_BGR);

/* Two threads will be used for processing songs and LED state */
Thread *songWorker;
Thread *lightWorker;

/* LEDs hardware initialization */
void ledsInit()
{
    leds.setBrightness(currentBrightness);
    leds.begin();
    leds.clear();
    playLED("0xFFFFFF");
}

/* 5-way joystick hardware initialization */
void joystickPortInit()
{
    /* Set pins to input with internal pull-up */
    pinMode(PIN_CENTER, INPUT_PULLUP);
    pinMode(PIN_UP, INPUT_PULLUP);
    pinMode(PIN_DOWN, INPUT_PULLUP);
    pinMode(PIN_LEFT, INPUT_PULLUP);
    pinMode(PIN_RIGHT, INPUT_PULLUP);
    
    /* Using interrupts for the pins */
    attachInterrupt(PIN_CENTER, buttonHandler, FALLING);
    attachInterrupt(PIN_UP, buttonHandler, FALLING);
    attachInterrupt(PIN_DOWN, buttonHandler, FALLING);
    attachInterrupt(PIN_LEFT, buttonHandler, FALLING);
    attachInterrupt(PIN_RIGHT, buttonHandler, FALLING);
}

/* Piezo buzzer hardware initialization */
void buzzerPortInit(void)
{
    pinMode(PIN_BUZZER, OUTPUT);
}

/* Set all the LEDs to color, e.g. "0xFF0000" = Red */
int playLED(String color)
{
    for (int i = 0; i < TOTAL_LED; i++)
        leds.setPixelColor(i, strtol(color, NULL, 0));
    leds.show();
}

/* Turn off all LEDs */
void stopLED()
{
    playLED("0x000000"); // R,G,B = 0,0,0
}

void playTone(int freq, int duration)
{
    Serial.println("playTone");
    tone(PIN_BUZZER, freq, duration);
}

void stopTone(String)
{
    noTone(PIN_BUZZER);
}

/* For publishing the device-connected event to the cloud */
void publishConnected()
{
    Serial.println("publishConnected");
    Particle.publish("nightLightConnected", projName);
}

/* For publishing the effect state changed event to the cloud */
void publishStateChanged()
{
    Serial.println("publishStateChanged");
    Particle.publish("stateChanged", String(currentState));
}

/* For publishing the animation changed event to the cloud */
void publishBrightnessChanged()
{
    Serial.println("publishBrightnessChanged");
    Particle.publish("brightnessChanged", String(currentBrightness));
}

/* For publishing the song changed event to the cloud */
void publishSongChanged()
{
    Serial.println("publishSongChanged");
    Particle.publish("songChanged", String(currentSong));
}

/* Play a song by the song ID */
int playSong(String songIndex)
{
    Serial.println("playSong");
    
    /* Stop the current playing song at once */
    changeSong = true;

    /* Set the current song and the song worker will play it */
    currentSong = atoi(songIndex);

    /* Update the tree current state */    
    if (currentState == STATE_NONE)
        currentState = STATE_SONG;
    else if (currentState == STATE_LIGHT)
        currentState = STATE_BOTH;
    
    return 1;
}

/* Stop the current song playing */
int stopSong(String)
{
    Serial.println("stopSong");

    /* Stop the current playing song at once */
    changeSong = true;

    /* Update the tree current state */    
    if (currentState == STATE_SONG)
        currentState = STATE_NONE;
    else if (currentState == STATE_BOTH)
        currentState = STATE_LIGHT;
        
    return 1;
}

/* Turn on the Night Light */
int startLight(String)
{
    Serial.println("startLight");

    playLED("0xFFFFFF");
    
    /* Update the tree current state */    
    if (currentState == STATE_NONE)
        currentState = STATE_LIGHT;
    else if (currentState == STATE_SONG)
        currentState = STATE_BOTH;

    return 1;
}

/* Stop the LED cureent playing animation */
int stopLight(String)
{
    Serial.println("stopLight");

    /* Stop the Light */
    stopLED();

    /* Update the tree current state */
    if (currentState == STATE_LIGHT)
        currentState = STATE_NONE;
    else if (currentState == STATE_BOTH)
        currentState = STATE_SONG;

    return 1;
}

/* Toggle the state of the tree for the Center button */
int toggleState()
{
    currentState++;
    if (currentState > STATE_MAX)
        currentState = 0;
    
    Serial.println("toggleState");

    if (currentState == STATE_NONE || currentState == STATE_SONG)
        stopLight("");

    if (currentState == STATE_NONE || currentState == STATE_LIGHT)
        stopSong("");

    if (currentState == STATE_BOTH || currentState == STATE_LIGHT)
        startLight("0xFFFFFF");

    return currentState;
}

/* Set the state of the tree */
int setState(String state)
{
    Serial.println("setState");

    if (String(currentState) == state)
        return 0;
        
    if ( (state == "BOTH") || (state == String(STATE_BOTH)) ) {
        playSong(String(currentSong));
        startLight("0xFFFFFF");
        currentState = STATE_BOTH;
    } else if ( (state == "SONG") || (state == String(STATE_SONG)) ) {
        playSong(String(currentSong));
        stopLight("");
        currentState = STATE_SONG;
    } else if ( (state == "LIGHT") || (state == String(STATE_LIGHT)) ) {
        stopSong("");
        startLight("0xFFFFFF");
        currentState = STATE_LIGHT;
    } else if ( (state == "NONE") || (state == String(STATE_NONE)) ) {
        stopSong("");
        stopLight("");
        currentState = STATE_NONE;
    } else
        return -1;

    return 1;
}

/* Increase the brightness of the LED */
int increaseBrightness()
{
    Serial.println("increaseBrightness");

    /* if the state is LED off, then do nothing */ 
    if ( (currentState == STATE_LIGHT) || (currentState == STATE_BOTH) ) {
        currentBrightness+=4;
        if ( currentBrightness >= 80) {
            currentBrightness = 79;
        }
        leds.setBrightness(currentBrightness);
    }
    return currentBrightness;
}

/* Increase the brightness of the LED */
int decreaseBrightness()
{
    Serial.println("decreaseBrightness");

    /* if the state is LED off, then do nothing */ 
    if ( (currentState == STATE_LIGHT) || (currentState == STATE_BOTH) ) {
        currentBrightness-=4;
        if ( currentBrightness <= 0) {
            currentBrightness = 1;
        }
        leds.setBrightness(currentBrightness);
    }
    return currentBrightness;
}

/* Start playing the next song */
int nextSong()
{
    Serial.println("nextSong");

    /* if the state is not playing song, then do nothing */ 
    if ( (currentState == STATE_SONG) || (currentState == STATE_BOTH) ) {
        currentSong++;
        if (currentSong == songCount)
            currentSong = 0;
        
        changeSong = true;
    }
    return currentSong;
}

/* Start playing the previous song */
int prevSong()
{
    Serial.println("prevSong");

    if ( (currentState == STATE_SONG) || (currentState == STATE_BOTH) ) {
        currentSong--;
        if (currentSong < 0)
            currentSong = songCount-1;
                
        changeSong = true;
    }
    return currentSong;
}

/* Process the button states */
int buttonPressed(String direction)
{
    Serial.print("buttonPressed: ");
    Serial.println(direction);
    
    if ( direction == "CENTER" || direction == String(BUTTON_CENTER) )
        return toggleState();
    else if ( direction == "UP" || direction == String(BUTTON_UP) )
        return increaseBrightness();
    else if ( direction == "DOWN" || direction == String(BUTTON_DOWN) )
        return decreaseBrightness();
    else if ( direction == "LEFT" || direction == String(BUTTON_LEFT) )
        return prevSong();
    else if ( direction == "RIGHT" || direction == String(BUTTON_RIGHT) )
        return nextSong();
    else
        return -1;
}

/* Interrupt handler for the 5-way joystick */
void buttonHandler()
{
    if (digitalRead(PIN_CENTER) == LOW)
        buttonState = BUTTON_CENTER;
    else if (digitalRead(PIN_UP) == LOW)
        buttonState = BUTTON_UP;
    else if (digitalRead(PIN_DOWN) == LOW)
        buttonState = BUTTON_DOWN;
    else if (digitalRead(PIN_LEFT) == LOW)
        buttonState = BUTTON_LEFT;
    else if (digitalRead(PIN_RIGHT) == LOW)
        buttonState = BUTTON_RIGHT;
}

/* Process buttons for the 5-way joystick */
void processButtons()
{
    if (buttonState != BUTTON_NONE) {

        /* Wait until button up */        
        if ( (digitalRead(PIN_CENTER) == LOW) || (digitalRead(PIN_UP) == LOW) ||
            (digitalRead(PIN_DOWN) == LOW) || (digitalRead(PIN_LEFT) == LOW) ||
            (digitalRead(PIN_RIGHT) == LOW) )
            return;
        
        if (buttonState == BUTTON_CENTER)
            buttonPressed("CENTER");
        else if (buttonState == BUTTON_UP)
            buttonPressed("UP");
        else if (buttonState == BUTTON_DOWN)
            buttonPressed("DOWN");
        else if (buttonState == BUTTON_LEFT)
            buttonPressed("LEFT");
        else if (buttonState == BUTTON_RIGHT)
            buttonPressed("RIGHT");
        
        /* Clear the button state */
        buttonState = BUTTON_NONE;
    }
}

/* Button pressed from the cloud */ 
void buttonEventHandler(const char *event, const char *data)
{
    if (String(event) == "btnPressed")
        buttonPressed(String(data));
}

/* Process Light */
void processLight()
{
    /* This is an endless loop to process the light */
    while (1) {
        if ( (currentState == STATE_BOTH) || (currentState == STATE_LIGHT) ) {
            startLight("0xFFFFFF");
        }
        else {
            stopLight("");
        }
        delay(2000);
    }
}

/* Process songs */
void processSongs()
{
    /* This is an endless loop to process the songs one by one */
    while (1) {
        changeSong = false;

        if ( (currentState == STATE_BOTH) || (currentState == STATE_SONG) ) {
            if (currentSong == SONG_BAABAA)
                playBaabaa();
            else if (currentSong == SONG_MARY)
                playMary();
            else if (currentSong == SONG_TWINKLE)
                playTwinkle();
            // Work in Progress
            else if (currentSong == SONG_WHITENOISE)
                playWhiteNoise();
        }
        delay(2000);
    }
}

/* Cloud functions, variables, events initialization */
void cloudInit()
{
    /* Cloud variables */
    Particle.variable("whoami", projName);
    Particle.variable("version", version);
    
    Particle.variable("songList", songList);

    Particle.variable("currSong", currentSong);
    Particle.variable("currState", currentState);
    Particle.variable("currBrightns", currentBrightness);
    Particle.variable("songCount", songCount);

    /* Cloud functions */
    Particle.function("playLED", playLED);
    Particle.function("btnPressed", buttonPressed);
    Particle.function("setState", setState);
    Particle.function("playSong", playSong);
    Particle.function("startLight", startLight);
    Particle.function("stopSong", stopSong);
    Particle.function("stopLight", stopLight);

    /* Cloud subscription */
    Particle.subscribe("btnPressed", buttonEventHandler);
    
    /* Publish this event once connected to the cloud */
    publishConnected();
}

/* Process cloud events */
void processCloud()
{
    static int savedSong = -1;
    static int savedState = -1;
    static int savedBrightness = -1;

    if (savedSong != currentSong) {
        savedSong = currentSong;
        publishSongChanged();
    }
    
    if (savedState != currentState) {
        savedState = currentState;
        publishStateChanged();
    }
    
    if (savedBrightness != currentBrightness) {
        savedBrightness = currentBrightness;
        publishBrightnessChanged();
    }
}

/* Print out the tree state to serial port */
void printTree()
{
    static int savedSong = -1;
    static int savedState = -1;
    static int savedBrightness = -1;

    if (savedState != currentState) {
        Serial.print("Current State: ");
        Serial.println(currentState);
        savedState = currentState;
    }

    if (savedSong != currentSong) {
        Serial.print("Current Song: ");
        Serial.println(currentSong);
        savedSong = currentSong;
    }

    if (savedBrightness != currentBrightness) {
        Serial.print("Current Brightness: ");
        Serial.println(currentBrightness);
        savedBrightness = currentBrightness;
    }
}

/* Wiring programming starts from setup() and then calling loop() */
void setup()
{
    /* LEDs setup */
    ledsInit();
    
    /* 5-way joystick setup */
    joystickPortInit();

    /* Buzzer setup */
    buzzerPortInit();

    /* Cloud functions and variables setup */
    cloudInit();
    
    /* Creating a thread for processing LED brightness and songs */
    /* So that I can just need to keep track of the buttons and the cloud activities */
    lightWorker = new Thread("light", processLight);
    songWorker = new Thread("song", processSongs);
    
    /* Start to play the 1st song and turn the LED on */
    setState("BOTH");
    //playSong("0");
    //startLight("0xFFFFFF");
}

/* This will be called repeatedly */
void loop()
{
    /* Print the tree info. */
    printTree();
    
    /* Check if button pressed */
    processButtons();
    
    /* Process the Cloud events */
    processCloud();
}
