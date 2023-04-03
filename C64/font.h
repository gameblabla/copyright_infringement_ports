const unsigned char font[640] = {
	0x05, 0x16, 0x16, 0x16, 0x16, 0x16, 0x05, 0x00, 0x40, 0x50, 0x50, 0x50, 0x50, 0x50, 0x40, 0x00, 
	0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 
	0x05, 0x16, 0x00, 0x05, 0x14, 0x16, 0x15, 0x00, 0x40, 0x50, 0x50, 0x40, 0x00, 0x50, 0x50, 0x00, 
	0x05, 0x16, 0x00, 0x05, 0x00, 0x16, 0x05, 0x00, 0x40, 0x50, 0x50, 0x40, 0x50, 0x50, 0x40, 0x00, 
	0x16, 0x16, 0x16, 0x15, 0x00, 0x00, 0x00, 0x00, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x00, 
	0x15, 0x16, 0x14, 0x15, 0x00, 0x16, 0x05, 0x00, 0x50, 0x50, 0x00, 0x40, 0x50, 0x50, 0x40, 0x00, 
	0x05, 0x16, 0x14, 0x15, 0x16, 0x16, 0x05, 0x00, 0x40, 0x50, 0x00, 0x40, 0x50, 0x50, 0x40, 0x00, 
	0x15, 0x16, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x00, 
	0x05, 0x16, 0x16, 0x05, 0x16, 0x16, 0x05, 0x00, 0x40, 0x50, 0x50, 0x40, 0x50, 0x50, 0x40, 0x00, 
	0x05, 0x16, 0x16, 0x05, 0x00, 0x16, 0x05, 0x00, 0x40, 0x50, 0x50, 0x50, 0x50, 0x50, 0x40, 0x00, 
	0x05, 0x14, 0x14, 0x15, 0x14, 0x14, 0x14, 0x00, 0x50, 0x14, 0x14, 0x54, 0x14, 0x14, 0x14, 0x00, 
	0x15, 0x14, 0x14, 0x15, 0x14, 0x14, 0x15, 0x00, 0x50, 0x14, 0x14, 0x50, 0x14, 0x14, 0x50, 0x00, 
	0x05, 0x14, 0x14, 0x14, 0x14, 0x14, 0x05, 0x00, 0x50, 0x14, 0x00, 0x00, 0x00, 0x14, 0x50, 0x00, 
	0x15, 0x16, 0x14, 0x14, 0x14, 0x16, 0x15, 0x00, 0x40, 0x50, 0x14, 0x14, 0x14, 0x50, 0x40, 0x00, 
	0x15, 0x14, 0x14, 0x15, 0x14, 0x14, 0x15, 0x00, 0x54, 0x00, 0x00, 0x54, 0x00, 0x00, 0x54, 0x00, 
	0x15, 0x14, 0x14, 0x15, 0x14, 0x14, 0x14, 0x00, 0x54, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 
	0x05, 0x14, 0x14, 0x16, 0x14, 0x14, 0x05, 0x00, 0x50, 0x14, 0x00, 0x54, 0x14, 0x14, 0x50, 0x00, 
	0x14, 0x14, 0x14, 0x15, 0x14, 0x14, 0x14, 0x00, 0x14, 0x14, 0x14, 0x54, 0x14, 0x14, 0x14, 0x00, 
	0x15, 0x01, 0x01, 0x01, 0x01, 0x01, 0x15, 0x00, 0x54, 0x40, 0x40, 0x40, 0x40, 0x40, 0x54, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x14, 0x14, 0x05, 0x00, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x50, 0x00, 
	0x14, 0x16, 0x15, 0x15, 0x15, 0x16, 0x14, 0x00, 0x14, 0x50, 0x40, 0x00, 0x40, 0x50, 0x14, 0x00, 
	0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x00, 
	0x56, 0x55, 0x59, 0x59, 0x50, 0x50, 0x50, 0x00, 0x50, 0x54, 0x94, 0x94, 0x14, 0x14, 0x14, 0x00, 
	0x14, 0x15, 0x15, 0x15, 0x16, 0x14, 0x14, 0x00, 0x14, 0x94, 0x54, 0x54, 0x54, 0x14, 0x14, 0x00, 
	0x05, 0x14, 0x14, 0x14, 0x14, 0x14, 0x05, 0x00, 0x50, 0x14, 0x14, 0x14, 0x14, 0x14, 0x50, 0x00, 
	0x15, 0x14, 0x14, 0x15, 0x14, 0x14, 0x14, 0x00, 0x50, 0x14, 0x14, 0x50, 0x00, 0x00, 0x00, 0x00, 
	0x05, 0x14, 0x14, 0x14, 0x14, 0x16, 0x05, 0x00, 0x50, 0x14, 0x14, 0x14, 0x14, 0x50, 0x54, 0x00, 
	0x15, 0x14, 0x14, 0x15, 0x15, 0x16, 0x14, 0x00, 0x50, 0x14, 0x14, 0x50, 0x40, 0x50, 0x14, 0x00, 
	0x05, 0x14, 0x14, 0x05, 0x00, 0x14, 0x05, 0x00, 0x50, 0x14, 0x00, 0x50, 0x14, 0x14, 0x50, 0x00, 
	0x15, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x54, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 
	0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x05, 0x00, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x50, 0x00, 
	0x14, 0x14, 0x14, 0x14, 0x05, 0x05, 0x01, 0x00, 0x16, 0x16, 0x16, 0x16, 0x50, 0x50, 0x40, 0x00, 
	0x50, 0x50, 0x59, 0x59, 0x59, 0x55, 0x16, 0x00, 0x14, 0x14, 0x94, 0x94, 0x94, 0x54, 0x50, 0x00, 
	0x14, 0x14, 0x05, 0x01, 0x05, 0x14, 0x14, 0x00, 0x14, 0x14, 0x50, 0x40, 0x50, 0x14, 0x14, 0x00, 
	0x14, 0x14, 0x14, 0x05, 0x01, 0x01, 0x01, 0x00, 0x14, 0x14, 0x14, 0x50, 0x40, 0x40, 0x40, 0x00, 
	0x15, 0x00, 0x00, 0x01, 0x05, 0x14, 0x15, 0x00, 0x54, 0x14, 0x50, 0x40, 0x00, 0x00, 0x54, 0x00, 
	0x05, 0x05, 0x05, 0x05, 0x05, 0x00, 0x05, 0x00, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x01, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x05, 0x05, 0x00, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x05, 0x16, 0x00, 0x01, 0x05, 0x00, 0x05, 0x00, 0x40, 0x50, 0x50, 0x40, 0x00, 0x00, 0x00, 0x00
};

const int font_length = 640;
