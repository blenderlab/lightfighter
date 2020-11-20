#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN    1

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 30

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT * 8, LED_PIN, NEO_GRB + NEO_KHZ800);

const int pb_pins[8] = {2, 4, 5, 6, 7, 8, 9, 10 };
bool pb_state[8] = {0, 0, 0, 0, 0, 0, 0, 0 };
byte player_scores[8] = {0, 0, 0, 0, 0, 0, 0, 0 };
bool player[] = {false, false, false, false, false, false, false, false};

//nom des joueurs
String player_names[] = {"alpha", "bravo", "charlie", "delta", "echo", "foxtrot", "golf", "hotel"};

const int bpStart = 13;

// Déterminer le nombre de joueurs
int nb_players = 0;

bool isstart = false;
bool ingame = false;
#define INTRO 0 // 0 = waiting for players;
#define WAITSTART 1 // 1 = Waiting to start
#define RUNNING 2 // 2 = game running
#define LOST 3 // 3 = one player lost..
#define GAMEOVER 4 // 4 = game over !
#define ENGAGE 5 // When a player has to launch palet.
byte game_state = INTRO;


// Objet "palet" (led)
//  No branche (camp joueur No XX )
//  Position (dans la branche)
//  Direction (aller/retour)
class _Palet {
  public:
    byte num_branch = 0;
    byte position = 0;
    int speed = 0;

    void toggledir(void) {
      speed  = -speed;
    }
    void update(void) {
      position = position + speed;
      if (position <= 0 ) {
        game_state = LOST;
      }
      if (position >= LED_COUNT) {
        position=LED_COUNT;
        int newbranch=random(0,7); // random branch 
        while(!player[newbranch]){ // Check if we have a pler here :
          newbranch=random(0,7);  /// Else, try another one.
        }
        num_branch = newbranch;
        toggledir();
      }
    }

    void display(void) {
      strip.clear();
      strip.setPixelColor(convertPos2NumLed(), strip.Color(255, 0, 0));
      strip.show();

    }
    int convertPos2NumLed(void) {
      return (LED_COUNT * (num_branch - 1 ) + position);
    }
};


_Palet palet;


void setup() {
  for (int i = 0; i <= 7; i++)
    pinMode(pb_pins[i], INPUT_PULLUP); // Initialise la broche 2 comme entrée

  pinMode(bpStart, INPUT_PULLUP); // Initialise la broche 5 comme entrée
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  Serial.begin(9600);     // Ouvre le port série à 9600 bauds
}

void loop()
{
  switch (game_state) {
    case INTRO:
      intro();
      break;
    case WAITSTART:
      wait_to_start();
      break;
    case RUNNING:
      main_game();
      break;
    case LOST:
      player_lost();
      break;
    case GAMEOVER:
      game_over();
      break;
    case ENGAGE:
      main_game();
      break;
  }
}

void intro() {
  // Not enough players,
  //displaying cool stuf ...

  // And wait for players...
  wait_for_players();
}
void wait_for_players() {
  // Wait for at least one to press
  for (int i = 0; i <= 7; i++) {
    if (digitalRead(pb_pins[i] == LOW)) {
      // not yet pressed
      if (pb_state[i] == 0) {
        pb_state[i] = 1; // button is pressed.
        player[i] = !player[i];
        Serial.print(player_names[i]);
        if (player[i]) {
          nb_players++;
          Serial.println(" enter the game");
        } else {
          nb_players--;
          Serial.println(" left the game");
        }
      } else {
        pb_state[i] = 0; // button is released.
      }
    }
    if (nb_players > 2) {
      // Enough players, we can prepare to strt
      game_state = WAITSTART;
    } else {
      // Not enough, go back in tro mode :
      game_state = INTRO;
    }
  }
}

void wait_to_start() {
  // Players are arriving until PBStart pressed
  wait_for_players();

  // Display a ready to start game :
  if (digitalRead(bpStart) == LOW )
  {
    game_state = RUNNING;
  }
}

void main_game() {
  // Update palet :
  palet.update();
  palet.display();
  // Read buttons :
  for (int i = 0; i <= 7; i++)  pb_state[i] = !digitalRead(pb_pins[i]);
 
  // Manage players shots :
  for (int i = 0; i <= 7; i++) {
    if (pb_state[i] && palet.num_branch == (i + 1) && palet.position < 2 ) {
      if (game_state == ENGAGE) {
        palet.speed = 1;
      } else {
        palet.toggledir();
      //  palet.speed += 1;
      }
    }
  }
}

void player_lost() {
  Serial.println(player_names[palet.num_branch] + " Lost ! ");
  palet.position =  palet.speed = 0;
  game_state = ENGAGE;
  for (int i=0;i<=3;i++){
    blink_branch(palet.num_branch);
  }
}

void blink_branch(int i){
  
}
void game_over() {
}
