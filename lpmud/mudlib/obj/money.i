# 1 "obj/money.c"
int money;

reset(arg) {
    if (arg)
	return;
    money = 1;
}

query_weight() { return 0; }

short() {
    if (money == 0)
	return 0;
    return money + " gold coins";
}








init()
{
  if (environment(this_object())==this_player()) {
    call_other(this_player(), "add_money", money);
    money = 0;
    set_heart_beat(1);
  }
}

get()
{
  return money>0;
}

set_money(m) {
    money = m;
}

id(str) {
    if (str == "coins")
	return 1;
    if (str == "money")
	return 1;
}

heart_beat() {
    if (money == 0)
	destruct(this_object());
}
