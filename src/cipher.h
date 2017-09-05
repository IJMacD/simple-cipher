typedef struct {
  char wiring[26];
  char ring_setting;
  char position;
  char notch;
} rotor;

typedef struct {
  char plugboard[26];
  rotor *rotor_1;
  rotor *rotor_2;
  rotor *rotor_3;
  char reflector[26];
} cipher_conf;

char *encode(const char *, cipher_conf *);
void repl(cipher_conf);
