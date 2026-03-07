# 1 "room/inn.c"

# 1 "room/room.h"
inherit "room/room";
























# 3 "room/inn.c"







int cm,mm,rmm;
string last_eater;









reset(arg) { if (!arg) { set_light( 1); short_desc =  "Eastroad Inn"; long_desc =  "You are in the Eastroad Inn. Here you can buy food to still your\n"+ "hunger, but only a limited selection is available.\n"; dest_dir = ({ "room/eastroad5", "east" }); } cm=2*4; mm=2*2; rmm=2; if(arg) return; items=({"menu","The menu looks like this"}); }
# 27 "room/inn.c"

init() {
 add_action("buy","buy");
 add_action("buy","order");
 ::init();
}

long(s) {
 ::long(s);
 show_menu();
}

show_menu() {
 write("\n");
 if(!(cm||mm||rmm)) 
  write("Innkeeper says: "+"We have completely sold out...come back later."+"\n");
 else {
  write("1: Commonor's Meal     ");write(cm);write(" at 20 gp\n");
  write("2: Merchant's Meal     ");write(mm);write(" at 50 gp\n");
  write("3: Rich Man's Meal     ");write(rmm);write(" at 90 gp\n");
  write("4: A Mug of Beer       ");write(" ");write("     2 gp\n");
  write("\n");
  write("Use 'buy <number>' to buy the desired Food. The food will\n"+
        "be consumed at once. Good appetite.\n\n");
 }
 return;
}

no_food() {
 write("Innkeeper says: "+"Sorry - we have sold out of that."+"\n");
 if(cm||mm||rmm)
  write("Innkeeper says: "+"Why don't you try something else ?"+"\n");
 else
  write("Innkeeper says: "+"Why don't you come back later ?"+"\n");
 return 1;
}

pays(n) {
 if(this_player()->query_money()<n) {
  write("Innkeeper says: "+"You cannot afford that."+"\n");
  return 0;
 }
 this_player()->add_money(-n);
 return 1;
}

tease(n) {
 if(this_player()->query_name()==last_eater)
  write("Innkeeper says: "+"My - Are we hungry today."+"\n");
 last_eater=this_player()->query_name();
 this_player()->heal_self(n);
 return 1;
}

buy(s) {
 if(!s) {
  write("Innkeeper says: "+"What do you want to buy ?"+"\n");
  return 1;
 }
 
if(s=="1"||s=="<1>") {
  if(!cm) return no_food();
  if(!pays(20)) return 1;
  if(this_player()->query_level()>6) {
   write("Innkeeper says: "+"You don't look like a commonor to me."+"\n");
   if(mm||rmm) {
    write("Innkeeper says: "+"You should eat food more suited for you."+"\n");
    return 1;
   }
   write("Innkeeper says: "+"But as we have no better food - here you are."+"\n");
  }
  write("You are served a commonor's meal - very nourishing\n");
  say(this_player()->query_name()+" orders a commonor's meal\n");
  cm=cm-1;
  return tease(4);
 }
 
 if(s=="2"||s=="<2>") {
  if(!mm) return no_food();
  if(!pays(50)) return 1;
  if(this_player()->query_level()>12) {
   write("Innkeeper says: "+"You look more like a richman to me."+"\n");
   if(rmm) {
    write("Innkeeper says: "+"You should eat food more suited for you."+"\n");
    return 1;
   }
   write("Innkeeper says: "+"But as we have no better food - here you are."+"\n");
  }
  write("You are served a merchant's meal - very good\n");
  say(this_player()->query_name()+" orders a merchant's meal\n");
  mm=mm-1;
  return tease(8);
 }
 
 if(s=="3"||s=="<3>") {
  if(!rmm) return no_food();
  if(!pays(90)) return 1;
  write("You are served a rich man's meal - very delicious\n");
  say(this_player()->query_name()+" orders a rich man's meal\n");
  rmm=rmm-1;
  return tease(12);
 }
 if(s=="4"||s=="<4>"||s=="mug"||s=="beer") {
  if(!pays(2)) return 1;
  if(!this_player()->drink_alcohol(2)) {
   write("Innkeeper says: "+"You look a little too drunk for that..let me take it back."+"\n");
   this_player()->add_money(2);
   return 1;
  }
  write("You drink a mug of first class beer - That feels good.\n");
  say(this_player()->query_name()+" drinks a beer.\n");
  return 1;
 }
 write("Innkeeper says: "+"We have no such number on the menu, try 1, 2 or 3."+"\n");
 return 1;
}

