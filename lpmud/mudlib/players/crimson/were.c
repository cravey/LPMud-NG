#include "std.h"

object girl;
object werewolf;
object jackbox;
object toybox;
object tear;

#undef EXTRA_RESET
#define EXTRA_RESET\
   extra_reset();

#undef EXTRA_INIT
#define EXTRA_INIT\
    add_action("kill"); add_verb("kill");\
    add_action("kill"); add_verb("fireball");\
    add_action("kill"); add_verb("shock");\
    add_action("kill"); add_verb("chill");\
    add_action("kill"); add_verb("strike");\
    add_action("kill"); add_verb("missile");

ONE_EXIT("players/crimson/eastrail", "west",
          "The Nursery",
          "This is the nursery for children too young to attend\n" +
          "mourning services.  Various toys are scattered about\n" +
          "the room. Overall, the whole place looks very sterile.\n", 1)

extra_reset() {
    if (!toybox || !environment(toybox)) {
          toybox = clone_object("players/crimson/toybox");
          move_object(toybox, this_object());
          jackbox = clone_object("players/crimson/.surprise");
          move_object(jackbox, toybox);
    }
    if ((!werewolf || !living(werewolf)) && (!girl || !living(girl))) {
          girl = clone_object("obj/monster");
          call_other(girl, "set_name", "girl");
          call_other(girl, "set_short", "A little girl");
          call_other(girl, "set_alias", "little girl");
          call_other(girl, "set_long", "A cute little girl in pig tails.\n");
          call_other(girl, "set_level", 4);
          call_other(girl, "set_wc", 3);
          call_other(girl, "set_ac", 12);
          call_other(girl, "set_race", "were");
          call_other(girl, "set_al", 300);
          call_other(girl, "set_eps", 1);
          move_object(girl, this_object());
          }
}

kill(str) {
string killer;

    killer = this_player();
    if (str == "girl" || str == "little girl" || str == "were" && present("girl")) {
    destruct(girl);
    tell_room(this_object(), "Before your eyes the girl transforms into a wolf!\n");
    if (!werewolf || !living(werewolf)) {
    werewolf = clone_object("obj/monster");
    call_other(werewolf, "set_name", "wolf");
    call_other(werewolf, "set_short", "A huge wolf");
    call_other(werewolf, "set_alias", "werewolf");
    call_other(werewolf, "set_race", "were");
    call_other(werewolf, "set_level", 18);
    call_other(werewolf, "set_wc", 24);
    call_other(werewolf, "set_ac", 9);
    call_other(werewolf, "set_al", -1000);
    call_other(werewolf, "set_eps", 1000000);
    call_other(werewolf, "set_hp", 200);
    call_other(werewolf, "set_chance", 40);
    call_other(werewolf, "set_spell_mess1", "The wolf howls in joy for the blood bath.\n");
    call_other(werewolf, "set_spell_mess2", "The wolf's claws sink deep into your flesh.\n");
    call_other(werewolf, "set_spell_dam", 15);
    call_other(werewolf, "set_aggressive", 1);
    move_object(werewolf, this_object());
    call_other(werewolf, "attack_object", killer);
    call_other(killer, "set_title", "the Werewolf");
    tear = clone_object("players/crimson/tear");
    move_object(tear, werewolf);
    return 1;
    }
  }
}
