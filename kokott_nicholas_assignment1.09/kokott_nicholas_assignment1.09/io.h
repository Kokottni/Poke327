#ifndef IO_H
# define IO_H

class Character;
class Pokemon;
typedef int16_t pair_t[2];

void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(void);
void io_handle_input(pair_t dest);
void io_queue_message(const char *format, ...);
void io_battle(Character *aggressor, Character *defender);
void io_encounter_pokemon(void);
void io_start(void);
int io_battle_sequence(Character *npc, Pokemon *wild);

#endif
