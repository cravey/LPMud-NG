#include "../../room/std.h"

#undef EXTRA_INIT
#define EXTRA_INIT\
       ex_init();

#undef EXTRA_RESET
#define EXTRA_RESET\
        my_reset();

#undef EXTRA_LONG
#define EXTRA_LONG\
      ex_long();

TWO_EXIT("players/ringer/en_for6", "east",
         "players/ringer/rd_lair", "north",
	   "You are in a hidden meadow",
"This meadow was obscured from the forest path (perhaps intentionally).\n"
+"There are signs that a magician practices new spells here (purple grass,\n"
+"frogs with hair, you even notice a 2 inch elephant scurry by)\n",1)

int passage;

ex_init() {
    add_action("west"); add_verb("west");
    add_action("search"); add_verb("search");
}

search(str) {
    if (!str) str = "room";
    if (str == "room") {
        write("A careful search of the area reveals a path leading west.\n");
        say(call_other(this_player(),"query_name")+" finds a path leading west.\n");
        passage = 1;
        return 1;
    }
}

ex_long(){
    if (passage) write("There is path leading west.\n");
}

west() {
    if (!passage) return 0;
    if (present("mulf")) {
        write("Mulf prevents you.\n");
        return 1;
    }
    if (present("elemental")) {
        write("An elemental bars the way.\n");
        return 1;
    }
    call_other(this_player(),"move_player","north#players/ringer/magtr");
    return 1;
}

object mulf,item;
my_reset() {
    passage=0;
    if (!present("mulf the mage")) {
        mulf = clone_object("obj/monster.talk");
        call_other(mulf,"set_name","mulf the mage");
        call_other(mulf,"set_alias","mulf");
        call_other(mulf,"set_level",15);
        call_other(mulf,"set_ac",6);
        call_other(mulf,"set_long","Mulf is practicing new spells.\n");
        call_other(mulf,"set_aggressive",0);
        call_other(mulf,"set_chance",30);
        call_other(mulf,"set_spell_mess1","Mulf casts a fireball.\n");
        call_other(mulf,"set_spell_mess2","Mulf casts a fireball.\n");
        call_other(mulf,"set_spell_dam",40);
        call_other(mulf,"set_al",500);
        call_other(mulf,"set_dead_ob",this_object());
        move_object(mulf,this_object());
        if (present("red stone")) {
            item = present("red stone");
            move_object(item,mulf);
        }
        if (!present("red stone",mulf)) {
            item = clone_object("players/ringer/trapstone");
            call_other(item,"set_id","red stone");
            call_other(item,"set_alias","stone");
            call_other(item,"set_short","A translucent red stone");
            call_other(item,"set_long",
"The stone is beautiful. Deep inside light plays in its facets resembling\n"
+"little tongues of flame.\n");
            call_other(item,"set_value",100);
            call_other(item,"set_weight",1);
            move_object(item,mulf);       
        }
    }
}    

object elemental,ob;

monster_died() {
    if (!elemental || !living(elemental)) {
        write("Mulf casts a spell with his dying breath!\n");
        say("Mulf casts a spell with his dying breath!\n");
        elemental = clone_object("obj/monster");
        call_other(elemental,"set_name","fire elemental");
        call_other(elemental,"set_alias","elemental");
        call_other(elemental,"set_level",8);
        call_other(elemental,"set_ac",5);
        call_other(elemental,"set_wc",18);
        call_other(elemental,"set_al",0);
        call_other(elemental,"set_long",
"The elemental is made entirely of flame!\n");
        call_other(elemental,"set_chance",20);
        call_other(elemental,"set_spell_mess1",
"The fire elemental throws a ball of fire.\n");
        call_other(elemental,"set_spell_mess2",
"The fire elemental throws a ball of fire.\n");
        call_other(elemental,"set_spell_dam",40);
        move_object(elemental,this_object());
        if (mulf)
          ob = call_other(mulf,"query_attack",0);
        if (ob) 
          call_other(elemental,"attack_object",ob);

        elemental = clone_object("obj/monster");
        call_other(elemental,"set_name","earth elemental");
        call_other(elemental,"set_alias","elemental");
        call_other(elemental,"set_level",7);
        call_other(elemental,"set_ac",7);
        call_other(elemental,"set_wc",7);
        call_other(elemental,"set_al",0);
        call_other(elemental,"set_long",
"The elemental is made of rock and soil.\n");
        call_other(elemental,"set_chance",20);
        call_other(elemental,"set_spell_mess1",
"The earth elemental throws a large clod.\n");
        call_other(elemental,"set_spell_mess2",
"The earth elemental throws a large clod.\n");
        call_other(elemental,"set_spell_dam",25);
        move_object(elemental,this_object());
        if (mulf)
          ob = call_other(mulf,"query_attack",0);
        if (ob) 
          call_other(elemental,"attack_object",ob);

        elemental = clone_object("obj/monster");
        call_other(elemental,"set_name","air elemental");
        call_other(elemental,"set_alias","elemental");
        call_other(elemental,"set_level",7);
        call_other(elemental,"set_ac",5);
        call_other(elemental,"set_wc",5);
        call_other(elemental,"set_al",0);
        call_other(elemental,"set_long",
"The elemental is made of thin air.\n");
        call_other(elemental,"set_chance",20);
        call_other(elemental,"set_spell_mess1",
"The air elemental summons a small tornado.\n");
        call_other(elemental,"set_spell_mess2",
"The air elemental summons a small tornado.\n");
        call_other(elemental,"set_spell_dam",35);
        move_object(elemental,this_object());
        if (mulf)
          ob = call_other(mulf,"query_attack",0);
        if (ob) 
          call_other(elemental,"attack_object",ob);

        elemental = clone_object("obj/monster");
        call_other(elemental,"set_name","water elemental");
        call_other(elemental,"set_alias","elemental");
        call_other(elemental,"set_level",7);
        call_other(elemental,"set_ac",5);
        call_other(elemental,"set_wc",5);
        call_other(elemental,"set_al",0);
        call_other(elemental,"set_long",
"The elemental is made of water.\n");
        call_other(elemental,"set_chance",20);
        call_other(elemental,"set_spell_mess1",
"The water elemental calls down a deluge of water.\n");
        call_other(elemental,"set_spell_mess2",
"The water elemental calls down a deluge of water.\n");
        call_other(elemental,"set_spell_dam",15);
        move_object(elemental,this_object());
        if (mulf)
          ob = call_other(mulf,"query_attack",0);
        if (ob) 
          call_other(elemental,"attack_object",ob); 
    }
    return 0;
}               

