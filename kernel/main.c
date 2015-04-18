extern goto_ring3();

void main() {
    char *video = (char*)0xb8000;
    video[0] = 'K';
    video[1] = 0x1f;
    video[2] = 'e';
    video[3] = 0x1f;
    video[4] = 'r';
    video[5] = 0x1f;
    video[6] = 'n';
    video[7] = 0x1f;
    video[8] = 'e';
    video[9] = 0x1f;
    video[10] = 'l';
    video[11] = 0x1f;

    goto_ring3();
    while (1) {}
}
