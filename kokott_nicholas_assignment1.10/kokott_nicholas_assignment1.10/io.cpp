#include <unistd.h>
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "io.h"
#include "character.h"
#include "poke327.h"
#include "pokemon.h"
#include "db_parse.h"

typedef struct io_message {
  /* Will print " --more-- " at end of line when another message follows. *
   * Leave 10 extra spaces for that.                                      */
  char msg[71];
  struct io_message *next;
} io_message_t;

static io_message_t *io_head, *io_tail;

void io_init_terminal(void)
{
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

void io_reset_terminal(void)
{
  endwin();

  while (io_head) {
    io_tail = io_head;
    io_head = io_head->next;
    free(io_tail);
  }
  io_tail = NULL;
}

void io_queue_message(const char *format, ...)
{
  io_message_t *tmp;
  va_list ap;

  if (!(tmp = (io_message_t *) malloc(sizeof (*tmp)))) {
    perror("malloc");
    exit(1);
  }

  tmp->next = NULL;

  va_start(ap, format);

  vsnprintf(tmp->msg, sizeof (tmp->msg), format, ap);

  va_end(ap);

  if (!io_head) {
    io_head = io_tail = tmp;
  } else {
    io_tail->next = tmp;
    io_tail = tmp;
  }
}

static void io_print_message_queue(uint32_t y, uint32_t x)
{
  while (io_head) {
    io_tail = io_head;
    attron(COLOR_PAIR(COLOR_CYAN));
    mvprintw(y, x, "%-80s", io_head->msg);
    attroff(COLOR_PAIR(COLOR_CYAN));
    io_head = io_head->next;
    if (io_head) {
      attron(COLOR_PAIR(COLOR_CYAN));
      mvprintw(y, x + 70, "%10s", " --more-- ");
      attroff(COLOR_PAIR(COLOR_CYAN));
      refresh();
      getch();
    }
    free(io_tail);
  }
  io_tail = NULL;
}

/**************************************************************************
 * Compares trainer distances from the PC according to the rival distance *
 * map.  This gives the approximate distance that the PC must travel to   *
 * get to the trainer (doesn't account for crossing buildings).  This is  *
 * not the distance from the NPC to the PC unless the NPC is a rival.     *
 *                                                                        *
 * Not a bug.                                                             *
 **************************************************************************/
static int compare_trainer_distance(const void *v1, const void *v2)
{
  const Character *const *c1 = (const Character *const *) v1;
  const Character *const *c2 = (const Character *const *) v2;

  return (world.rival_dist[(*c1)->pos[dim_y]][(*c1)->pos[dim_x]] -
          world.rival_dist[(*c2)->pos[dim_y]][(*c2)->pos[dim_x]]);
}

static Character *io_nearest_visible_trainer()
{
  Character **c, *n;
  uint32_t x, y, count;

  c = (Character **) malloc(world.cur_map->num_trainers * sizeof (*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = world.cur_map->cmap[y][x];
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof (*c), compare_trainer_distance);

  n = c[0];

  free(c);

  return n;
}

void io_display()
{
  uint32_t y, x;
  Character *c;

  clear();
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.cur_map->cmap[y][x]) {
        mvaddch(y + 1, x, world.cur_map->cmap[y][x]->symbol);
      } else {
        switch (world.cur_map->map[y][x]) {
        case ter_boulder:
        case ter_mountain:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, '%');
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;
        case ter_tree:
        case ter_forest:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, '^');
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_path:
        case ter_exit:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, '#');
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;
        case ter_mart:
          attron(COLOR_PAIR(COLOR_BLUE));
          mvaddch(y + 1, x, 'M');
          attroff(COLOR_PAIR(COLOR_BLUE));
          break;
        case ter_center:
          attron(COLOR_PAIR(COLOR_RED));
          mvaddch(y + 1, x, 'C');
          attroff(COLOR_PAIR(COLOR_RED));
          break;
        case ter_gym:
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, 'G');
          attroff(COLOR_PAIR(COLOR_CYAN));
          break;
        case ter_grass:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, ':');
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_clearing:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, '.');
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        default:
 /* Use zero as an error symbol, since it stands out somewhat, and it's *
  * not otherwise used.                                                 */
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, '0');
          attroff(COLOR_PAIR(COLOR_CYAN)); 
       }
      }
    }
  }

  mvprintw(23, 1, "PC position is (%2d,%2d) on map %d%cx%d%c.",
           world.pc.pos[dim_x],
           world.pc.pos[dim_y],
           abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
           abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S');
  mvprintw(22, 1, "%d known %s.", world.cur_map->num_trainers,
           world.cur_map->num_trainers > 1 ? "trainers" : "trainer");
  mvprintw(22, 30, "Nearest visible trainer: ");
  if ((c = io_nearest_visible_trainer())) {
    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(22, 55, "%c at %d %c by %d %c.",
             c->symbol,
             abs(c->pos[dim_y] - world.pc.pos[dim_y]),
             ((c->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              'N' : 'S'),
             abs(c->pos[dim_x] - world.pc.pos[dim_x]),
             ((c->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              'W' : 'E'));
    attroff(COLOR_PAIR(COLOR_RED));
  } else {
    attron(COLOR_PAIR(COLOR_BLUE));
    mvprintw(22, 55, "NONE.");
    attroff(COLOR_PAIR(COLOR_BLUE));
  }

  io_print_message_queue(0, 0);

  refresh();
}

uint32_t io_teleport_pc(pair_t dest)
{
  /* Just for fun. And debugging.  Mostly debugging. */

  do {
    dest[dim_x] = rand_range(1, MAP_X - 2);
    dest[dim_y] = rand_range(1, MAP_Y - 2);
  } while (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]                  ||
           move_cost[char_pc][world.cur_map->map[dest[dim_y]]
                                                [dest[dim_x]]] == INT_MAX ||
           world.rival_dist[dest[dim_y]][dest[dim_x]] < 0);

  return 0;
}

static void io_scroll_trainer_list(char (*s)[40], uint32_t count)
{
  uint32_t offset;
  uint32_t i;

  offset = 0;

  while (1) {
    for (i = 0; i < 13; i++) {
      mvprintw(i + 6, 19, " %-40s ", s[i + offset]);
    }
    switch (getch()) {
    case KEY_UP:
      if (offset) {
        offset--;
      }
      break;
    case KEY_DOWN:
      if (offset < (count - 13)) {
        offset++;
      }
      break;
    case 27:
      return;
    }

  }
}

static void io_list_trainers_display(Npc **c,
                                     uint32_t count)
{
  uint32_t i;
  char (*s)[40]; /* pointer to array of 40 char */

  s = (char (*)[40]) malloc(count * sizeof (*s));

  mvprintw(3, 19, " %-40s ", "");
  /* Borrow the first element of our array for this string: */
  snprintf(s[0], 40, "You know of %d trainers:", count);
  mvprintw(4, 19, " %-40s ", s[0]);
  mvprintw(5, 19, " %-40s ", "");

  for (i = 0; i < count; i++) {
    snprintf(s[i], 40, "%16s %c: %2d %s by %2d %s",
             char_type_name[c[i]->ctype],
             c[i]->symbol,
             abs(c[i]->pos[dim_y] - world.pc.pos[dim_y]),
             ((c[i]->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              "North" : "South"),
             abs(c[i]->pos[dim_x] - world.pc.pos[dim_x]),
             ((c[i]->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              "West" : "East"));
    if (count <= 13) {
      /* Handle the non-scrolling case right here. *
       * Scrolling in another function.            */
      mvprintw(i + 6, 19, " %-40s ", s[i]);
    }
  }

  if (count <= 13) {
    mvprintw(count + 6, 19, " %-40s ", "");
    mvprintw(count + 7, 19, " %-40s ", "Hit escape to continue.");
    while (getch() != 27 /* escape */)
      ;
  } else {
    mvprintw(19, 19, " %-40s ", "");
    mvprintw(20, 19, " %-40s ",
             "Arrows to scroll, escape to continue.");
    io_scroll_trainer_list(s, count);
  }

  free(s);
}

static void io_list_trainers()
{
  Character **c;
  uint32_t x, y, count;

  c = (Character **) malloc(world.cur_map->num_trainers * sizeof (*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = world.cur_map->cmap[y][x];
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof (*c), compare_trainer_distance);

  /* Display it */
  io_list_trainers_display((Npc **)(c), count);
  free(c);

  /* And redraw the map */
  io_display();
}

void io_pokemart()
{
  int x, key = 0;
  while(key != 27){
    x = 0;
    mvprintw(x, 0, "Welcome to the Pokemart.  Items are available as listed below.");
    x++;
    mvprintw(x++, 2, "You currently have %d coins.", world.pc.player_bag.coin);
    mvprintw(x++, 2, " %-50s ", "");
    mvprintw(x++, 2, " %-50s ", "(1)Pokeballs, cost: 3 coins");
    mvprintw(x++, 2, " %-50s ", "");
    mvprintw(x++, 2, " %-50s ", "(2)Revives, cost: 5 coins");
    mvprintw(x++, 2, " %-50s ", "");
    mvprintw(x++, 2, " %-50s ","(3)Potions, cost: 2 coin");
    mvprintw(x++, 2, " %-50s ", "");
    mvprintw(x++, 2, " %-50s ", "To leave my store please hit escape");
    refresh();
    switch(key = getch()){
      case '1':
        if(world.pc.player_bag.coin - 3 > -1){
          world.pc.player_bag.pokeball++;
          world.pc.player_bag.coin = world.pc.player_bag.coin - 3;
        }else{
          mvprintw(x++, 0, "DO NOT TEASE ME WITH MONEY YOU DO NOT HAVE!!!!");
          refresh();
        }
        break;
      case '2':
        if(world.pc.player_bag.coin - 5 > -1){
          world.pc.player_bag.revive++;
          world.pc.player_bag.coin = world.pc.player_bag.coin - 5;
        }else{
          mvprintw(x++, 0, "DO NOT TEASE ME WITH MONEY YOU DO NOT HAVE!!!!");
          refresh();
        }
        break;
      case '3':
        if(world.pc.player_bag.coin - 2 > -1){
          world.pc.player_bag.potion++;
          world.pc.player_bag.coin = world.pc.player_bag.coin - 2;
        }else{
          mvprintw(x++, 2, "DO NOT TEASE ME WITH MONEY YOU DO NOT HAVE!!!!");
          refresh();
        }
        break;
      default:
        break;
    }
  }
}

void io_pokemon_center()
{
  mvprintw(0, 0, "Welcome to the Pokemon Center.  Pokemon health is restored.");
  int i;
  for(i = 0; i < (int)(world.pc.poke.size() - 1); ++i){
    world.pc.poke.at(i).effective_stat[stat_hp] = world.pc.poke.at(i).startinghp;
  }
  refresh();
  getch();
}

void io_gym(){
  int trainers = rand() % 4 + 1;
  int i, fight, key;
  mvprintw(0, 0, "Welcome to the local gym. There are currently %d trainers", trainers);
  mvprintw(1, 0, "If you are too scared to fight here press escape, press any other key to fight");
  refresh();
  switch(key = getch()){
    case 27:
      fight = 0;
      break;
    default:
      fight = 1;
      break;
  }
  for(i = 0; i < trainers && fight == 1; ++i){
    Npc *npc = new Npc;
    io_battle(&world.pc, npc);
    if(i == trainers - 1){
      break;
    }
    mvprintw(0, 0, "Welcome to the local gym. There are currently %-50d trainers", trainers - i - 1);
    mvprintw(1, 0, "If you are too scared to fight here press escape, press any other key to fight");
    switch(key = getch()){
      case 27:
        fight = 0;
        break;
      default:
        break;
    }
  }
}

void io_list_pokemon(int option) {
  int i, x = 4, valid = 0, heal = 1;
  int size =  world.pc.poke.size();
  int input;

  do {
    
    io_display();

    if (!option) {
      mvprintw(x++, 2, " %-30s ", "Pokemon (Select to Swap)");
    }

    else {
      mvprintw(x++, 2, " %-30s ", "Pokemon (Select to Use Item)");
    }
    
    mvprintw(x++, 2, " %-30s ", "");

    for (i = 0; i < size; i++) {
      mvprintw(x++, 2, " %d %-19s HP: %-4d ", i+1, world.pc.poke[i].get_species(), world.pc.poke[i].get_hp());
    }
    
    mvprintw(x++, 2, " %-30s ", "");
    mvprintw(x++, 2, " %-30s ", "Click 'esc' to exit");

    x = 4;

    switch(input = getch()) {
    case '1':
      input = 1;
      break;
    case '2':
      input = 2;
      break;
    case '3':
      input = 3;
      break;
    case '4':
      input = 4;
      break;
    case '5':
      input = 5;
      break;
    case '6':
      input = 6;
      break;
    case 27:
      if(world.pc.poke[0].effective_stat[0] == 0) {
	mvprintw(4, 2, " %-30s ", "Need Available Pokemon");
	getch();
	break;
      }
      valid = 1;
      break;
    }
    
    if (!option && size > input-1) {
      if (world.pc.poke[input-1].get_hp() != 0) {	
	std::swap(world.pc.poke[0], world.pc.poke[input-1]);
	valid = 1;
      }

      else {
	mvprintw(4, 2, " %-30s ", "Can't Swap");
	getch();
      }
    }

    else if (option == 1  && size > input-1) {
      if ((world.pc.poke[input-1].get_hp() != 0) && heal < 21
	  && (world.pc.poke[input-1].get_hp() != world.pc.poke[input-1].base[0])) {
	while (world.pc.poke[input-1].get_hp() < world.pc.poke[input-1].base[0]) {
	  world.pc.poke[input-1].effective_stat[0] += 1;
	  heal++;
	}

	world.pc.player_bag.potion--;
	valid = 1;	
      }

      else {
	mvprintw(4, 2, " %-30s ", "Can't use Potion");
	getch();
      }
    }

    else if (option == 3 && size > input-1) {
      if (world.pc.poke[input-1].get_hp() == 0) {
	world.pc.poke[input-1].effective_stat[0] += (world.pc.poke[input-1].base[0] / 2);

	world.pc.player_bag.revive--;
	valid = 1;
      }

      else {
	mvprintw(4, 2, " %-30s ", "Can't use Revive");
	getch();
      }
    }

    refresh();  
  } while (!valid);

  io_display();
  refresh();
}

int check_poke(Character* trainer) {
  int size = trainer->poke.size();

  for (int i = 0; i < size; i++) {
    if (trainer->poke[i].effective_stat[0] != 0) {
      return i;
    }
  }

  return 0;
}

int rand_mov(Pokemon *p) {
  int ran_range, rand_num;
  
  if (strcmp(p->get_move(0), "")) {
    ran_range = 1;
  }

  if (strcmp(p->get_move(1), "")) {
    ran_range = 2;
  }

  if (strcmp(p->get_move(2), "")) {
    ran_range = 3;
  }

  if (strcmp(p->get_move(3), "")) {
    ran_range = 4;
  }

  rand_num = rand() % ran_range;

  return rand_num;
}

int io_bag(int catchable) {
  int key, x = 4, size =  world.pc.poke.size();

  do {
    io_display();
    mvprintw(x++, 2, " %-30s ", "Bag");
    mvprintw(x++, 2, " %-30s ", "");
    mvprintw(x++, 2, " Potions(1): %-17d ", world.pc.player_bag.potion);
    mvprintw(x++, 2, " PokeBalls(2): %-15d ", world.pc.player_bag.pokeball);
    mvprintw(x++, 2, " Revives(3): %-17d ", world.pc.player_bag.revive);
    mvprintw(x++, 2, " Coins:  %-17d", world.pc.player_bag.coin);
    mvprintw(x++, 2, " %-30s ", "");
    mvprintw(x++, 2, " %-30s ", "Click 'esc' to exit");
      
    x = 4;

    switch(key = getch()) {
    case '1':
      if (world.pc.player_bag.potion != 0) {
	io_list_pokemon(1);
	return 0;
      }

      else {
	mvprintw(4, 2, " %-30s ", "No Potions!");
	getch();
      }
      break;
    case '2':
      if (world.pc.player_bag.pokeball != 0) {
	if (catchable == 1 && size < 6) {
	  mvprintw(4, 2, " %-30s ", "Caught!");
	  getch();
	  world.pc.player_bag.pokeball--;
	  return 1;
	}

	else if (catchable == 2) {
	  mvprintw(4, 2, " %-30s ", "What Are you Throwing At?");
	  getch();
	}

	else if (size > 6 && catchable == 1){
	  mvprintw(4, 2, " %-30s ", "Pokemon Ran Away :(");
	  getch();
	}

	else {
	  mvprintw(4, 2, " %-30s ", "Can't Catch!");
	  getch();
	}
      
	world.pc.player_bag.pokeball--;
      }

      else {
	mvprintw(4, 2, " %-30s ", "No Balls!");
	getch();
      }
      break;
    case '3':
       if (world.pc.player_bag.revive != 0) {
	io_list_pokemon(3);
	return 0;
      }

      else {
	mvprintw(4, 2, " %-30s ", "No Revives!");
	getch();
      }
      break;
    }

    refresh();
  } while (key != 27);

  io_display();
  refresh();

  return 0;
}

int find_priority(int p_player, int p_cpu, int spd_player, int spd_cpu) {

  if (p_player > p_cpu) {
    return 0;
  }

  else if (p_cpu > p_player) {
    return 1;
  }

  else {
    if (spd_player > spd_cpu) {
      return 1;
    }

    else if(spd_cpu > spd_player) {
      return 0;
    }

    else {
      return rand() % 2;
    }
  }
   
  return 0;
}

int damage_calculator(Pokemon *attacker, Pokemon *defender, int move) {

  int i, power, attack, defense, level, base_speed,
    rand_num, base, move_type, type = 1;
  double critical = 1.0, stab = 1.0;

  if (moves[attacker->move_index[move]].power == -1) {
    return 0;
  }

  if (!(rand() % 100 < moves[attacker->move_index[move]].accuracy)) {
    return -1;
  }

  level = attacker->get_level();
  power = moves[attacker->move_index[move]].power;
  attack = attacker->get_atk();
  defense = defender->get_def();
  base_speed = pokemon_stats[attacker->pokemon_species_index * 6 - 0].base_stat;
  move_type = moves[attacker->move_index[move]].type_id;

  for (i = 0; i <= 1675; i++) {
    if (pokemon_types[i].pokemon_id == attacker->pokemon_species_index) {
      if (pokemon_types[i].type_id == move_type) {
	stab = 1.5;
      }
    }
  }

  if(rand() % 256 < (base_speed / 2)) {
    critical = 1.5;
  }

  rand_num = (rand() % 16 + 85);

  base = (((((2 * level) / 5) + 2) * power * (attack / defense)) / 50) + 2;

  return base * critical * ((float)rand_num / 100.0) * type * stab;
}

 int io_fight(Character *Npc, Pokemon *user, Pokemon *enemy, int other, int wild) {
  int key, valid = 0, move, x = 4, user_dam_max, enemy_dam_max;

  mvprintw(x++, 2, " %-50s ", "BATTLE!");
  mvprintw(x++, 2, " %-50s ", "");
  mvprintw(x++, 2, " LVL %-46d ", enemy->get_level());
  mvprintw(x++, 2, " %-50s ", enemy->get_species());
  mvprintw(x++, 2, " HP: %-46d ", enemy->get_hp());
  mvprintw(x++, 2, " %-50s ", "");
  mvprintw(x++, 2, " %-50s ","vs");
  mvprintw(x++, 2, " %-50s ", "");
  mvprintw(x++, 2, " LVL %-46d ", user->get_level());
  mvprintw(x++, 2, " %-50s ", user->get_species());
  mvprintw(x++, 2, " HP: %-46d ", user->get_hp());
  mvprintw(x++, 2, " %-50s ", "");
  mvprintw(x, 2, " %-25s", "'F'ight");
  mvprintw(x++, 28, "%-25s ", "'B'ag");
  mvprintw(x, 2, " %-25s", "'P'okemon");
  mvprintw(x++, 28, "%-25s ", "'R'un");
  

  if (!other) {
    x -= 2;
    mvprintw(x, 2, " 1 %-22s", user->get_move(0));
    mvprintw(x++, 28, "%-25s ", user->get_move(2));
    mvprintw(x, 2, " 2 %-22s", user->get_move(1));
    mvprintw(x++, 28, "%-25s ", user->get_move(3));
    
    do {  
      switch(key = getch()) {
      case '1':
	if (!strcmp(user->get_move(0), "")) {
	  break;
	}

	move = 0;
	valid = 1;
      
	break;
      case '2':
	if (!strcmp(user->get_move(1), "")) {
	  break;
	}

	move = 1;
	valid = 1;
      
	break;
      case '3':
	if (!strcmp(user->get_move(2), "")) {
	  break;
	}

	move = 2;
	valid = 1;
      
	break;
      case '4':
	if (!strcmp(user->get_move(3), "")) {
	  break;
	}

	move = 3;
	valid = 1;
      
	break;

      case 27:
	return 0;
	break;
      }
      
      refresh();
    } while(key != 27 && !valid);

    user_dam_max = damage_calculator(user, enemy, move);
  }

  int enemy_move = rand_mov(enemy);
  enemy_dam_max = damage_calculator(enemy, user, enemy_move);

  int player_turn = 0, cpu_turn = 0;
  int priority = 1;
  
  if (other) {
    player_turn = 1;
  }

  else {
    priority = find_priority(moves[user->move_index[move]].priority,
			       moves[enemy->move_index[enemy_move]].priority, user->get_speed(), enemy->get_speed());
  }

  while (!player_turn || !cpu_turn) {
    if (!other && !priority) {
      player_turn = 1;
      if (user_dam_max == -1) {
	      mvprintw(4, 2, " %-50s ", "");
	      mvprintw(4, 2, " %-50s ", "Miss");
	      getch();
	      break;
      }

      mvprintw(4, 2, " %-50s ", "");
      mvprintw(4, 2, " %s Used %s for %d Damage", user->get_species(), user->get_move(move), user_dam_max);
      
      while (enemy->effective_stat[0] != 0 && user_dam_max != 0) {
	      enemy->effective_stat[0]--;
	      user_dam_max--;
      }
	
      mvprintw(8, 2, " HP: %-46d ", enemy->get_hp());
      getch();

      if (enemy->effective_stat[0] == 0) {
	      if (wild) {
	        mvprintw(4, 2, " %-50s ", "");
	        mvprintw(4, 2, " %-50s ", "Wild Pokemon Fainted");
	        getch();
	        return 1;
	      }
	
	      if (!check_poke(Npc)) {
	        mvprintw(4, 2, " %-50s ", "");
	        mvprintw(4, 2, " %-50s ", "All Pokemon Fainted, Trainer Defeated!");
	        getch();
	        return 1;
	      }

	      mvprintw(4, 2, " %-50s ", "");
	      mvprintw(4, 2, " %-50s ", "Defeated Pokemon");
	      std::swap(Npc->poke[0], Npc->poke[check_poke(Npc)]);
	      getch();
	      return 0;
      }
    }

    else {
      cpu_turn = 1;
      if (enemy_dam_max == -1) {
	mvprintw(4, 2, " %-50s ", "");
	mvprintw(4, 2, " %-50s ", "Miss");
	getch();;
      }

      mvprintw(4, 2, " %-50s ", "");
      mvprintw(4, 2, " %s Used %s for %d Damage", enemy->get_species(), enemy->get_move(enemy_move), enemy_dam_max);
      
      while (user->effective_stat[0] != 0 && enemy_dam_max != 0) {
	user->effective_stat[0]--;
	enemy_dam_max--;
      }

      mvprintw(14, 2, " HP: %-46d ", user->get_hp());
      getch();

      if (user->effective_stat[0] == 0) {
	mvprintw(4, 2, " %-50s ", "");
	mvprintw(4, 2, " %-50s ", "Your Pokemon Fainted");
	getch();

	int size = world.pc.poke.size();

	for (int i = 0; i < size; i++) {
	  if (world.pc.poke[i].effective_stat[0] != 0) {
	    io_list_pokemon(0);
	    return 0;
	  }
	}
	
	mvprintw(4, 2, " %-50s ", "");
	mvprintw(4, 2, " %-50s ", "All Pokemon Fainted, YOU LOSE");
	getch();

	io_reset_terminal();
	exit(0);
      }

      if (other) {
	return 0;
      }
    }
    priority = priority == 1 ? 0 : 1;
  }
  return 0;
  //getch();
}

int io_battle_sequence(Character *npc, Pokemon *wild) {
  Pokemon *p;
  Npc *trainer;
  int key, valid = 0, x = 4, cancatch = 0, attempts = 0, odds; 

  if (npc != NULL) {
    trainer = dynamic_cast<Npc *>(npc);
    p = &trainer->poke[0];
    world.pc.player_bag.coin += trainer->poke.size();
  }

  else {
    p = wild;
    cancatch = 1;
    world.pc.player_bag.coin += 1;
    odds = ((world.pc.poke[0].get_speed() * 32) / ((p->get_speed() / 4) % 256)) + 32 * attempts;
  }

  do {
    if (cancatch) {
      mvprintw(x++, 2, " %-50s ", "A Wild Pokemon Appeared!");
    }

    else {
      mvprintw(x++, 2, " %-50s ", "BATTLE!");
    }
  
    //mvprintw(x++, 2, " %-50s ", "BATTLE!");
    mvprintw(x++, 2, " %-50s ", "");
    mvprintw(x++, 2, " LVL %-46d ", p->get_level());
    mvprintw(x++, 2, " %-50s ", p->get_species());
    mvprintw(x++, 2, " HP: %-46d ", p->get_hp());
    mvprintw(x++, 2, " %-50s ", "");
    mvprintw(x++, 2, " %-50s ","vs");
    mvprintw(x++, 2, " %-50s ", "");
    mvprintw(x++, 2, " LVL %-46d ", world.pc.poke[0].get_level());
    mvprintw(x++, 2, " %-50s ", world.pc.poke[0].get_species());
    mvprintw(x++, 2, " HP: %-46d ", world.pc.poke[0].get_hp());
    mvprintw(x++, 2, " %-50s ", "");
    mvprintw(x, 2, " %-25s", "(F)ight");
    mvprintw(x++, 28, "%-25s ", "(B)ag");
    mvprintw(x, 2, " %-25s", "(P)okemon");
    mvprintw(x++, 28, "%-25s ", "(R)un");
    x = 4;

    switch(key = getch()) {
    case 'F':
    case 'f':
      valid = io_fight(trainer, &world.pc.poke[0], p, 0, cancatch);
      break;
    case 'B':
    case 'b':
      if (io_bag(cancatch)) {
	world.pc.poke.push_back(*p);	
	valid = 1;
	break;
      }

      io_fight(trainer, &world.pc.poke[0], p, 1, cancatch);
      valid = 0;
      break;
    case 'P':
    case 'p':
      io_list_pokemon(0);
      io_fight(trainer, &world.pc.poke[0], p, 1, cancatch);
      valid = 0;
      break;
    case 'R':
    case 'r':
      if (!cancatch) {
	mvprintw(4, 2, " %-50s ", "Can't Run From A Trainer");
	getch();
      }

      else {
	if (rand() % 256 < odds) {
	  mvprintw(4, 2, " %-50s ", "ESCAPED!");
	  getch();
	  valid = 1;
	}

	else {
	  mvprintw(4, 2, " %-50s ", "CAN'T ESCAPE!");
	  getch();
	  io_fight(trainer, &world.pc.poke[0], p, 1, cancatch);
	  valid = 0;
	}
      }
      break;
    default:
      break;
    }
      
    refresh();
  } while (!valid);

  return 0;
}

void io_battle(Character *aggressor, Character *defender)
{
  Npc *npc;
  int poke_amount = 0;

   Pokemon *p;
  
  int md = (abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)) +
            abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)));
  int minl, maxl;
  
  if (md <= 200) {
    minl = 1;
    maxl = md / 2;
  } else {
    minl = (md - 200) / 2;
    maxl = 100;
  }
  if (minl < 1) {
    minl = 1;
  }
  if (minl > 100) {
    minl = 100;
  }
  if (maxl < 1) {
    maxl = 1;
  }
  if (maxl > 100) {
    maxl = 100;
  }

  io_display();
  //mvprintw(0, 0, "Aww, how'd you get so strong?  You and your pokemon must share a special bond!");
  //refresh();
  //getch();
  
  if (!(npc = dynamic_cast<Npc *>(aggressor))) {
    npc = dynamic_cast<Npc *>(defender);
  }

  npc->poke.erase(npc->poke.begin());
  
  while (poke_amount != 6 && rand() % 10 < 6) {
    p = new Pokemon(rand() % (maxl - minl + 1) + minl);
    npc->poke.push_back(*p);
    poke_amount++;
  }

  io_battle_sequence(npc, NULL);
  
  npc->defeated = 1;
  if (npc->ctype == char_hiker || npc->ctype == char_rival) {
    npc->mtype = move_wander;
  }
}

uint32_t move_pc_dir(uint32_t input, pair_t dest)
{
  dest[dim_y] = world.pc.pos[dim_y];
  dest[dim_x] = world.pc.pos[dim_x];

  switch (input) {
  case 1:
  case 2:
  case 3:
    dest[dim_y]++;
    break;
  case 4:
  case 5:
  case 6:
    break;
  case 7:
  case 8:
  case 9:
    dest[dim_y]--;
    break;
  }
  switch (input) {
  case 1:
  case 4:
  case 7:
    dest[dim_x]--;
    break;
  case 2:
  case 5:
  case 8:
    break;
  case 3:
  case 6:
  case 9:
    dest[dim_x]++;
    break;
  case '>':
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_mart) {
      io_pokemart();
    }
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_center) {
      io_pokemon_center();
    }
    if(world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] == ter_gym){
      io_gym();
    }
    break;
  }

  if ((world.cur_map->map[dest[dim_y]][dest[dim_x]] == ter_exit) &&
      (input == 1 || input == 3 || input == 7 || input == 9)) {
    // Exiting diagonally leads to complicated entry into the new map
    // in order to avoid INT_MAX move costs in the destination.
    // Most easily solved by disallowing such entries here.
    return 1;
  }

  if (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) {
    if (dynamic_cast<Npc *>(world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) &&
        ((Npc *) world.cur_map->cmap[dest[dim_y]][dest[dim_x]])->defeated) {
      // Some kind of greeting here would be nice
      return 1;
    } else if (dynamic_cast<Npc *>
               (world.cur_map->cmap[dest[dim_y]][dest[dim_x]])) {
      io_battle(&world.pc, world.cur_map->cmap[dest[dim_y]][dest[dim_x]]);
      // Not actually moving, so set dest back to PC position
      dest[dim_x] = world.pc.pos[dim_x];
      dest[dim_y] = world.pc.pos[dim_y];
    }
  }
  
  if (move_cost[char_pc][world.cur_map->map[dest[dim_y]][dest[dim_x]]] ==
      INT_MAX) {
    return 1;
  }

  return 0;
}

void io_teleport_world(pair_t dest)
{
  int x, y;
  
  world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = NULL;

  mvprintw(0, 0, "Enter x [-200, 200]: ");
  refresh();
  echo();
  curs_set(1);
  mvscanw(0, 21, (char *) "%d", &x);
  mvprintw(0, 0, "Enter y [-200, 200]:          ");
  refresh();
  mvscanw(0, 21, (char *) "%d", &y);
  refresh();
  noecho();
  curs_set(0);

  if (x < -200) {
    x = -200;
  }
  if (x > 200) {
    x = 200;
  }
  if (y < -200) {
    y = -200;
  }
  if (y > 200) {
    y = 200;
  }
  
  x += 200;
  y += 200;

  world.cur_idx[dim_x] = x;
  world.cur_idx[dim_y] = y;

  new_map(1);
  io_teleport_pc(dest);
}

void io_encounter_pokemon()
{
  Pokemon *p;
  
  int md = (abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)) +
            abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)));
  int minl, maxl;
  
  if (md <= 200) {
    minl = 1;
    maxl = md / 2;
  } else {
    minl = (md - 200) / 2;
    maxl = 100;
  }
  if (minl < 1) {
    minl = 1;
  }
  if (minl > 100) {
    minl = 100;
  }
  if (maxl < 1) {
    maxl = 1;
  }
  if (maxl > 100) {
    maxl = 100;
  }

  p = new Pokemon(rand() % (maxl - minl + 1) + minl);

  //  std::cerr << *p << std::endl << std::endl;
  /*

  io_queue_message("%s%s%s: HP:%d ATK:%d DEF:%d SPATK:%d SPDEF:%d SPEED:%d %s",
                   p->is_shiny() ? "*" : "", p->get_species(),
                   p->is_shiny() ? "*" : "", p->get_hp(), p->get_atk(),
                   p->get_def(), p->get_spatk(), p->get_spdef(),
                   p->get_speed(), p->get_gender_string());
  io_queue_message("%s's moves: %s %s", p->get_species(),
                   p->get_move(0), p->get_move(1));

  // Later on, don't delete if captured
  */

  if(!(io_battle_sequence(NULL, p))) {
    delete p;
  }
  
  //delete p;
}

void io_start() {

  Pokemon *p1, *p2, *p3;
  int key, valid = 0;

  p1 = new Pokemon(1);
  p2 = new Pokemon(1);
  p3 = new Pokemon(1);

  mvprintw(4, 2, "%-45s ", "Please Select Your Starting Pokemon: '1' '2' or '3'");
  mvprintw(5, 2, " %-45s ", "");
  mvprintw(6, 2, "1: %-43s ", p1->get_species());
  mvprintw(7, 2, "2: %-43s ", p2->get_species());
  mvprintw(8, 2, "3: %-43s ", p3->get_species());

  do {
    switch(key = getch()) {
    case '1':
      world.pc.poke[0] = *p1;
      valid = 1;
      break;
    case '2':
      world.pc.poke[0] = *p2;
      valid = 1;
      break;
    case '3':
      world.pc.poke[0] = *p3;
      valid = 1;
      break;
    default:
      break;
    }
    
    refresh();
  } while (!valid);
}

void io_handle_input(pair_t dest)
{
  uint32_t turn_not_consumed;
  int key;

  do {
    switch (key = getch()) {
    case '7':
    case 'y':
      turn_not_consumed = move_pc_dir(7, dest);
      break;
    case '8':
    case 'k':
      turn_not_consumed = move_pc_dir(8, dest);
      break;
    case '9':
    case 'u':
      turn_not_consumed = move_pc_dir(9, dest);
      break;
    case '6':
    case 'l':
      turn_not_consumed = move_pc_dir(6, dest);
      break;
    case '3':
    case 'n':
      turn_not_consumed = move_pc_dir(3, dest);
      break;
    case '2':
    case 'j':
      turn_not_consumed = move_pc_dir(2, dest);
      break;
    case '1':
    case 'b':
      turn_not_consumed = move_pc_dir(1, dest);
      break;
    case '4':
    case 'h':
      turn_not_consumed = move_pc_dir(4, dest);
      break;
    case '5':
    case ' ':
    case '.':
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    case KEY_RIGHT:
      turn_not_consumed = move_pc_dir('>', dest);
      break;
    case 'Q':
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      world.quit = 1;
      turn_not_consumed = 0;
      break;
      break;
    case 't':
      /* Teleport the PC to a random place in the map.              */
      io_teleport_pc(dest);
      turn_not_consumed = 0;
      break;
    case 'T':
      /* Teleport the PC to any map in the world.                   */
      io_teleport_world(dest);
      turn_not_consumed = 0;
      break;
    case 'm':
      io_list_trainers();
      turn_not_consumed = 1;
      break;
    case 'B':
      io_bag(2);
      turn_not_consumed = 1;
      break;
    case 'q':
      /* Demonstrate use of the message queue.  You can use this for *
       * printf()-style debugging (though gdb is probably a better   *
       * option.  Not that it matters, but using this command will   *
       * waste a turn.  Set turn_not_consumed to 1 and you should be *
       * able to figure out why I did it that way.                   */
      io_queue_message("This is the first message.");
      io_queue_message("Since there are multiple messages, "
                       "you will see \"more\" prompts.");
      io_queue_message("You can use any key to advance through messages.");
      io_queue_message("Normal gameplay will not resume until the queue "
                       "is empty.");
      io_queue_message("Long lines will be truncated, not wrapped.");
      io_queue_message("io_queue_message() is variadic and handles "
                       "all printf() conversion specifiers.");
      io_queue_message("Did you see %s?", "what I did there");
      io_queue_message("When the last message is displayed, there will "
                       "be no \"more\" prompt.");
      io_queue_message("Have fun!  And happy printing!");
      io_queue_message("Oh!  And use 'Q' to quit!");

      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    default:
      /* Also not in the spec.  It's not always easy to figure out what *
       * key code corresponds with a given keystroke.  Print out any    *
       * unhandled key here.  Not only does it give a visual error      *
       * indicator, but it also gives an integer value that can be used *
       * for that key in this (or other) switch statements.  Printed in *
       * octal, with the leading zero, because ncurses.h lists codes in *
       * octal, thus allowing us to do reverse lookups.  If a key has a *
       * name defined in the header, you can use the name here, else    *
       * you can directly use the octal value.                          */
      mvprintw(0, 0, "Unbound key: %#o ", key);
      turn_not_consumed = 1;
    }
    refresh();
  } while (turn_not_consumed);
}
