# 1 "players/crimson/.good.c" 
# 1 "/home/einstein/mud/lpmud/mudlib/obj/handshake.h" 1



string handshake_data;
int handshake;









get_handshake(confirm) {
    if (confirm == handshake && handshake) {
        handshake = 0;
	return handshake_data;
    } else {
        return 0;
    }
}
# 2 "players/crimson/.good.c" 2
int charges;

reset(arg) {
   if (!arg) charges = 1;
}

short() { return "A grey parchment"; }

long() { 
   write("*****************************\n");
   write("*                           *\n");
   write("*                           *\n");
   write("*  This will aid you in     *\n");
   write("*                           *\n");
   write("*      finding your         *\n");
   write("*                           *\n");
   write("*       goody goody         *\n");
   write("*                           *\n");
   write("*          foes.            *\n");
   write("*                           *\n");
   write("*     align <opponent>      *\n");
   write("*                           *\n");
   write("*                           *\n");
   write("*****************************\n");
   return 1;
}

init() {
   add_action("read"); add_verb("read");
   add_action("align"); add_verb("align");
   add_action("align"); add_verb("alignment");
}

id(str) {
   return str == "grey parchment" || str == "grey scroll" || str == "scroll" || str == "grey scroll" || str == "parchment";
}

read(str) {
   if (!id(str))
    return 0;
   say(call_other(this_player(), "query_name") + " reads a grey parchment.\n");
   long();
   return 1;
}

align(str) {
  object liv;
  object attackr;
  string name;
  string targ;
  string align;

  if (!str) { return 1; }
  liv = present(lower_case(str), environment(this_player()));
  if (!liv) {
  write("That is not here.\n");
  return 1;
  }
  if (!living(liv)) {
  write("That is quite neutral as it is not even alive.\n");
  return 1;
  } else
  if (liv == this_player()) {
  write("Are you so dense that you do not know yourself?\n");
  destruct(this_object());
  write("The parchment crumbles to dust.\n");
  return 1;
  }
  name = call_other(this_player(), "query_name");
  targ = call_other(liv, "query_name");
  attackr = this_player();
  align = call_other(liv, "query_alignment", 0);
  write("The parchment becomes warm....\n");
  if (align<0) {
  write("Don't worry es one of us.\n");
  } else
  if ((align>0) && (align<30)) {
  write("That one could stand to be reformed a bit.\n");
  } else
  if ((align>30) && (align<200)) {
  write("That one is probably beyond reform.\n");
  } else
  if ((align>200) && (align<700)) {
  write("Goody two shoes there is making me sick.\n");
  } else
  if ((align>700) && (align<1000)) {
  write("Kill it before it spreds!\n");
  } else
  if (align>1000) {
  write("If you won't cleanse the world of that one I will!\n");
  call_other(attackr, "attack_object", liv);
  }
  if (charges == 1) {
    destruct(this_object());
    tell_object(this_player(), "You watch the parchment crumble before you.\n");
    }
    return 1;
}

query_weight() { return 1; }

get() { return 1; }

query_value() { return 300; }
