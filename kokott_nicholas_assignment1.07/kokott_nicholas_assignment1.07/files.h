#ifndef FILES_H
# define FILES_H

class pokemon{
    public:
      int id;
      char identifier[30];
      int species_id;
      int height;
      int weight;
      int base_experience;
      int order;
      int is_default;
};

class move{
    public:
    int id;
    char identifier[30];
    int generation_id;
    int type_id;
    int power;
    int pp;
    int accuracy;
    int priority;
    int target_id;
    int damage_class_id;
    int effect_id;
    int effect_chance;
    int contest_type_id;
    int contest_effect_id;
    int super_contest_effect_id;
};

class pokemon_move{
    public:
    int pokemon_id;
    int version_group_id;
    int move_id;
    int pokemon_move_method;
    int level;
    int order;
};

class pokemon_specie{
    public:
    int id;
    char identifier[30];
    int generation_id;
    int evolves_from_species_id;
    int evolution_chain_id;
    int color_id;
    int shape_id;
    int habitat_id;
    int gender_rate;
    int capture_rate;
    int base_happiness;
    int is_baby;
    int hatch_counter;
    int has_gender_differences;
    int growth_rate_id;
    int forms_switchable;
    int is_legendary;
    int is_mythical;
    int order;
    int conquest_order;
};

class experience{
    public:
    int growth_rate_id;
    int level;
    int experience;
};

class type_name{
    public:
    int type_id;
    int local_language_id;
    char name[30];
};

class pokemon_stat{
    public:
    int pokemon_id;
    int stat_id;
    int base_stat;
    int effort;
};

class sta{
    public:
    int id;
    int damage_class_id;
    char identifier[30];
    int is_battle_only;
    int game_index;
};

class pokemon_type{
    public:
    int pokemon_id;
    int type_id;
    int slot;
};

void pokemons(int power, int obtained);
void moves(int power, int obtained);
void pokemon_moves(int power, int obtained);
void pokemon_species(int power, int obtained);
void experiences(int power, int obtained);
void type_names(int power, int obtained);
void pokemon_stats(int power, int obtained);
void stats(int power, int obtained);
void pokemon_types(int power, int obtained);

#endif

