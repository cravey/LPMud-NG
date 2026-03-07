/*
 */

string short_desc, long_desc;
int value, local_weight, spell, charges, blast_dam;
string name, alias_name, alias2;
string read_msg, real_name, wear_msg, blast_id;
object func_ob;

query_charges() { return charges; }

query_magic() { return 1;}

query_id() { return real_name; }

set_real_id(n) { real_name = n; }

set_alias2(n) { alias2=n; }

set_object(ob) { func_ob = ob; }

set_blast_id(str) { blast_id = str; }

set_blast_dam(d) { blast_dam = d; }

id(str)
{
    return str == name || str == alias_name || str == alias2;
}

short() {
    return short_desc;
}

long() {
    write(long_desc);
}

query_value() { return value*(charges+1); }

set_id(str) {
    local_weight = 1;
    name = str;
}

set_alias(str) {
    alias_name = str;
}

set_short(str) {
    short_desc = str;
    long_desc = "You see nothing special.\n";
}

set_long(str) {
    long_desc = str;
}

set_value(v) {
    value = v;
}

set_weight(w) {
    local_weight = w;
}

get() {
    return 1;
}

query_weight() {
    return local_weight;
}

init() {
    add_action("use"); add_verb("use");
    add_action("brk"); add_verb("break");
}

brk(str) {
    if (!id(str)) return 0;
    say(call_other(this_player(),"query_name")+" breaks a "+name+".\n");
    if (charges <=0) {
        write("The "+name+" shatters into magical fragments and is gone.\n");
        destruct(this_object());
        return 1;
    }
    write("The "+name+" explodes in a maelstrom of magical plasma!\n");
    write("The room and all in it are engulfed in the explosion.\n");
    say("The "+name+" explodes in a maelstrom of magical plasma!\n");
    say("The room and all in it are engulfed in the explosion.\n");
    magstorm();
    destruct(this_object());
    return 1;
}

magstorm() {
    object ob,obn,env;
    int r;

    env = environment(this_player());
    ob = first_inventory(env);
    while (ob) {
        obn = next_inventory(ob);
        if (random(100+charges) >= 100) 
          teleport(ob);
        if (living(ob)) {
            r = random(charges*value/10);
            call_other(ob,"hit_player",r);
        }
        ob=obn;
    }
}

teleport(ob) {
    string dest;
    int r;
    tell_object(ob,"You are magically teleported somewhere.\n");
    r = random(10)+1;
    if (r==1) dest = "players/ringer/mg_shop";
    if (r==2) dest = "players/ringer/for_cliff";
    if (r==3) dest = "players/ringer/en_stream";
    if (r==4) dest = "players/ringer/en_forest";
    if (r==5) dest = "room/vill_green";
    if (r==6) dest = "room/shop";
    if (r==7) dest = "room/yard";
    if (r==8) dest = "room/plane1";
    if (r==9) dest = "room/clearing";
    if (r==10) dest = "room/vill_track";
    if (call_other(ob,"query_npc")) {
      tell_room(environment(ob),call_other(ob,"query_name")+" disappears.\n");
      move_object(ob,dest);
      tell_room(environment(ob),call_other(ob,"query_name")+" appears.\n");
    } else
      call_other(ob,"move_player","X#"+dest);
}

set_spell(n) { spell = n; }

set_charges(n) { charges = n; }

use(str) {
    string s,who;
    who = 0;
    if (!str) return 0;
    if (sscanf(str, "%s on %s",s,who) < 2) s = str;
    if (!id(s))
	return 0;
    if (charges <=0) {
        write("Seems to have fizzled out...\n");
        return 1;
    }
    if (spell == 1) {
        return missile(who);
    }
    if (spell == 2) {
        return identify(who);
    }
    if (spell == 3) {
        return lightning(who);
    }
    if (spell == 4) {
        return fireball(who);
    }
    if (spell == 5) {
        if (call_other(func_ob,"use",who)) {
            charges = charges - 1;
            return 1;
        }
        return 1;
    }
    if (spell == 6) {
        return blast(who);
    }
}

target(who) {
       object at;
     if (!who) {
         at = call_other(this_player(),"query_attack");
         if (!at) {
             write("You aren't attacking anyone.  Try using on someone.\n");
             return 0;
         }
     } else {
         at = present(who,environment(this_player()));
         if (!at) {
             write("Who?\n");
             return 0;
         }
         if (!living(at)) {
             write("I don't do inanimate objects!\n");
             return 0;
         }
     }
     return at;
 }
  
missile(who) {
       object t;
     int d;
     t = target(who);
     if (!t) return 1;
     d = random(5) + random (5) + random(5);
     write("Missiles fly from the "+name+".\n");
     say("Missiles fly from the "+name+".\n");
     if (!call_other(this_player(),"query_attack")) 
         call_other(t,"attack_object",this_player());
     call_other(t,"hit_player",d);
     charges = charges - 1;
     return 1;
 }

lightning(who) {
       object t;
     int d;
     t = target(who);
     if (!t) return 1;
     d = random(5) + random (5) + random(5) + random(5) + random(5);
     write("A lightning bolt sizzles forth from the "+name+".\n");
     say("A lightning bolt sizzles forth from the "+name+".\n");
     if (!call_other(this_player(),"query_attack")) 
         call_other(t,"attack_object",this_player());
     call_other(t,"hit_player",d);
     charges = charges - 1;
     return 1;
 }

fireball(who) {
       object t;
     int d;
     t = target(who);
     if (!t) return 1;
     d = random(5) + random (5) + random(5);
     d = d + random(5) + random (5) + random(5);
     d = d + random(5);
     write("A ball of fire flashes forth from the "+name+".\n");
     say("A ball of fire flashes forth from the "+name + ".\n");
     if (!call_other(this_player(),"query_attack")) 
         call_other(t,"attack_object",this_player());
     call_other(t,"hit_player",d);
     charges = charges - 1;
     return 1;
 }

blast(who) {
     object t;
     int d;
     t = target(who);
     if (!t) return 1;
     if (call_other(t,"id",blast_id))
         d = blast_dam;
     else  
         d = random(10);
     write("A bolt of plasma streaks forth from the "+name+".\n");
     say("A bolt of plasma streaks forth from the "+name + ".\n");
     if (!call_other(this_player(),"query_attack")) 
         call_other(t,"attack_object",this_player());
     if (d == blast_dam) {
         write(call_other(t,"query_name") + " is knocked backwards!\n");
         say(call_other(t,"query_name") + " is knocked backwards!\n");
     } else {
         write(call_other(t,"query_name") + " is hardly affected.\n");
         say(call_other(t,"query_name") + " is hardly affected.\n");
     }
     call_other(t,"hit_player",d);
     charges = charges - 1;
     return 1;
 }

identify(who) {
       object ob;
       string s;
     if (!who) {
         write("Try using on something.\n");
         return 1;
     }
     ob = present(who);
     if (!ob) {
         write("You don't have that.\n");
         return 1;
     }
     say(call_other(this_player(),"query_name") + " identifies " + who +"\n");
     s = call_other(ob,"query_id",0);
     if (s) {                  /* If the item has a magical id, use it */
         call_other(ob,"set_id",s);
         call_other(ob,"set_name",s);
         call_other(ob,"set_short",s);
     }
     s = call_other(ob,"short");
     write("Name: "); write(s); write("\n");
     s = call_other(ob,"query_alias");
     if (s) {
         write("Alias: "); write(s); write("\n"); }
     s = call_other(ob,"query_type");
     if (s) {
         write("Type: "); write(s); write("\n"); }
     s = call_other(ob,"weapon_class");
     if (s) {
         write("Weapon class: "); write(s); write("\n"); }
     s = call_other(ob,"armor_class");
     if (s) {
         write("Armor class: "); write(s); write("\n"); }
     s = call_other(ob,"query_value");
     if (s) { 
         write("Value: "); write(s); write("\n"); }
     s = call_other(ob,"query_weight");
     if (s) {
         write("Weight: "); write(s); write("\n"); }
     s = call_other(ob,"query_charges");
     if (s) {
         write("Charges: "); write(s); write("\n"); }

     charges = charges - 1;
     return 1;
 }
