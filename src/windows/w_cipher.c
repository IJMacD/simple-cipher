#include <Windows.h>

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

void print_spaces(void *hConsole, COORD pos, char length) {
  DWORD dwBytesWritten = 0;
  char spaces[REPL_MAX_LENGTH];
  memset(spaces, ' ', length);
  WriteConsoleOutputCharacterA(hConsole, spaces, length, pos, &dwBytesWritten);
}

void clear_message(void *hConsole) {
  COORD pos = { .X = MESSAGE_X, .Y = MESSAGE_Y };
  print_spaces(hConsole, pos, REPL_MAX_LENGTH);
}

void clear_output(void *hConsole) {
  COORD pos = { .X = OUTPUT_X, .Y = OUTPUT_Y };
  print_spaces(hConsole, pos, REPL_MAX_LENGTH);
}

void print_instructions(void *hConsole) {
  COORD pos;
  DWORD dwBytesWritten = 0;

  pos.X = MESSAGE_X - 1;
  pos.Y = MESSAGE_Y - 1;
  const char instructions[] = "Type your message:";
  WriteConsoleOutputCharacterA(hConsole, instructions, sizeof(instructions), pos, &dwBytesWritten);

  pos.X = OUTPUT_X - 1;
  pos.Y = OUTPUT_Y - 1;
  const char cipher[] = "Ciphertext:";
  WriteConsoleOutputCharacterA(hConsole, cipher, sizeof(cipher), pos, &dwBytesWritten);

  pos.X = LINKS_X - 1;
  pos.Y = LINKS_Y - 1;
  const char links[] = "Links:";
  WriteConsoleOutputCharacterA(hConsole, links, sizeof(links), pos, &dwBytesWritten);

  pos.X = LINKS_X;
  pos.Y = LINKS_Y + 1;
  const char link_graphic[] = "||||||||||||||||||||||||||";
  WriteConsoleOutputCharacterA(hConsole, link_graphic, sizeof(link_graphic), pos, &dwBytesWritten);

  pos.X = ROTORS_X - 1;
  pos.Y = ROTORS_Y - 1;
  const char rotors[] = "Rotors:";
  WriteConsoleOutputCharacterA(hConsole, rotors, sizeof(rotors), pos, &dwBytesWritten);
  pos.X = ROTORS_X + 7;
  pos.Y = ROTORS_Y - 1;
  const wchar_t rotors_box_top[] = L"\u2554\u2550\u2557";
  WriteConsoleOutputCharacterW(hConsole, rotors_box_top, LENGTH(rotors_box_top) - 1, pos, &dwBytesWritten);
  pos.X = ROTORS_X + 7;
  pos.Y = ROTORS_Y + 3;
  const wchar_t rotors_box_bottom[] = L"\u255a\u2550\u255d";
  WriteConsoleOutputCharacterW(hConsole, rotors_box_bottom, LENGTH(rotors_box_bottom) - 1, pos, &dwBytesWritten);
}

void print_message(void *hConsole, const char *msg, int length) {
  COORD pos;
  DWORD dwBytesWritten = 0;
  pos.X = MESSAGE_X;
  pos.Y = MESSAGE_Y;
  WriteConsoleOutputCharacterA(hConsole, msg, length, pos, &dwBytesWritten);
}

void print_output(void *hConsole, const char *msg, int length) {
  COORD pos;
  DWORD dwBytesWritten = 0;
  pos.X = OUTPUT_X;
  pos.Y = OUTPUT_Y;
  WriteConsoleOutputCharacterA(hConsole, msg, length, pos, &dwBytesWritten);
}

void print_links(void *hConsole, const char *links) {
  COORD pos;
  DWORD dwBytesWritten = 0;
  const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  pos.X = LINKS_X;
  pos.Y = LINKS_Y;
  WriteConsoleOutputCharacterA(hConsole, alphabet, 26, pos, &dwBytesWritten);
  pos.X = LINKS_X;
  pos.Y = LINKS_Y + 2;
  WriteConsoleOutputCharacterA(hConsole, links, 26, pos, &dwBytesWritten);
}

void print_rotor(void *hConsole, COORD pos, rotor r) {
  DWORD dwBytesWritten = 0;
  wchar_t buffer[16 + 1 + 2] = { 0 };
  int i, offset = 0;
  for(i = -8; i <= 8; i++) {
    if (i == -1 || i == 1) {
      buffer[offset++] = 0x2551;  // Vertical double box
    } else if(i < 0) {
      buffer[offset++] = r.wiring[(r.position + r.ring_setting + i + 1 + 26) % 26];
    } else if(i > 0) {
      buffer[offset++] = r.wiring[(r.position + r.ring_setting + i - 1 + 26) % 26];
    } else {
      buffer[offset++] = r.wiring[(r.position + r.ring_setting) % 26];
    }
  }
  WriteConsoleOutputCharacterW(hConsole, buffer, LENGTH(buffer) - 1, pos, &dwBytesWritten);
}

void print_rotors(void *hConsole, const cipher_conf cipher) {
  COORD pos;
  pos.X = ROTORS_X;
  pos.Y = ROTORS_Y;
  print_rotor(hConsole, pos, *cipher.rotor_1);
  pos.X = ROTORS_X;
  pos.Y = ROTORS_Y + 1;
  print_rotor(hConsole, pos, *cipher.rotor_2);
  pos.X = ROTORS_X;
  pos.Y = ROTORS_Y + 2;
  print_rotor(hConsole, pos, *cipher.rotor_3);
}

void process(void *hConsole, const char *buffer, int length, cipher_conf cipher) {

  print_message(hConsole, buffer, length);

  char *enc = encode(buffer, &cipher);

  print_output(hConsole, enc, strlen(enc));

  print_rotors(hConsole, cipher);
}

void repl (cipher_conf cipher) {
  void *hConsole =  CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
  COORD size = {.X = 80, .Y = 28 };
  SetConsoleScreenBufferSize(hConsole, size);
  SMALL_RECT rect = { .Top = 0, .Left = 0, .Bottom = size.Y, .Right = size.X };
  SetConsoleWindowInfo(hConsole, 1, &rect);
  SetConsoleActiveScreenBuffer(hConsole);
  COORD cursor = {MESSAGE_X, MESSAGE_Y};
  SetConsoleCursorPosition(hConsole, cursor);

  print_instructions(hConsole);

  print_links(hConsole, cipher.plugboard);
  print_rotors(hConsole, cipher);

  char buffer[REPL_MAX_LENGTH] = { 0 };
  unsigned int offset = 0;

  char c;
  while((c = _getch()) != 27){  // ESC key
    if (c == 0x08) { // Backspace
      offset = --offset % REPL_MAX_LENGTH;
      buffer[offset] = '\0';

      clear_message(hConsole);
      clear_output(hConsole);
    }
    else if (isprint(c)) {
      buffer[offset] = c;
      offset = (++offset % REPL_MAX_LENGTH);
    }

    cursor.X = MESSAGE_X + offset;
    SetConsoleCursorPosition(hConsole, cursor);

    process(hConsole, buffer, offset, cipher);
  }
}
