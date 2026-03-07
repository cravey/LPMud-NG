special_reset() {
object potion;
int n;
int j;
        n = random(8);
        j = random(2);
    if (!present("potion")) {
        if (j == 0) {
        potion = clone_object("players/crimson/unknowna");
        } else
        if (j == 1) {
        potion = clone_object("players/crimson/unknown");
        }
        if (n == 0) {
        call_other(potion, "set_id", "brilliant red potion");
        } else
        if (n == 1) {
        call_other(potion, "set_id", "light blue potion");
        } else
        if (n == 2) {
        call_other(potion, "set_id", "light green potion");
        } else
        if (n == 3) {
        call_other(potion, "set_id", "black potion");
        } else
        if (n == 4) {
        call_other(potion, "set_id", "purple potion");
        } else
        if (n == 5) {
        call_other(potion, "set_id", "multi-colored potion");
        } else
        if (n == 6) {
        call_other(potion, "set_id", "orange potion");
        } else
        if (n == 7) {
        call_other(potion, "set_id", "grey potion");
        }
        if (n == 0) {
        call_other(potion, "set_short", "A brilliant red potion");
        } else
        if (n == 1) {
        call_other(potion, "set_short", "A light blue potion");
        } else
        if (n == 2) {
        call_other(potion, "set_short", "A light green potion");
        } else
        if (n == 3) {
        call_other(potion, "set_short", "A black potion");
        } else
        if (n == 4) {
        call_other(potion, "set_short", "A purple potion");
        } else
        if (n == 5) {
        call_other(potion, "set_short", "A multi-colored potion");
        } else
        if (n == 6) {
        call_other(potion, "set_short", "An orange potion");
        } else
        if (n == 7) {
        call_other(potion, "set_short", "A grey potion");
        }
        move_object(potion, this_object());
    }
}
