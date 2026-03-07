int cnt;

id(str) {
    return str=="strength spell" || str=="glow" || str=="hands";
}

reset() {
    cnt=100;
    set_heart_beat(1);
}

query_weight() { return 0;}

short() {
    return "A violet glow surrounding the hands";
}

drop() {
    return 1;
}

heart_beat() {
    cnt -= 1;
    if (cnt<=0) {
        say("The violet glow fades away.\n");
        destruct(this_object());
    }
}
    
