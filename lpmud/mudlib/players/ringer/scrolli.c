string name, alias, short_desc, long_desc, value, weight;
string type;
int worn, ac;
object worn_by;
object next;

reset(arg)
{
    if(arg)
	return;
    type = "scroll";
}

init() {
    add_action("read"); add_verb("read");
    add_action("ident"); add_verb("identify");
}

read(str) {
    if (id(str)) {
        write("The scroll reads:\n usage: identify <item>\n");
        return 1;
    }
    return 0;
}

ident(str) {
      object ob;
      string s;
      if (!str) {
        write("Identify what?\n");
        return 1;
      }
      ob = present(str);
      if (!ob) {
         write("You don't have that.\n");
         return 1;
      }
      say(call_other(this_player(),"query_name") + " identifies " + str);
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
        write("Armour class: "); write(s); write("\n"); }
      s = call_other(ob,"query_value");
      if (s) { 
        write("Value: "); write(s); write("\n"); }
      s = call_other(ob,"query_weight");
      if (s) {
        write("Weight: "); write(s); write("\n"); }
      write("The scroll dissolves...\n");
      destruct(this_object());
      return 1;
}

rec_short()
{
    if(next)
	return name + ", " + call_other(next, "rec_short");
    return name;
}

short() {
    return short_desc;
}

long(str) {
    write(long_desc);
}

id(str)
{
    return str == name || str == alias || str == type;
}

test_type(str)
{
    if(str == type)
	return this_object();
    if(next)
	return call_other(next, "test_type", str);
    return 0;
}

query_type() { return type; }

query_value() { return value; }

query_worn() { return worn; }

query_name() { return name; }

query_magic() { return 1; }

get() { return 1; }

query_weight() { return weight; }

set_name(n) { name = n; }
set_short(s) { short_desc = s; long_desc = s + ".\n"; }
set_value(v) { value = v; }
set_weight(w) { weight = w; }
set_alias(a) { alias = a; }
set_long(l) { long_desc = l; }
set_type(t) { type = t; }
set_arm_light(l) { set_light(l); }
