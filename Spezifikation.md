# Spezifikation
Spiel: 2048

## Spielfeld
- Quadrat aus Feldern (mind. 4x4, vllt auch größer, wenn das Spiel im testen noch gut funktionieren)
- 1 Feld ist eine Zahl (mind. 2) oder leer

## Spielprinzip
- Anfang: Spielfeld ist leer bis auf 2 Felder, die eine 2 enthalten
- Verschieben der Felder mit den Pfeiltasten
  - Es bewegen sich immer alle Felder (die sich bewegen können)
  - Felder bewegen sich soweit sie können
- Gleiche Zahlen, die aufeinander geschoben werden, werden zu einer Zahl addiert
  - Dies gibt Punkte
- Jedes Verschieben lässt eine neue Zahl erscheinen
- Spiel ist verloren / zu Ende, wenn das Spielfeld voll ist und keine weiteren Verschiebungen möglich sind

## Spielziel
- Feld mit 2048 erreichen (vllt. auch höher, je nach Spielfeldgröße)
- Optional: "Endless Mode", man kann auch weiter spielen nachdem 2048 erreicht wurde
  - Ziel: Möglichst viele Punkte


# Programmaufbau

## Datenstrukturen
``` c
struct Game {
  uint32_t board[]; // 0 ist leer, ansonsten Zahlenwert des Feldes
  uint8_t board_size; // Seitenlänge des Spielfelds

  uint32_t score;

  SDLRenderer* renderer;
}
```

## Funktionen
### Spielfluss
``` c
  void add_new_number(Game *game);

  // These functions apply the corresponding move to the game.
  // Their return value indicates if any number was actually moved.
  int move_up(Game *game);
  int move_down(Game *game);
  int move_left(Game *game);
  int move_right(Game *game);

  int has_valid_moves(Game *game);
  int has_won(Game *game);
```

### Rendering
``` c
  void display_board(Game *game);
  void display_square(uint32_t val, int x, int y);
  void display_interface(Game *game);
  void display_endscreen(Game *game);
```

## Programmablauf

1. Initialisierung
   1. Fenster erstellen
   2. Spielfeld erstellen
2. Game-Loop
   1. Noch gültige Verschiebungen?
    -> Nein => Spiel beenden
   2. Gewonnen && nicht endlos?
    -> Ja => Spiel beenden
   3. Auf Nutzer-Input warten (nur Pfeiltasten)
   4. Entsprechende Verschiebung ausführen
   5. Falls Verschiebung etwas verändert hat, neue Zahl hinzufügen
3. End-Screen darstellen -> Score, Neues Spiel starten
