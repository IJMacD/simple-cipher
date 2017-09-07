#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <curses.h>
#include <string.h>

#include "../cipher.h"

#define LENGTH(x) sizeof(x) / sizeof(x[0])

#define MESSAGE_X 1
#define MESSAGE_Y 1
#define OUTPUT_X  1
#define OUTPUT_Y  4
#define LINKS_X  1
#define LINKS_Y  7
#define ROTORS_X  1
#define ROTORS_Y  12
#define REPL_MAX_LENGTH 80

void print_spaces(int y, int x, char length) {
  char spaces[length + 1];
  memset(spaces, ' ', length);
  spaces[length] = '\0';
  mvaddstr(y, x, spaces);
}

void clear_message() {
  print_spaces(MESSAGE_Y, MESSAGE_X, REPL_MAX_LENGTH);
}

void clear_output() {
  print_spaces(OUTPUT_Y, OUTPUT_X, REPL_MAX_LENGTH);
}

void print_instructions() {
  mvaddstr(MESSAGE_Y - 1, MESSAGE_X - 1, "Type your message:");

  mvaddstr(OUTPUT_Y - 1, OUTPUT_X - 1, "Ciphertext:");

  mvaddstr(LINKS_Y - 1, LINKS_X - 1, "Links:");

  mvaddstr(ROTORS_Y - 1, ROTORS_X - 1, "Rotors:");
  mvaddstr(ROTORS_Y - 1, ROTORS_X + 7, "___");
}

void print_message(const char *msg, int length) {
  mvaddstr(MESSAGE_Y, MESSAGE_X, msg);
}

void print_output(const char *msg, int length) {
  char buffer[REPL_MAX_LENGTH + 1] = { 0 };
  strncpy(buffer, msg, length);
  mvaddstr(OUTPUT_Y, OUTPUT_X, buffer);
}

void print_links(const char *links, char in_c, char out_c) {
  const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  mvaddstr(LINKS_Y, LINKS_X, alphabet);

  int i;
  char buffer[26 + 1] = { 0 };
  for (i = 0; i < 26; i++) {
    buffer[i] = (alphabet[i] == in_c) ? '|' : ((alphabet[i] == out_c) ? '|' : ' '); // 0x2191 = ↑, 0x2193 = ↓
  }
  mvaddstr(LINKS_Y + 1, LINKS_X, buffer);

  strncpy(buffer, links, 26);
  mvaddstr(LINKS_Y + 2, LINKS_X, buffer);
}

void print_rotor(int y, int x, rotor r) {
  char buffer[16 + 1 + 2] = { 0 };
  int i, offset = 0;
  for(i = -8; i <= 8; i++) {
    if (i == -1 || i == 1) {
      buffer[offset++] = '|'; // 0x2551;  // Vertical double box
    } else if(i < 0) {
      buffer[offset++] = r.wiring[(r.position + r.ring_setting + i + 1 + 26) % 26];
    } else if(i > 0) {
      buffer[offset++] = r.wiring[(r.position + r.ring_setting + i - 1 + 26) % 26];
    } else {
      buffer[offset++] = r.wiring[(r.position + r.ring_setting) % 26];
    }
  }
  mvaddstr(y, x, buffer);
}

void print_rotors(const cipher_conf cipher) {
  print_rotor(ROTORS_Y, ROTORS_X, cipher.rotor_1);
  print_rotor(ROTORS_Y + 1, ROTORS_X, cipher.rotor_2);
  print_rotor(ROTORS_Y + 2, ROTORS_X, cipher.rotor_3);
}

void process(const char *msg, int length, cipher_conf cipher) {

  print_message(msg, length);

  char *enc = encode(msg, &cipher);

  print_output(enc, strlen(enc));

  print_links(cipher.plugboard, toupper(msg[length - 1]), enc[length - 1]);

  print_rotors(cipher);
}

void repl (cipher_conf cipher) {
  WINDOW * mainwin;

  /*  Initialize ncurses  */

  if ( (mainwin = initscr()) == NULL ) {
      fprintf(stderr, "Error initialising ncurses.\n");
      exit(EXIT_FAILURE);
  }

  noecho();                  /*  Turn off key echoing                 */
  keypad(mainwin, TRUE);     /*  Enable the keypad for non-char keys  */

  print_instructions();

  print_links(cipher.plugboard, 0, 0);
  print_rotors(cipher);

  refresh();

  char buffer[REPL_MAX_LENGTH] = { 0 };
  int offset = 0;

  char c;
  while((c = getch()) != 27){  // ESC key

    if (c == 0x07) { // Backspace
      if(--offset < 0) {
        offset = 0;
      }
      buffer[offset] = '\0';

      clear_message();
      clear_output();
    }
    else if (isprint(c)) {
      buffer[offset++] = c;
      buffer[offset > REPL_MAX_LENGTH ? REPL_MAX_LENGTH - 1 : offset] = '\0';
      offset = (offset % REPL_MAX_LENGTH);
    }

//     cursor.X = MESSAGE_X + offset;
//     SetConsoleCursorPosition(hConsole, cursor);

    process(buffer, offset, cipher);

    refresh();
  }


  /*  Clean up after ourselves  */

  delwin(mainwin);
  endwin();
  refresh();

  exit(EXIT_SUCCESS);

}
