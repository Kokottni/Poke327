#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <string>
#include <fstream>

#include "files.h"

pokemon pokemans[1093];
move movess[845];
pokemon_move pokemon_movess[528239];
pokemon_specie species[899];
experience exps[601];
type_name tnames[194];
pokemon_stat pstats[6553];
sta regstats[8];
pokemon_type ptypes[1676];

void pokemons(int power, int obtained){
    std::ifstream file;
    file.open("/share/cs327/pokedex/pokedex/data/csv/pokemon.csv");
    if(!file){
        std::string home = getenv("HOME");
        home += "/poke327/pokedex/pokedex/data/csv/pokemon.csv";
        file.open(home);
        if(!file){
            printf("File does not exist allegedly");
            return;
        }
    }
    std::string s;
    getline(file, s);
    int i;
    //char line[800];
    for(i = 0; i < 1092; ++i){
        pokemans[i].id = 0;
        while(file.peek() != ','){
            pokemans[i].id = pokemans[i].id * 10 + (file.get() - 48);
            obtained = 1;
            ++power;
        }
        if(obtained != 1){
            pokemans[i].id = -1;
        }
        obtained = power = 0;
        file.get();
        int j = 0;
        while(file.peek() != ','){
            pokemans[i].identifier[j] = file.get();
            ++j;
        }
        file.get();
        pokemans[i].species_id = 0;
        while(file.peek() != ','){
            pokemans[i].species_id = pokemans[i].species_id * 10 + (file.get() - 48);
            obtained = 1;
            ++power;
        }
        if(obtained != 1){
            pokemans[i].species_id = -1;
        }
        file.get();
        obtained = power = 0;
        pokemans[i].height = 0;
        while(file.peek() != ','){
            pokemans[i].height = pokemans[i].height * 10 + (file.get() - 48);
            obtained = 1;
            ++power;
        }
        if(obtained != 1){
            pokemans[i].height = -1;
        }
        file.get();
        obtained = power = 0;
        pokemans[i].weight = 0;
        while(file.peek() != ','){
            pokemans[i].weight = pokemans[i].weight * 10 + (file.get() - 48);
            obtained = 1;
            ++power;
        }
        if(obtained != 1){
            pokemans[i].weight = -1;
        }
        file.get();
        obtained = power = 0;
        pokemans[i].base_experience = 0;
        while(file.peek() != ','){
            pokemans[i].base_experience = pokemans[i].base_experience * 10 + (file.get() - 48);
            obtained = 1;
            ++power;
        }
        if(obtained != 1){
            pokemans[i].base_experience = -1;
        }
        file.get();
        obtained = power = 0;
        pokemans[i].order = 0;
        if(pokemans[i].id < 1000){
            pokemans[i].is_default = 1;
        }else{
            pokemans[i].is_default = 0;
        }
        getline(file, s);
        obtained = power = 0;
        pokemans[i].is_default = 1;
        printf("%d %s %d %d %d %d %d %d\n", pokemans[i].id, pokemans[i].identifier, pokemans[i].species_id, pokemans[i].height, pokemans[i].weight, pokemans[i].base_experience, pokemans[i].order, pokemans[i].is_default);
    }
    // for(i = 0; i < 1093; ++i){
    //     printf("%d %s %d %d %d %d %d %d\n", pokemans[i].id, pokemans[i].identifier, pokemans[i].species_id, pokemans[i].height, pokemans[i].weight, pokemans[i].base_experience, pokemans[i].order, pokemans[i].is_default);
    // }
    file.close();
}

void moves(int power, int obtained){
    std::ifstream file;
    file.open("/share/cs327/pokedex/pokedex/data/csv/moves.csv");
    if(!file){
        std::string home = getenv("HOME");
        home += "/poke327/pokedex/pokedex/data/csv/moves.csv";
        file.open(home);
        if(!file){
            printf("File does not exist allegedly");
            return;
        }
    }
    std::string s;
    getline(file, s);
    int i;
    for(i = 0; i < 844; ++i){
        movess[i].id = 0;
        while(file.peek() != ','){
            movess[i].id = movess[i].id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            movess[i].id = -1;
        }
        file.get();
        power = obtained = 0;
        int j = 0;
        while(file.peek() != ','){
            movess[i].identifier[j] += file.get();
            ++j;
        }
        file.get();
        movess[i].generation_id = 0;
        while(file.peek() != ','){
            movess[i].generation_id = movess[i].generation_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            movess[i].generation_id = -1;
        }
        file.get();
        power = obtained = 0;
        movess[i].type_id = 0;
        while(file.peek() != ','){
            movess[i].type_id = movess[i].type_id * 10 + (file.get() - 48);
            obtained = 1;
        }
        if(obtained != 1){
            movess[i].type_id = -1;
        }
        file.get();
        power = obtained = 0;
        movess[i].power = 0;
        while(file.peek() != ','){
            movess[i].power = movess[i].power * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            movess[i].power = -1;
        }
        file.get();
        power = obtained = 0;
        movess[i].pp = 0;
        while(file.peek() != ','){
            movess[i].pp = movess[i].pp * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            movess[i].pp = -1;
        }
        file.get();
        power = obtained = 0;
        movess[i].accuracy = 0;
        while(file.peek() != ','){
            movess[i].accuracy = movess[i].accuracy * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            movess[i].accuracy = -1;
        }
        file.get();
        power = obtained = 0;
        movess[i].priority = 0;
        while(file.peek() != ','){
            movess[i].priority = movess[i].priority * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            movess[i].priority = -1;
        }
        file.get();
        power = obtained = 0;
        movess[i].target_id = 0;
        while(file.peek() != ','){
            movess[i].target_id = movess[i].target_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            movess[i].target_id = -1;
        }
        file.get();
        power = obtained = 0;
        movess[i].damage_class_id = 0;
        while(file.peek() != ','){
            movess[i].damage_class_id = movess[i].damage_class_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            movess[i].damage_class_id = -1;
        }
        file.get();
        power = obtained = 0;
        movess[i].effect_id = 0;
        while(file.peek() != ','){
            movess[i].effect_id = movess[i].effect_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            movess[i].effect_id = -1;
        }
        file.get();
        power = obtained = 0;
        movess[i].effect_chance = 0;
        while(file.peek() != ','){
            movess[i].effect_chance = movess[i].effect_chance * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            movess[i].effect_chance = -1;
        }
        file.get();
        power = obtained = 0;
        movess[i].contest_type_id = 0;
        while(file.peek() != ','){
            movess[i].contest_type_id = movess[i].contest_type_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            movess[i].contest_type_id = -1;
        }
        file.get();
        power = obtained = 0;
        movess[i].contest_effect_id = 0;
        while(file.peek() != ','){
            movess[i].contest_effect_id = movess[i].contest_effect_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            movess[i].contest_effect_id = -1;
        }
        file.get();
        power = obtained = 0;
        movess[i].super_contest_effect_id = 0;
        while(file.peek() != '\n'){
            movess[i].super_contest_effect_id = movess[i].super_contest_effect_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            movess[i].super_contest_effect_id = -1;
        }
        file.get();
        power = obtained = 0;
        printf("%d %s %d %d %d %d %d %d %d %d %d %d %d %d %d\n", movess[i].id, movess[i].identifier, movess[i].generation_id, movess[i].type_id, movess[i].power, movess[i].pp, movess[i].accuracy, movess[i].priority, movess[i].target_id, movess[i].damage_class_id, movess[i].effect_id, movess[i].effect_chance, movess[i].contest_type_id, movess[i].contest_effect_id, movess[i].super_contest_effect_id);
    }
    // for(i = 0; i < 845; ++i){
    //     printf("%d %s %d %d %d %d %d %d %d %d %d %d %d %d %d\n", movess[i].id, movess[i].identifier, movess[i].generation_id, movess[i].type_id, movess[i].power, movess[i].pp, movess[i].accuracy, movess[i].priority, movess[i].target_id, movess[i].damage_class_id, movess[i].effect_id, movess[i].effect_chance, movess[i].contest_type_id, movess[i].contest_effect_id, movess[i].super_contest_effect_id);
    // }
    file.close();
}

void pokemon_moves(int power, int obtained){
    std::ifstream file;
    file.open("/share/cs327/pokedex/pokedex/data/csv/pokemon_moves.csv");
    if(!file){
        std::string home = getenv("HOME");
        home += "/poke327/pokedex/pokedex/data/csv/pokemon_moves.csv";
        file.open(home);
        if(!file){
            printf("File does not exist allegedly");
            return;
        }
    }
    std::string s;
    getline(file, s);
    int i;
    for(i = 0; i < 528238; ++i){
        pokemon_movess[i].pokemon_id = 0;
        while(file.peek() != ','){
            pokemon_movess[i].pokemon_id = pokemon_movess[i].pokemon_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            pokemon_movess[i].pokemon_id = -1;
        }
        file.get();
        obtained = power = 0;
        pokemon_movess[i].version_group_id = 0;
        while(file.peek() != ','){
            pokemon_movess[i].version_group_id = pokemon_movess[i].version_group_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            pokemon_movess[i].version_group_id = -1;
        }
        file.get();
        obtained = power = 0;
        pokemon_movess[i].move_id = 0;
        while(file.peek() != ','){
            pokemon_movess[i].move_id = pokemon_movess[i].move_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            pokemon_movess[i].move_id = -1;
        }
        file.get();
        obtained = power = 0;
        pokemon_movess[i].pokemon_move_method = 0;
        while(file.peek() != ','){
            pokemon_movess[i].pokemon_move_method = pokemon_movess[i].pokemon_move_method * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            pokemon_movess[i].pokemon_move_method = -1;
        }
        file.get();
        obtained = power = 0;
        pokemon_movess[i].level = 0;
        while(file.peek() != ','){
            pokemon_movess[i].level = pokemon_movess[i].level * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            pokemon_movess[i].level = -1;
        }
        file.get();
        obtained = power = 0;
        pokemon_movess[i].order = 0;
        while(file.peek() != '\n'){
            pokemon_movess[i].order = pokemon_movess[i].order * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            pokemon_movess[i].order = -1;
        }
        file.get();
        obtained = power = 0;
        printf("%d %d %d %d %d %d \n", pokemon_movess[i].pokemon_id, pokemon_movess[i].version_group_id, pokemon_movess[i].move_id, pokemon_movess[i].pokemon_move_method, pokemon_movess[i].level, pokemon_movess[i].order);
    }
    // for(i = 0; i < 528239; ++i){
    //     printf("%d %d %d %d %d %d \n", pokemon_movess[i].pokemon_id, pokemon_movess[i].version_group_id, pokemon_movess[i].move_id, pokemon_movess[i].pokemon_move_method, pokemon_movess[i].level, pokemon_movess[i].order);
    // }
    file.close();
}

void pokemon_species(int power, int obtained){
    std::ifstream file;
    file.open("/share/cs327/pokedex/pokedex/data/csv/pokemon_species.csv");
    if(!file){
        std::string home = getenv("HOME");
        home += "/poke327/pokedex/pokedex/data/csv/pokemon_species.csv";
        file.open(home);
        if(!file){
            printf("File does not exist allegedly");
            return;
        }
    }
    std::string s;
    getline(file, s);
    int i;
    for(i = 0; i < 898; ++i){
        species[i].id = 0;
        while(file.peek() != ','){
            species[i].id = species[i].id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            species[i].id = -1;
        }
        file.get();
        obtained = power = 0;
        int j = 0;
        while(file.peek() != ','){
            species[i].identifier[j] = file.get();
            ++j;
        }
        file.get();
        species[i].generation_id = 0;
        while(file.peek() != ','){
            species[i].generation_id = species[i].generation_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            species[i].generation_id = -1;
        }
        file.get();
        obtained = power = 0;
        species[i].evolves_from_species_id = 0;
        while(file.peek() != ','){
            species[i].evolves_from_species_id = species[i].evolves_from_species_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            species[i].evolves_from_species_id = -1;
        }
        file.get();
        obtained = power = 0;
        species[i].evolution_chain_id = 0;
        while(file.peek() != ','){
            species[i].evolution_chain_id = species[i].evolution_chain_id * 10 + (file.get() - 48);
            obtained = 1;
        }
        if(obtained != 1){
            species[i].evolution_chain_id = -1;
        }
        file.get();
        obtained = power = 0;
        species[i].color_id = 0;
        while(file.peek() != ','){
            species[i].color_id = species[i].color_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            species[i].color_id = -1;
        }
        file.get();
        obtained = power = 0;
        species[i].shape_id = 0;
        while(file.peek() != ','){
            species[i].shape_id = species[i].shape_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            species[i].shape_id = -1;
        }
        file.get();
        obtained = power = 0;
        species[i].habitat_id = 0;
        while(file.peek() != ','){
            species[i].habitat_id = species[i].habitat_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            species[i].habitat_id = -1;
        }
        file.get();
        obtained = power = 0;
        species[i].gender_rate = 0;
        while(file.peek() != ','){
            species[i].gender_rate = species[i].gender_rate * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            species[i].gender_rate = -1;
        }
        file.get();
        obtained = power = 0;
        species[i].capture_rate = 0;
        while(file.peek() != ','){
            species[i].capture_rate = species[i].capture_rate * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            species[i].capture_rate = -1;
        }
        file.get();
        obtained = power = 0;
        species[i].base_happiness = 0;
        while(file.peek() != ','){
            species[i].base_happiness = species[i].base_happiness * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            species[i].base_happiness = -1;
        }
        file.get();
        obtained = power = 0;
        species[i].is_baby = 0;
        while(file.peek() != ','){
            species[i].is_baby = species[i].is_baby * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            species[i].is_baby = -1;
        }
        file.get();
        obtained = power = 0;
        species[i].hatch_counter = 0;
        while(file.peek() != ','){
            species[i].hatch_counter = species[i].hatch_counter * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            species[i].hatch_counter = -1;
        }
        file.get();
        obtained = power = 0;
        species[i].has_gender_differences = 0;
        while(file.peek() != ','){
            species[i].has_gender_differences = species[i].has_gender_differences * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            species[i].has_gender_differences = -1;
        }
        file.get();
        obtained = power = 0;
        species[i].growth_rate_id = 0;
        while(file.peek() != ','){
            species[i].growth_rate_id = species[i].growth_rate_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            species[i].growth_rate_id = -1;
        }
        file.get();
        obtained = power = 0;
        species[i].forms_switchable = 0;
        while(file.peek() != ','){
            species[i].forms_switchable = species[i].forms_switchable * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            species[i].forms_switchable = -1;
        }
        file.get();
        obtained = power = 0;
        species[i].is_legendary = 0;
        while(file.peek() != ','){
            species[i].is_legendary = species[i].is_legendary * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            species[i].is_legendary = -1;
        }
        file.get();
        obtained = power = 0;
        species[i].is_mythical = 0;
        while(file.peek() != ','){
            species[i].is_mythical = species[i].is_mythical * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            species[i].is_mythical = -1;
        }
        file.get();
        obtained = power = 0;
        species[i].order = 0;
        while(file.peek() != ','){
            species[i].order = species[i].order * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            species[i].order = -1;
        }
        file.get();
        obtained = power = 0;
        species[i].conquest_order = 0;
        while(file.peek() != '\n'){
            species[i].conquest_order = species[i].conquest_order * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            species[i].conquest_order = -1;
        }
        file.get();
        obtained = power = 0;
        printf("%d %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", species[i].id, species[i].identifier, species[i].generation_id, species[i].evolves_from_species_id, species[i].evolution_chain_id, species[i].color_id, species[i].shape_id, species[i].habitat_id, species[i].gender_rate, species[i].capture_rate, species[i].base_happiness, species[i].is_baby, species[i].hatch_counter, species[i].has_gender_differences, species[i].growth_rate_id, species[i].forms_switchable, species[i].is_legendary, species[i].is_mythical, species[i].order, species[i].conquest_order);
    }
    // for(i = 0; i < 899; ++i){
    //     printf("%d %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", species[i].id, species[i].identifier, species[i].generation_id, species[i].evolves_from_species_id, species[i].evolution_chain_id, species[i].color_id, species[i].shape_id, species[i].habitat_id, species[i].gender_rate, species[i].capture_rate, species[i].base_happiness, species[i].is_baby, species[i].hatch_counter, species[i].has_gender_differences, species[i].growth_rate_id, species[i].forms_switchable, species[i].is_legendary, species[i].is_mythical, species[i].order, species[i].conquest_order);
    // }
    file.close();
}

void experiences(int power, int obtained){
    std::ifstream file;
    file.open("/share/cs327/pokedex/pokedex/data/csv/experience.csv");
    if(!file){
        std::string home = getenv("HOME");
        home += "/poke327/pokedex/pokedex/data/csv/experience.csv";
        file.open(home);
        if(!file){
            printf("File does not exist allegedly");
            return;
        }
    }
    std::string s;
    getline(file, s);
    int i;
    for(i = 0; i < 600; ++i){
        exps[i].growth_rate_id = 0;
        while(file.peek() != ','){
            exps[i].growth_rate_id = exps[i].growth_rate_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            exps[i].growth_rate_id = -1;
        }
        file.get();
        obtained = power = 0;
        exps[i].level = 0;
        while(file.peek() != ','){
            exps[i].level = exps[i].level * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            exps[i].level = -1;
        }
        file.get();
        obtained = power = 0;
        exps[i].experience = 0;
        while(file.peek() != '\n'){
            exps[i].experience = exps[i].experience * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            exps[i].experience = -1;
        }
        file.get();
        obtained = power = 0; 
        printf("%d %d %d\n", exps[i].growth_rate_id, exps[i].level, exps[i].experience);       
    }
    // for(i = 0; i < 601; ++i){
    //     printf("%d %d %d\n", exps[i].growth_rate_id, exps[i].level, exps[i].experience);
    // }
    file.close();
}

void type_names(int power, int obtained){
    std::ifstream file;
    file.open("/share/cs327/pokedex/pokedex/data/csv/type_names.csv");
    if(!file){
        std::string home = getenv("HOME");
        home += "/poke327/pokedex/pokedex/data/csv/type_names.csv";
        file.open(home);
        if(!file){
            printf("File does not exist allegedly");
            return;
        }
    }
    std::string s;
    getline(file, s);
    int i;
    for(i = 0; i < 194; ++i){
        tnames[i].type_id = 0;
        while(file.peek() != ','){
            tnames[i].type_id = tnames[i].type_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            tnames[i].type_id = -1;
        }
        file.get();
        obtained = power = 0;
        tnames[i].local_language_id = 0;
        while(file.peek() != ','){
            tnames[i].local_language_id = tnames[i].local_language_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            tnames[i].local_language_id = -1;
        }
        file.get();
        obtained = power = 0;
        int j = 0;
        while(file.peek() != '\n'){
            tnames[i].name[j] = file.get();
            ++j;
        }
        file.get();
        printf("%d %d %s\n", tnames[i].type_id, tnames[i].local_language_id, tnames[i].name);
    }
    file.close();
    // for(i = 0; i < 194; ++i){
    //     printf("%d %d %s\n", tnames[i].type_id, tnames[i].local_language_id, tnames[i].name);
    // }
}

void pokemon_stats(int power, int obtained){
    std::ifstream file;
    file.open("/share/cs327/pokedex/pokedex/data/csv/pokemon_stats.csv");
    if(!file){
        std::string home = getenv("HOME");
        home += "/poke327/pokedex/pokedex/data/csv/pokemon_stats.csv";
        file.open(home);
        if(!file){
            printf("File does not exist allegedly");
            return;
        }
    }
    std::string s;
    getline(file, s);
    int i;
    for(i = 0; i < 6552; ++i){
        pstats[i].pokemon_id = 0;
        while(file.peek() != ','){
            pstats[i].pokemon_id = pstats[i].pokemon_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            pstats[i].pokemon_id = -1;
        }
        file.get();
        obtained = power = 0;
        pstats[i].stat_id = 0;
        while(file.peek() != ','){
            pstats[i].stat_id = pstats[i].stat_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            pstats[i].stat_id = -1;
        }
        file.get();
        obtained = power = 0;
        pstats[i].base_stat = 0;
        while(file.peek() != ','){
            pstats[i].base_stat = pstats[i].base_stat * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            pstats[i].base_stat = -1;
        }
        file.get();
        obtained = power = 0;
        pstats[i].effort = 0;
        while(file.peek() != '\n'){
            pstats[i].effort = pstats[i].effort * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            pstats[i].effort = -1;
        }
        file.get();
        obtained = power = 0;
        printf("%d %d %d %d\n", pstats[i].pokemon_id, pstats[i].stat_id, pstats[i].base_stat, pstats[i].effort);
    }
    // for(i = 0; i < 6553; ++i){
    //     printf("%d %d %d %d\n", pstats[i].pokemon_id, pstats[i].stat_id, pstats[i].base_stat, pstats[i].effort);
    // }
    file.close();
}

void stats(int power, int obtained){
    std::ifstream file;
    file.open("/share/cs327/pokedex/pokedex/data/csv/stats.csv");
    if(!file){
        std::string home = getenv("HOME");
        home += "/poke327/pokedex/pokedex/data/csv/stats.csv";
        file.open(home);
        if(!file){
            printf("File does not exist allegedly");
            return;
        }
    }
    std::string s;
    getline(file, s);
    int i;
    for(i = 0; i < 8; ++i){
        regstats[i].id = (file.get() - 48) * pow(10, power);
        regstats[i].damage_class_id = 0;
        while(file.peek() != ','){
            regstats[i].damage_class_id = regstats[i].damage_class_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            regstats[i].damage_class_id = -1;
        }
        file.get();
        obtained = power = 0;
        int j = 0;
        while(file.peek() != ','){
            regstats[i].identifier[j] = file.get();
            ++j;
        }
        file.get();
        regstats[i].is_battle_only = 0;
        while(file.peek() != ','){
            regstats[i].is_battle_only = regstats[i].is_battle_only * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            regstats[i].is_battle_only = -1;
        }
        file.get();
        obtained = power = 0;
        regstats[i].game_index = 0;
        while(file.peek() != '\n'){
            regstats[i].game_index = regstats[i].game_index * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            regstats[i].game_index = -1;
        }
        file.get();
        obtained = power = 0;
        printf("%d %d %s %d %d\n", regstats[i].id, regstats[i].damage_class_id, regstats[i].identifier, regstats[i].is_battle_only, regstats[i].game_index);
    }
    // for(i = 0; i < 8; ++i){
    //     printf("%d %d %s %d %d\n", regstats[i].id, regstats[i].damage_class_id, regstats[i].identifier, regstats[i].is_battle_only, regstats[i].game_index);
    // }
    file.close();
}

void pokemon_types(int power, int obtained){
    std::ifstream file;
    file.open("/share/cs327/pokedex/pokedex/data/csv/pokemon_types.csv");
    if(!file){
        std::string home = getenv("HOME");
        home += "/poke327/pokedex/pokedex/data/csv/pokemon_types.csv";
        file.open(home);
        if(!file){
            printf("File does not exist allegedly");
            return;
        }
    }
    std::string s;
    getline(file, s);
    int i;
    for(i = 0; i < 1675; ++i){
        ptypes[i].pokemon_id = 0;
        while(file.peek() != ','){
            ptypes[i].pokemon_id = ptypes[i].pokemon_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            ptypes[i].pokemon_id = -1;
        }
        file.get();
        obtained = power = 0;
        ptypes[i].type_id = 0;
        while(file.peek() != ','){
            ptypes[i].type_id = ptypes[i].type_id * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            ptypes[i].type_id = -1;
        }
        file.get();
        obtained = power = 0;
        ptypes[i].slot = 0;
        while(file.peek() != '\n'){
            ptypes[i].slot = ptypes[i].slot * 10 + (file.get() - 48);
            ++power;
            obtained = 1;
        }
        if(obtained != 1){
            ptypes[i].slot = -1;
        }
        file.get();
        obtained = power = 0;
        printf("%d %d %d\n", ptypes[i].pokemon_id, ptypes[i].type_id, ptypes[i].slot);
    }
    file.close();
    // for(i = 0; i < 1676; ++i){
    //     printf("%d %d %d\n", ptypes[i].pokemon_id, ptypes[i].type_id, ptypes[i].slot);
    // }
}
