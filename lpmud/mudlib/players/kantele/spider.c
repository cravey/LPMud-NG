#include "../../obj/living.h"

reset(arg)
{
    if (arg)
       return;
    max_hp =15;
    hit_point =25;
    level = 10;
    experience =70000;
    weapon_class =55;
    is_npc = 1;
    name = "spider";
    cap_name = "the spider";
    alignment = 100;
    enable_commands();
}

short() {
   return "A small poison spider";
}

long() {
    write("A litle black spider.");
}

id(str) { return str == name; }

heart_beat()
{
    age += 1;
    if (!attack())
	set_heart_beat(0);
}

