
#include <LiquidCrystal_I2C.h>
#include <FastLED.h>
#include "FastLED_RGBW.h"
#include "projet_timer.h"

// "Création" des leds
CRGBW leds[NB_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];

// "Création" de l'écran LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {

  // Serial.begin(9600);

  // Réglages des boutons en PULLUP
  pinMode(BG, INPUT_PULLUP);
  pinMode(BD, INPUT_PULLUP);

  // Initialisation des Leds
  FastLED.addLeds<WS2812B, LED_DATA_PIN, RGB>(ledsRGB, getRGBWsize(NB_LEDS));
  // FastLED.setBrightness(50);

  FastLED.clear(true);

  // Initialisation de l'écran LCD
  lcd.init();
  lcd.backlight();

  // Petit bonjour
  show_title("Bonjour !");

  delay(1200);

  show_title(INACTIVE_TITLE);
  
}

void loop() {
  /*
  Différents modes :
  Mode inactif (arc en ciel)  : 0
  Mode Minuteur               : 1
  Mode Chronomètre            : 2
  Mode Pomodoro               : 3
*/
  static uint8_t mode = 0;
  static bool pause = false;
  
  static uint8_t potL;

  // Instant où un minuteur a été lancé (en ms)
  static uint32_t t0;
  // Durée d'un minuteur (constante)
  static uint32_t duration;

  // Instant où le bouton gauche a été pressé pour la dernière fois
  static uint32_t last_press_BG = 0;
  // Instant où le bouton gauche a été pressé pour la dernière fois
  static uint32_t last_press_BD = 0;

  // static uint8_t bg_color[4] = {0, 0, 0, 0};  // Pas utiles je crois, juste mettre la couleur par rapport aux constantes, ça fait moins de paramètres et ça fait la même chose

  // Serial.println(duration);  

  // show_time(potH, potM, potS);

  update_brightness(&potL);
  FastLED.setBrightness(potL);
  FastLED.show();

  
  if (!pause){
    not_paused(&mode, &pause, &last_press_BG, &last_press_BD, &duration, &t0);
  } else {
    paused(&mode, &pause, &last_press_BG, &last_press_BD, &t0, duration);
  }

}

void not_paused(uint8_t *md, bool *p, uint32_t *lpress_BG, uint32_t *lpress_BD, uint32_t *dur, uint32_t *t){

  static uint8_t fg_color[4] = {0, 0, 0, 0};  // Peut-être plutôt laisser seulement fg_color mais en le modifiant : on le transforme en tableau de tableau (avec chaque constante à l'indice de chaque mode)

  // S'execute quand le mode n'est pas mis en pause
  switch(*md){
    case 0: // mode Inactif
      mode_0(md, lpress_BG, lpress_BD, fg_color, dur, t);
      break;
    
    case 1: // minuteur
    case 3:
    case 4:
      mode_timer(md, p, lpress_BG, lpress_BD, fg_color, dur, t);
      break;
    
    case 2:
      mode_2(md, p, lpress_BG, lpress_BD, dur, *t);
      break;

  }
}

void paused(uint8_t *md, bool *p, uint32_t *lpress_BG, uint32_t *lpress_BD, uint32_t *t, uint32_t duration){
  // Créer une fonction paused

  switch(*md){ // En fonction du mode, si on appuie sur un certain bouton, on remet le mode en route. On remet aussi sa couleur d'arrière-plan
    case 1:
      *t = millis();
      if (BD_pressed(lpress_BD)){

        // change_background_color(*md);
        *p = false;

        show_title(SIMPLE_TIMER_TITLE);
      } else if (BG_pressed(lpress_BG)){
        *p = false;
        to_mode_0(md);
      }
      break;
    case 2:
      rainbow();
      if (BD_pressed(lpress_BD)){
        // change_background_color(*md);
        *p = false;

        *t = millis() - duration * 1000;

        show_title(CHRONOMETER_TITLE);
      } else if (BG_pressed(lpress_BG)){
        *p = false;
        to_mode_0(md);
      }
      break;
      
    case 3:

      *t = millis();
      if (BG_pressed(lpress_BG)){
        // change_background_color(*md);
        *p = false;

        show_title(POMODORO_WORK_TITLE);
      } else if (BD_pressed(lpress_BD)){
        *p = false;
        to_mode_0(md);
      }
      break;

    case 4:
      *t = millis();
      if (BG_pressed(lpress_BG)){
        // change_background_color(*md);
        *p = false;

        show_title(POMODORO_REST_TITLE);
      } else if (BD_pressed(lpress_BD)){
        *p = false;
        to_mode_0(md);
      }
      break;
  }
}

void to_pause(bool *p, uint32_t *dur, uint32_t *tl){ // Peut-être mettre une couleur pour la pause
  // Met le mode en pause
  /*
  tl :  correspond au temps restant du minuteur
        Si le mode est le mode chronomètre, il correspond au temps déjà passé et dur ne correspond à rien
  
  */
  uint8_t h;
  uint8_t m;
  uint8_t s;

  *p = true;

  // Quand on va quitter le mode pause, on va relancer un nouveau minuteur avec une durée égale au temps qu'il restait
  *dur = *tl;

  
  show_title("En pause...");

  convert_HMS(*tl, &h, &m, &s);
  show_time(h, m, s);

}


void to_mode_timer(uint8_t *md, uint8_t h, uint8_t m, uint8_t s, uint8_t n_md, uint8_t fg_col[4], uint32_t *dur, uint32_t *t){
  // Fait passer le mode dans un mode minuteur (simple : 1, pomodoro (travail) : 3, pomodoro (repos))

  *md = n_md;
  change_background_color(n_md);

  switch(n_md){
    // Si on passe en mode minuteur simple
    case 1:
      set_colors(fg_col, FG_COLOR_RED_SIMPLE_TIMER, FG_COLOR_GREEN_SIMPLE_TIMER, FG_COLOR_BLUE_SIMPLE_TIMER, FG_COLOR_WHITE_SIMPLE_TIMER);

      show_title(SIMPLE_TIMER_TITLE);

      *dur = get_duration(h, m, s);

      break;
    
    // Si on passe en mode Pomodoro (travail)
    case 3:
      set_colors(fg_col, FG_COLOR_RED_POMODORO_WORK, FG_COLOR_GREEN_POMODORO_WORK, FG_COLOR_BLUE_POMODORO_WORK, FG_COLOR_WHITE_POMODORO_WORK);

      show_title(POMODORO_WORK_TITLE);

      *dur = get_duration(0, 25, 0);

      delay(1000);    // Peut-être mettre le changement de couleur avant le délai, voire enlever le délai

      break;

    // Si on passe en mode pomodoro (repos)
    case 4:
      set_colors(fg_col, FG_COLOR_RED_POMODORO_REST, FG_COLOR_GREEN_POMODORO_REST, FG_COLOR_BLUE_POMODORO_REST, FG_COLOR_WHITE_POMODORO_REST);

      show_title(POMODORO_REST_TITLE);

      *dur = get_duration(0, 5, 0);

      delay(1000);

      break;
  }

  // *t = init_timer(*md, *dur, fg_col);
  change_background_color(*md);

  // renvoie t0 (moment où on lance le timer)
  *t = millis();

}

void mode_timer(uint8_t *md, bool *p, uint32_t *lpress_BG, uint32_t *lpress_BD, uint8_t fg_col[4], uint32_t *dur, uint32_t *t){
  // S'execute quand on est dans un mode minuteur (simple, pomodoro (travail) ou pomodoro (repos))
  
  // Temps restant d'un minuteur
  static uint32_t t_left;

  // Nombre de leds à allumer à gauche (barre de chargement)
  static uint8_t leds_left;
  // Nombre de leds déjà allumées
  static uint8_t nb_leds_on = 0;

  uint8_t h;
  uint8_t m;
  uint8_t s;

  // Temps restant
  t_left = *dur - ((millis() - *t) / 1000);

  // Si le minuteur est terminé
  if (t_left > 216000){ // Faire attention (peut-être mettre un > 216 000 parce que c'est un unsigned int (uint32_t))
    nb_leds_on = 0;
    switch(*md){
      case 1: // Mode minuteur simple
      case 4: // Mode pomodoro (repos)
        to_mode_0(md); // On repasse en mode inactif
        break;
      
      case 3: // Mode pomodoro (travail)
        to_mode_timer(md, 0, 5, 0, 4, fg_col, dur, t); // On passe en mode pomodoro (repos)
        break;


    }
    
  } else if (BG_pressed(lpress_BG)){ // Si on appuie sur le bouton gauche
    switch(*md){
      case 1: // Mode minuteur simple
        nb_leds_on = 0;
        to_mode_0(md);
        break;
      
      case 3: // Mode pomodoro (travail)
      case 4: // Mode pomodoro (repos)
        // Pause
        to_pause(p, dur, &t_left);
        break;


    }
  } else if (BD_pressed(lpress_BD)){ // Si on appuie sur le bouton droit

    switch(*md){
      case 1: // Mode minuteur simple
        // Pause
        to_pause(p, dur, &t_left);
        break;
      
      case 3: // Mode pomodoro (travail)
      case 4: // Mode pomodoro (repos)
        nb_leds_on = 0;
        to_mode_0(md);
        break;
    }
        
  } else {  // Sinon, on affiche le temps restant

      convert_HMS(t_left, &h, &m, &s);
      show_time(h, m, s);

    // Nombre de leds à allumer à gauche (barre de chargement)
    leds_left = map(*dur - t_left, 0, *dur, 0, NB_LEDS);

    if (leds_left > nb_leds_on){
      for (int i = nb_leds_on; i < leds_left; i++){
        leds[i] = CRGBW(fg_col[0], fg_col[1], fg_col[2], fg_col[3]);
      }
      nb_leds_on = leds_left;
      FastLED.show();
    }
  }
}

void to_mode_0(uint8_t *md){
  *md = 0;

  show_title(INACTIVE_TITLE);
}

void mode_0(uint8_t *md, uint32_t *lpress_BG, uint32_t *lpress_BD, uint8_t fg_col[4], uint32_t *dur, uint32_t *t){
  // S'exécute quand le mode est en mode inactif

  static uint8_t potH;    // à mettre dans not_paused si on s'en sert dans un autre mode
  static uint8_t potM;
  static uint8_t potS;

  update_HMS(&potH, &potM, &potS); // Peut-être directement mettre les variables dans mode_0 parce qu'on en a pas besoin pour les autres modes

  rainbow();

  show_time(potH, potM, potS);

  if (BG_pressed(lpress_BG)){
    // Pomodoro
    to_mode_timer(md, potH, potM, potS, 3, fg_col, dur, t);

  } else if (BD_pressed(lpress_BD)){
    if (duration_null(potH, potM, potS)){
      // Chronomètre
      to_mode_2(md, t);
    } else {
      // Minuteur
      to_mode_timer(md, potH, potM, potS, 1, fg_col, dur, t);
    }
  }

}

void to_mode_2(uint8_t *md, uint32_t *t){
  // Passe le mode en mode chronomètre
  *md = 2;
  *t = millis();

  show_title(CHRONOMETER_TITLE);


}

void mode_2(uint8_t *md, bool *p, uint32_t *lpress_BG, uint32_t *lpress_BD, uint32_t *dur, uint32_t t0){
  // Peut-être plutôt une variable statique pour la durée passé parce qu'elle est pas censé être modifiée en dehors de cette fonction

  static uint8_t i;
  static uint32_t last_color_change = 0;

  uint8_t h;
  uint8_t m;
  uint8_t s;
  
  rainbow();

  if ((millis() - last_color_change) > 100){
    last_color_change = millis();
    i = (i + 1) % NB_LEDS;
    leds[i] = CRGBW(255, 255, 255, 255);


  }

  // Temps passé depuis le début
  *dur = (millis() - t0) / 1000;

  if (BG_pressed(lpress_BG)){ // Si on appuie sur le bouton gauche
    to_mode_0(md);


  } else if (BD_pressed(lpress_BD)){ // Si on appuie sur le bouton droit
    // Pause
    to_pause(p, 0, dur);

        
  } else {  // Sinon, on affiche le temps restant
    convert_HMS(*dur, &h, &m, &s);
    show_time(h, m, s);
  }



}

void rainbow(){
  uint16_t beatA = beatsin16(30, 0, 255);
  uint16_t beatB = beatsin16(20, 0, 255);
  fill_rainbow(leds, NB_LEDS, (beatA + beatB) / 2, 8);

  FastLED.show();
}

void fill_color(uint8_t r, uint8_t g, uint8_t b, uint8_t w){
  uint8_t i;
  for (i = 0; i < NB_LEDS; i++){
      leds[i] = CRGBW(r, g, b, w);
  }

  FastLED.show();
}

void change_background_color(uint8_t mode){   // Peut-être demander les couleurs directement en paramètre plutôt que le mode (ça évite de mettre un switch), après c'est juste un fill color donc peut-être le mettre directement sans fonction
  // Change la couleur d'arrière plan des leds en fonction du mode
  switch (mode){ 
    case 1:
      fill_color(BG_COLOR_RED_SIMPLE_TIMER, BG_COLOR_GREEN_SIMPLE_TIMER, BG_COLOR_BLUE_SIMPLE_TIMER, BG_COLOR_WHITE_SIMPLE_TIMER);
      break;
    
    case 3:
      fill_color(BG_COLOR_RED_POMODORO_WORK, BG_COLOR_GREEN_POMODORO_WORK, BG_COLOR_BLUE_POMODORO_WORK, BG_COLOR_WHITE_POMODORO_WORK);
      break;
    
    case 4:
      fill_color(BG_COLOR_RED_POMODORO_REST, BG_COLOR_GREEN_POMODORO_REST, BG_COLOR_BLUE_POMODORO_REST, BG_COLOR_WHITE_POMODORO_REST);
      break;

  }
}

void set_colors(uint8_t colors[4], uint8_t r, uint8_t g, uint8_t b, uint8_t w){
  // Change la valeur des composantes d'un tableau de couleurs
  colors[0] = r;
  colors[1] = g;
  colors[2] = b;
  colors[3] = w;
}

void update_brightness(uint8_t *l){
  // Actualise la valeur de la luminosité (ne fait pas le changement)
  unsigned short br = map(analogRead(POT_L), 32, 1023, 0, 255);

  if (br > 255){
    br = 0;
  }

  *l = br;
  
}

void update_HMS(uint8_t *h, uint8_t *m, uint8_t *s){
  // Met à jour les valeurs des potentiomètres
  *h = map(analogRead(POT_H), 0, 1023, 0, 59);
  *m = map(analogRead(POT_M), 0, 1023, 0, 59);
  *s = map(analogRead(POT_S), 0, 1023, 0, 59);
}

void convert_HMS(uint32_t tl, uint8_t *h, uint8_t *m, uint8_t *s){
  // Convertit un nombre de secondes en heures, minutes et secondes
  *h = tl / 3600;
  tl = tl % 3600;

  *m = tl / 60;
  tl = tl % 60;

  *s = tl;

}

void show_time(uint8_t h, uint8_t m, uint8_t s){
  // Affiche le temps sous la forme hh:mm:ss en bas de l'écran LCD
  lcd.setCursor(4, 1);

  lcd.print(normalize_number(h));
  lcd.print(":");
  lcd.print(normalize_number(m));
  lcd.print(":");
  lcd.print(normalize_number(s));

}

void show_title(String title){
  // Affiche le nom du mode en haut de l'écran LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(title);
}

uint32_t get_duration(uint8_t h, uint8_t m, uint8_t s){
  return (uint32_t) h * 3600 + m * 60 + s;
}

String normalize_number(uint8_t number){
  // Transforme les nombres pour qu'ils aient une longueur de 2 caractères
  String result;
  if (String(number).length() < 2) {
    result += "0";
  }
  result += String(number);

  return result;
}

bool BG_pressed(uint32_t *lpress_BG){
  // Renvoie true si le bouton est pressé et qu'on a attendu assez de temps
  bool result = (digitalRead(BG) && (millis() - *lpress_BG >= BUTTON_DELAY));
  if (result){
    *lpress_BG = millis();
  }
  
  return result;
}

bool BD_pressed(uint32_t *lpress_BD){
  bool result = (digitalRead(BD) && (millis() - *lpress_BD >= BUTTON_DELAY));
  if (result){
    *lpress_BD = millis();
  }
  
  return result;
}

bool duration_null(uint8_t h, uint8_t m, uint8_t s){
  return (h == 0 && m == 0 && s == 0);
}
