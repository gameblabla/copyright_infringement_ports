#define PMD_VECTOR 0x60
#define PCM86_VECTOR 0x65

void call_pmd_vector();
int check_pmd();
int check_pcm86();
void CallMusicDriver(unsigned char ah_value, unsigned int *segment, unsigned int *offset);
void pmd_mstart(const char *music_data, unsigned int music_data_size);
int initialize_p86();
int load_p86_file(const char *filename);
void pmd_play_FM_sound_effect(unsigned char sound_effect_index);
void pmd_play_pcm_sound_effect(unsigned char sound_effect_index, unsigned short frequency, char pan, unsigned char volume);
