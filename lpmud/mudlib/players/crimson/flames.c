#include "../../obj/living.h"
string vortex;

/*
 * The heart beat is started in living.h when we are attacked.
 */

reset(arg)
{

    if (arg)
       return;
    max_hp = 200;
    hit_point = 200;
    level = 19;
    experience = 39000;
    weapon_class = 35;
    is_npc = 1;
    name = "stalker";
    cap_name = "Something";
    alignment = 1000;
    enable_commands();
}

short() {
    if((call_other(this_player(), "query_level")> 600)) {
    return("An invisible stalker");
    }
    return 0;
}

long() {
    if ((call_other(this_player(), "query_level")< 600)) {
    write("Cool breeze blows against your face.\n");
    } else
    if ((call_other(this_player(), "query_level")> 600)) {
    write("This is an invisible stalker. Oddly, you can see it.\n");
    }
}

id(str) {
    return str == name; 
}

heart_beat()
{
    age += 1;
    if (!attack())
	set_heart_beat(0);
}

can_put_and_get(str)
{
    if (!str) {
        write(name + " says: HA!.\n");
	return 0;
    }
    return 1;
}
