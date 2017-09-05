#include <stdio.h>
#include <stdlib.h>

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

char valid_letter(char c) {
  return 'A' + (c - 'A' + 26 + 26) % 26;
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
  if(++cipher->rotor_3->position >= cipher->rotor_3->notch) {

    if (++cipher->rotor_2->position >= cipher->rotor_2->notch) {

      cipher->rotor_1->position++;
      cipher->rotor_1->position = cipher->rotor_1->position % 26;
    }

    cipher->rotor_2->position = cipher->rotor_2->position % 26;
  }

  cipher->rotor_3->position = cipher->rotor_3->position % 26;
}

char *encode (const char *msg, cipher_conf *cipher) {
  char *out = malloc(strlen(msg) + 1);

  int i;
  for(i = 0; i < strlen(msg); i++) {

    if(isalpha(msg[i])) {
      rotate_rotors(cipher);

      char c = toupper(msg[i]);
      c = swap_letter(c, cipher->plugboard);

      c = apply_rotor_f(c, *cipher->rotor_3);
      c = apply_rotor_f(c, *cipher->rotor_2);
      c = apply_rotor_f(c, *cipher->rotor_1);

      c = swap_letter(c, cipher->reflector);

      c = apply_rotor_r(c, *cipher->rotor_1);
      c = apply_rotor_r(c, *cipher->rotor_2);
      c = apply_rotor_r(c, *cipher->rotor_3);

      c = swap_letter(c, cipher->plugboard);
      out[i] = c;

    } else {
      out[i] = msg[i];
    }
  }
  out[i] = '\0';

  return out;
}

int main(int argc, char *argv[]) {

  cipher_conf enigma = {
    .plugboard = "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
    .reflector = "YRUHQSLDPXNGOKMIEBFZCWVJAT",
    .rotor_1 = &rotor_I,
    .rotor_2 = &rotor_II,
    .rotor_3 = &rotor_III,
  };


  // repl(enigma);
  const char *msg = "Hello World large apple cakes";
  char *msg2 = encode(msg, &enigma);

  printf("%s\n", msg2);

  // Reset
  enigma.rotor_1->position = 0;
  enigma.rotor_2->position = 0;
  enigma.rotor_3->position = 0;

  printf("%s\n", encode(msg2, &enigma));
}
