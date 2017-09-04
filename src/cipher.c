#include <stdio.h>
#include <stdlib.h>

#include "cipher.h"

char apply_rotor(const char c, const unsigned char rotor) {
  return ((c - 'A') + rotor) % 26 + 'A';
}

char swap_letter(const char c, const char *map) {
  return map[c - 'A'];
}

void rotate_rotors(cipher_conf *cipher) {
  if(++cipher->rotor_1 >= 26) {
    cipher->rotor_1 = 0;

    if (++cipher->rotor_2 >= 26) {
      cipher->rotor_2 = 0;

      if(++cipher->rotor_3 >= 26) {
        cipher->rotor_3 = 0;
      }
    }
  }
}

char *encode (const char *msg, cipher_conf *cipher) {
  char *out = malloc(strlen(msg) + 1);

  int i;
  for(i = 0; i < strlen(msg); i++) {
    out[i] = toupper(msg[i]);

    if(isalpha(out[i])) {
      out[i] = apply_rotor(out[i], cipher->rotor_1);
      out[i] = apply_rotor(out[i], cipher->rotor_2);
      out[i] = apply_rotor(out[i], cipher->rotor_3);
      out[i] = swap_letter(out[i], cipher->links);

      rotate_rotors(cipher);
    }
  }
  out[i] = '\0';

  return out;
}

int main(int argc, char *argv[]) {
  cipher_conf cipher = {
    .links = "NOPQRSTUVWXYZABCDEFGHIJKLM",
    .rotor_1 = 1,
    .rotor_2 = 2,
    .rotor_3 = 3
  };

  repl(cipher);
  // const char *msg = "Hello World";

  // printf("%s\n", encode(msg, cipher));
}
