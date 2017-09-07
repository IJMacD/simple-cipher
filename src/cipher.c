#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cipher.h"

// Source: http://enigma.louisedade.co.uk/jssource.html
rotor rotor_I = {
  .wiring = "EKMFLGDQVZNTOWYHXUSPAIBRCJ",
  .notch = 17, // R
  .position = 0,
  .ring_setting = 0,
};
rotor rotor_II = {
  .wiring = "AJDKSIRUXBLHWTMCQGZNPYFVOE",
  .notch = 5, // F
  .position = 0,
  .ring_setting = 0,
};
rotor rotor_III = {
  .wiring = "BDFHJLCPRTXVZNYEIWGAKMUSQO",
  .notch = 22, // W
  .position = 0,
  .ring_setting = 0,
};
rotor rotor_IV = {
  .wiring = "ESOVPZJAYQUIRHXLNFTGKDCMWB",
  .notch = 11, // K
  .position = 0,
  .ring_setting = 0,
};
rotor rotor_V = {
  .wiring = "VZBRGITYUPSDNHLXAWMJQOFECK",
  .notch = 0, // A
  .position = 0,
  .ring_setting = 0,
};
rotor rotor_VI = {
  .wiring = "JPGVOUMFYQBENHZRDKASXLICTW",
  .notch = 0, // A & M
  .position = 0,
  .ring_setting = 0,
};
rotor rotor_VII = {
  .wiring = "NZJHGRCXMYSWBOUFAIVLPEKQDT",
  .notch = 0, // A & M
  .position = 0,
  .ring_setting = 0,
};
rotor rotor_VIII = {
  .wiring = "FKQHTLXOCBJSPDZRAMEWNIUYGV",
  .notch = 0, // A & M
  .position = 0,
  .ring_setting = 0,
};
cipher_conf enigma = {
  .plugboard = "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
  .reflector = "YRUHQSLDPXNGOKMIEBFZCWVJAT",
};

char valid_index(char i) {
  return (i + 26) % 26;
}

char valid_letter(char c) {
  return 'A' + valid_index(c - 'A');
}

char swap_letter(const char c, const char *map) {
  return map[c - 'A'];
}

char unswap_letter(const char c, const char *map) {
  int i = 0;
  while(map[i] != c) {
    i++;
  }
  return valid_letter(i + 'A');
}
char apply_rotor_f(const char c, rotor r) {
  return valid_letter(
      swap_letter(
          valid_letter(c - r.ring_setting + r.position),
          r.wiring
      ) + r.ring_setting - r.position
  );
}
char apply_rotor_r(const char c, rotor r) {
  return valid_letter(
      unswap_letter(
          valid_letter(c - r.ring_setting + r.position),
          r.wiring
      ) + r.ring_setting - r.position
  );
}
// char apply_rotor(const char c, rotor r) {
//   return swap_letter(
//           valid_letter(c - r.ring_setting + r.position),
//           r.wiring
//   );
// }

void rotate_rotors(cipher_conf *cipher) {
  if(++cipher->rotor_3.position >= cipher->rotor_3.notch) {

    if (++cipher->rotor_2.position >= cipher->rotor_2.notch) {

      cipher->rotor_1.position++;
      cipher->rotor_1.position = cipher->rotor_1.position % 26;
    }

    cipher->rotor_2.position = cipher->rotor_2.position % 26;
  }

  cipher->rotor_3.position = cipher->rotor_3.position % 26;
}

char *encode (const char *msg, cipher_conf *cipher) {
  char *out = malloc(strlen(msg) + 1);

  int i;
  for(i = 0; i < strlen(msg); i++) {

    if(isalpha(msg[i])) {
      rotate_rotors(cipher);

      char c = toupper(msg[i]);
      c = swap_letter(c, cipher->plugboard);

      c = apply_rotor_f(c, cipher->rotor_3);
      c = apply_rotor_f(c, cipher->rotor_2);
      c = apply_rotor_f(c, cipher->rotor_1);

      c = swap_letter(c, cipher->reflector);

      c = apply_rotor_r(c, cipher->rotor_1);
      c = apply_rotor_r(c, cipher->rotor_2);
      c = apply_rotor_r(c, cipher->rotor_3);

      c = unswap_letter(c, cipher->plugboard);
      out[i] = c;

    } else {
      out[i] = msg[i];
    }
  }
  out[i] = '\0';

  return out;
}

void create_cipher(cipher_conf *dest, const cipher_conf src, const rotor r1, const rotor r2, const rotor r3) {
  memcpy(dest->plugboard, src.plugboard, 26);
  memcpy(dest->reflector, src.reflector, 26);

  memcpy(dest->rotor_1.wiring, r1.wiring, 26);
  dest->rotor_1.notch = r1.notch;
  dest->rotor_1.ring_setting = r1.ring_setting;
  dest->rotor_1.position = r1.position;

  memcpy(dest->rotor_2.wiring, r2.wiring, 26);
  dest->rotor_2.notch = r2.notch;
  dest->rotor_2.ring_setting = r2.ring_setting;
  dest->rotor_2.position = r2.position;

  memcpy(dest->rotor_3.wiring, r3.wiring, 26);
  dest->rotor_3.notch = r3.notch;
  dest->rotor_3.ring_setting = r3.ring_setting;
  dest->rotor_3.position = r3.position;
}

void shuffle_alphabet(char buffer[26]) {
  int i = 26;
  while (i--) {
    char r = rand() % 26;
    char tmp = buffer[r];
    buffer[r] = buffer[i];
    buffer[i] = tmp;
  }
}

char find_letter(char buffer[26], char c) {
  int i = 26;
  while(i-- && buffer[i] != c);
  return i;
}

char find_nth_space(char buffer[26], char n) {
  int i = 0, c = 0;
  while(c < n) {
    if (buffer[i++] == ' ') c++;
  }
  while(buffer[i] != ' ') i++;
  return i;
}

void random_plugboard(char buffer[26]) {
  int i = 26;
  while (i--) {
    buffer[i] = ' ';
  }
  i = 26;
  int used = 0;
  while (i--) {
    if(buffer[i] == ' ') {
      int spaces = 25 - used;
      int r = rand() % spaces;
      int n = find_nth_space(buffer, r);
      buffer[i] = n + 'A';
      buffer[n] = i + 'A';
      used += 2;
    }
  }
}

int validate_plugboard(char buffer[26]) {
  int i = 26;
  while (i--) {
    char c = buffer[i];
    char n = i + 'A';
    char j = c - 'A';
    char d = buffer[j];
    if(d != n) return 0;
  }
  return 1;
}

int main(int argc, char *argv[]) {

  cipher_conf my_cipher;

  create_cipher(&my_cipher, enigma, rotor_I, rotor_II, rotor_III);

  // random_plugboard(my_cipher.plugboard);
  // printf("%s\n", my_cipher.plugboard);

  if(!validate_plugboard(my_cipher.plugboard)) {
    fprintf(stderr, "Error: Invalid plugboard. Letters must be paired up.\n");
    exit(-1);
  }

  if (argc > 1 && !strcmp(argv[1], "-r")) {
    repl(my_cipher);
  } else {

    const char *msg = "Hello World large apple cakes";
    char *msg2 = encode(msg, &my_cipher);

    printf("%s\n", msg2);

    // Reset
    enigma.rotor_1.position = 0;
    enigma.rotor_2.position = 0;
    enigma.rotor_3.position = 0;

    printf("%s\n", encode(msg2, &my_cipher));
  }
}
