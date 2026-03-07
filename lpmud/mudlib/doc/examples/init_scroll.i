# 1 "doc/examples/init_scroll.c"









int hush_up;





init()
{

	add_action("read_rules", "rules");
	add_action("read_scroll", "read");
	add_action("hush", "quit");

}





reset(arg)
{
	if (arg)
		return;

	hush_up = 0;
	
}





id(str)
{
	return str == "scroll";
}





short()
{
	return "A scroll labeled 'READ ME!'";
}





long()
{

	write("The scroll is held rolled up with an blue and yellow band tied around its\n" +
		"middle. On the band the text 'READ ME!' is written in golden letters.\n");

}





get()
{
	return 1;
}





drop()
{
	if(!hush_up)
		write("Don't drop the scroll, it contains valuable information!\n");
	return 1;
}





query_auto_load()
{
	return "doc/examples/init_scroll:";
}





hush()
{
	hush_up = 1;
}





read_scroll()
{

	log_file("examples.scroll", capitalize(this_player()->query_real_name()) + " has read the scroll. " + ctime(time()) + "\n");
		
	this_player()->cat_file("/doc/examples/init_text");

	return 1;

}





read_rules()
{
	log_file("examples.scroll", capitalize(this_player()->query_real_name()) + " has read the RULES. " + ctime(time()) + "\n");
	this_player()->more("/doc/build/RULES");
	return 1;
}
