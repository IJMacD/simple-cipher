typedef struct {
  char links[26];
  unsigned char rotor_1;
  unsigned char rotor_2;
  unsigned char rotor_3;
} cipher_conf;

char *encode(const char *, cipher_conf *);
void repl(cipher_conf);
