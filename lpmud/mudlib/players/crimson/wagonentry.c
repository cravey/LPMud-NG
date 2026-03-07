short() { return "A bright red wagon"; }

long() {
    write("There is a multi-colored sign on the wagon which reads:\n");
    write("      ****************************************\n");
    write("      *////////////////////\\\\\\\\\\\\\\\\\\*\n");
    write("      *\\\\\\\\\\\\\\\\\\\\//////////////////*\n");
    write("      *||||||||||||||||||||||||||||||||||||||*\n");
    write("      *||||||                          ||||||*\n");
    write("      *||||||    FORTUNES TOLD HERE    ||||||*\n");
    write("      *||||||                          ||||||*\n");
    write("      *||||||||||||||||||||||||||||||||||||||*\n");
    write("      *////////////////////\\\\\\\\\\\\\\\\\\*\n");
    write("      *\\\\\\\\\\\\\\\\\\\\//////////////////*\n");
    write("      ****************************************\n");
    return 1;
}

id(str) {
    return str == "wagon" || str == "gypsy wagon" || str == "fortune wagon";
}

get() { return 0; }
