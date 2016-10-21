void init() {
    char *video = (char*) 0xffff800000000000UL + 0xb8000;
    video[0] = 'H';
    video[1] = 0x4e;
    while (1) {}
}
