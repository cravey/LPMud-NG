#include "../../obj/living.h"

/*
 * The heart beat is started in living.h when we are attacked.
 */

reset(arg)
{
    object weapon;
    object key;

    if (arg)
       return;
    max_hp = 200;
    hit_point = 200;
    level = 10;
    experience = 39000;
    weapon_class = WEAPON_CLASS_OF_HANDS;
    is_npc = 1;
    name = "manservant";
    cap_name = "Manservant";
    alignment = 100;
    enable_commands();
    weapon = clone_object("obj/weapon");
    call_other(weapon, "set_name", "battleaxe");
    call_other(weapon, "set_class", 12);
    call_other(weapon, "set_alias", "battleaxe");
    call_other(weapon, "set_value",250);
    call_other(weapon, "set_weight", 3);
    call_other(weapon, "wield", "axe");
    move_object(weapon, this_object());
    call_other(weapon, "wield", "battleaxe");
    key = clone_object("obj/treasure");
    call_other(key, "set_id", "key");
    call_other(key, "set_alias", "mansion key");
    call_other(key, "set_short", "A key");
    call_other(key, "set_value", 10);
    call_other(key, "set_weight", 1);
    move_object(key, this_object());
}

short() {
    return "Kanteles manservant is here";
}

long() {
    write("Kanteles manservant is here, he looks mean !\n");
    if (hit_point > max_hp - 40)
       write("He seems to be in a good shape.\n");
}

id(str) { return str == name; }

heart_beat()
{
    age += 1;
    if (!attack())
	set_heart_beat(0);
}

can_put_and_get(str)
{
    if (!str) {
        write(name + " says: Over my dead body.\n");
	return 0;
    }
    return 1;
}
