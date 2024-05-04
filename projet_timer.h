#ifndef PROJET_TIMER_H
#define PROJET_TIMER_H

#define NB_LEDS 8
#define LED_DATA_PIN 8

// Analog Luminosité
#define POT_L A0

// Pin Analog Heures
#define POT_H A1
// Pin Analog Minutes
#define POT_M A2
// Pin Analog Secondes
#define POT_S A3

// Bouton gauche ("Pomodoro")
#define BG 3
// Bouton droit ("Minuteur")
#define BD 2

// Temps à attendre avant de pouvoir réappuyer sur le bouton (en ms)
#define BUTTON_DELAY 1000

// Couleurs pour chaque mode :
// Mode minuteur simple :
// Couleurs des leds en fond pour le mode minuteur simple
#define BG_COLOR_RED_SIMPLE_TIMER     0
#define BG_COLOR_GREEN_SIMPLE_TIMER   0
#define BG_COLOR_BLUE_SIMPLE_TIMER    255
#define BG_COLOR_WHITE_SIMPLE_TIMER   0

// Couleurs des leds en avant pour le mode minuteur simple
#define FG_COLOR_RED_SIMPLE_TIMER     0
#define FG_COLOR_GREEN_SIMPLE_TIMER   40
#define FG_COLOR_BLUE_SIMPLE_TIMER    40
#define FG_COLOR_WHITE_SIMPLE_TIMER   0

// Mode pomodoro (travail) :
// Couleurs des leds en fond pour le mode pomodoro (travail)
#define BG_COLOR_RED_POMODORO_WORK    255
#define BG_COLOR_GREEN_POMODORO_WORK  70
#define BG_COLOR_BLUE_POMODORO_WORK   0
#define BG_COLOR_WHITE_POMODORO_WORK  0

// Couleurs des leds en avant pour le mode pomodoro (travail)
#define FG_COLOR_RED_POMODORO_WORK    255
#define FG_COLOR_GREEN_POMODORO_WORK  170
#define FG_COLOR_BLUE_POMODORO_WORK   0
#define FG_COLOR_WHITE_POMODORO_WORK  0

// Mode pomodoro (repos) :
// Couleurs des leds en fond pour le mode pomodoro (repos)
#define BG_COLOR_RED_POMODORO_REST    255
#define BG_COLOR_GREEN_POMODORO_REST  10
#define BG_COLOR_BLUE_POMODORO_REST   10
#define BG_COLOR_WHITE_POMODORO_REST  0

// Couleurs des leds en avant pour le mode pomodoro (repos)
#define FG_COLOR_RED_POMODORO_REST    255
#define FG_COLOR_GREEN_POMODORO_REST  70
#define FG_COLOR_BLUE_POMODORO_REST   0
#define FG_COLOR_WHITE_POMODORO_REST  0

// Noms des modes à afficher en haut de l'écran LCD
#define INACTIVE_TITLE "Reglages :"
#define SIMPLE_TIMER_TITLE "Minuteur :"
#define CHRONOMETER_TITLE "Chronometre :"
#define POMODORO_WORK_TITLE "Travail 25min :"
#define POMODORO_REST_TITLE "Repos 5 min : "


// S'exécute quand le mode n'est pas en pause
void not_paused(uint8_t *md, bool *p, uint32_t *lpress_BG, uint32_t *lpress_BD, uint32_t *dur, uint32_t *t);
// S'exécute quand le mode est en pause
void paused(uint8_t *md, bool *p, uint32_t *lpress_BG, uint32_t *lpress_BD, uint32_t *t, uint32_t duration);

// Met le mode en pause
void to_pause(bool *p, uint32_t *dur, uint32_t *tl);

// Fait passer le mode dans un mode minuteur (simple : 1, pomodoro (travail) : 3 ou pomodoro (repos))
void to_mode_timer(uint8_t *md, uint8_t h, uint8_t m, uint8_t s, uint8_t n_md, uint8_t fg_col[4], uint32_t *dur, uint32_t *t);

// S'execute quand on est dans un mode minuteur (simple, pomodoro (travail) ou pomodoro (repos))
void mode_timer(uint8_t *md, bool *p, uint32_t *lpress_BG, uint32_t *lpress_BD, uint8_t fg_col[4], uint32_t *dur, uint32_t *t);

// Fait passer le mode en inactif 
void to_mode_0(uint8_t *md);
// S'execute quand le mode est le mode inactif
void mode_0(uint8_t *md, uint32_t *lpress_BG, uint32_t *lpress_BD, uint8_t fg_col[4], uint32_t *dur, uint32_t *t);

// Fait passer le mode en mode chronomètre
void to_mode_2(uint8_t *md, uint32_t *t);
// S'exécute quand on est dans le mode chronomètre
void mode_2(uint8_t *md, bool *p, uint32_t *lpress_BG, uint32_t *lpress_BD, uint32_t *dur, uint32_t t0);

// Met un effet arc-en-ciel sur les leds
void rainbow();

// Met toutes les leds de la même couleur (r, g, b, w)
void fill_color(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
// Change la couleur de l'arrière plan en fonction du mode
void change_background_color(uint8_t mode);

// Change la valeur des composantes d'un tableau de couleurs
void set_colors(uint8_t colors[4], uint8_t r, uint8_t g, uint8_t b, uint8_t w);
// Met à jour la valeur de la luminosité entrée par le potentiomètre
void update_brightness(uint8_t *l);
// Met à jour les valeurs heures, minutes, secondes entrées par les potentiomètres
void update_HMS(uint8_t *h, uint8_t *m, uint8_t *s);

// Convertit un nombre de secondes en heures, minutes et secondes
void convert_HMS(uint32_t tl, uint8_t *h, uint8_t *m, uint8_t *s);
// Affiche une durée sous la forme hh:mm:ss
void show_time(uint8_t h, uint8_t m, uint8_t s);
// Affiche le nom du mode en haut de l'écran LCD
void show_title(String title);
// Renvoie la durée en secondes d'une durée en hh:mm:ss
uint32_t get_duration(uint8_t h, uint8_t m, uint8_t s);
// Ajoute des '0' à gauche pour que le nombre ait une longueur de 2
String normalize_number(uint8_t number);

// Renvoie true si le bouton gauche est pressé et que le dernier appuie date d'il y a au moins BUTTON_DELAY ms
bool BG_pressed(uint32_t *lpress_BG);
// Renvoie true si le bouton droit est pressé et que le dernier appuie date d'il y a au moins BUTTON_DELAY ms
bool BD_pressed(uint32_t *lpress_BD);
// renvoie true si les valeurs heures, minutes et secondes sont nulles
bool duration_null(uint8_t h, uint8_t m, uint8_t s);

#endif