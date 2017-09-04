#include <Windows.h>

#include "../cipher.h"

#define INSTRUCTIONS_X 0
#define INSTRUCTIONS_Y 0
#define MESSAGE_X 1
#define MESSAGE_Y 1
#define OUTPUT_X  1
#define OUTPUT_Y  4
#define LINKS_X  1
#define LINKS_Y  6
#define ROTORS_X  1
#define ROTORS_Y  8

void clear_message(void *hConsole) {
  COORD pos;
  DWORD dwBytesWritten = 0;
  const char spaces[] = "                                     ";
  pos.X = MESSAGE_X;
  pos.Y = MESSAGE_Y;
  WriteConsoleOutputCharacterA(hConsole, spaces, sizeof(spaces), pos, &dwBytesWritten);
}

void clear_output(void *hConsole) {
  COORD pos;
  DWORD dwBytesWritten = 0;
  const char spaces[] = "                                     ";
  pos.X = OUTPUT_X;
  pos.Y = OUTPUT_Y;
  WriteConsoleOutputCharacterA(hConsole, spaces, sizeof(spaces), pos, &dwBytesWritten);
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
  pos.Y = LINKS_Y + 1;
  WriteConsoleOutputCharacterA(hConsole, links, 26, pos, &dwBytesWritten);
}

void print_rotors(void *hConsole, const cipher_conf cipher) {
  COORD pos;
  DWORD dwBytesWritten = 0;
  char buffer[255] = { 0 };
  pos.X = ROTORS_X;
  pos.Y = ROTORS_Y;
  buffer[0] = cipher.rotor_1 + 'A';
  WriteConsoleOutputCharacterA(hConsole, buffer, 1, pos, &dwBytesWritten);
  pos.X = ROTORS_X;
  pos.Y = ROTORS_Y + 1;
  buffer[0] = cipher.rotor_2 + 'A';
  WriteConsoleOutputCharacterA(hConsole, buffer, 1, pos, &dwBytesWritten);
  pos.X = ROTORS_X;
  pos.Y = ROTORS_Y + 2;
  buffer[0] = cipher.rotor_3 + 'A';
  WriteConsoleOutputCharacterA(hConsole, buffer, 1, pos, &dwBytesWritten);
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
  COORD new_pos = {79, 0};
  SetConsoleCursorPosition(hConsole, new_pos);

  COORD pos;
  DWORD dwBytesWritten = 0;
  pos.X = INSTRUCTIONS_X;
  pos.Y = INSTRUCTIONS_Y;
  const char instructions[] = "Type your message:";
  WriteConsoleOutputCharacterA(hConsole, instructions, sizeof(instructions), pos, &dwBytesWritten);

  print_links(hConsole, cipher.links);

  char buffer[255] = { 0 };
  unsigned int offset = 0;

  while(1){
    if(_kbhit()) {
      char c = _getch();
      if (c == 0x08) { // Backspace
        offset = --offset % 255;
        buffer[offset] = '\0';

        clear_message(hConsole);
        clear_output(hConsole);
      }
      else if (isprint(c)) {
        buffer[offset] = c;
        offset = (++offset % 255);
      }

      process(hConsole, buffer, offset, cipher);
    }

    Sleep(10);
  }
}
