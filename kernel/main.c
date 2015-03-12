void kmain() {
	char *video = (char*) 0xb8000;
	video[0] = 'h';
	video[1] = 0x1f;
	video[2] = 'e';
	video[3] = 0x1f;
	video[4] = 'l';
	video[5] = 0x1f;
	video[6] = 'l';
	video[7] = 0x1f;
	video[8] = 'o';
	video[9] = 0x1f;
}
