const unsigned char font[1024] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x06, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 
	0x1e, 0x0c, 0x1e, 0x3f, 0x18, 0x3f, 0x1e, 0x3f, 0x1e, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x18, 0x66, 0x66, 0x7c, 0x66, 0x6c, 0x18, 0x70, 0x0e, 0x66, 0x18, 0x00, 0x00, 0x00, 0x60, 
	0x3c, 0x18, 0x3c, 0x7e, 0x30, 0x7e, 0x3c, 0x7e, 0x3c, 0x3c, 0x00, 0x00, 0x30, 0x00, 0x0c, 0x3c, 
	0x3c, 0x18, 0x3e, 0x3c, 0x1e, 0x7e, 0x7e, 0x7c, 0x66, 0x7e, 0x60, 0x66, 0x06, 0xc6, 0x66, 0x3c, 
	0x3e, 0x3c, 0x3e, 0x3c, 0x7e, 0x66, 0x66, 0xc6, 0x66, 0x66, 0x7e, 0x78, 0x02, 0x1e, 0x10, 0x00, 
	0x18, 0x00, 0x06, 0x00, 0x60, 0x00, 0x70, 0x00, 0x06, 0x18, 0x60, 0x06, 0x1c, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 
	0x33, 0x0e, 0x33, 0x18, 0x1c, 0x03, 0x03, 0x30, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x18, 0x66, 0xff, 0x06, 0x36, 0x38, 0x18, 0x38, 0x1c, 0x3c, 0x18, 0x00, 0x00, 0x00, 0x30, 
	0x66, 0x1c, 0x66, 0x30, 0x38, 0x06, 0x06, 0x60, 0x66, 0x66, 0x18, 0x18, 0x18, 0x7e, 0x18, 0x66, 
	0x66, 0x3c, 0x66, 0x66, 0x36, 0x06, 0x06, 0x06, 0x66, 0x18, 0x60, 0x36, 0x06, 0xee, 0x6e, 0x66, 
	0x66, 0x66, 0x66, 0x06, 0x18, 0x66, 0x66, 0xc6, 0x66, 0x66, 0x30, 0x18, 0x06, 0x18, 0x38, 0x00, 
	0x3c, 0x3c, 0x06, 0x3c, 0x60, 0x3c, 0x18, 0x7c, 0x06, 0x00, 0x00, 0x06, 0x18, 0x66, 0x3e, 0x3c, 
	0x3e, 0x7c, 0x3e, 0x7c, 0x7e, 0x66, 0x66, 0xc6, 0x66, 0x66, 0x7e, 0x3c, 0x18, 0x00, 0x00, 0x00, 
	0x33, 0x0c, 0x18, 0x0c, 0x1e, 0x1f, 0x1f, 0x18, 0x1e, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x18, 0x66, 0x66, 0x3c, 0x18, 0x1c, 0x18, 0x18, 0x18, 0xff, 0x7e, 0x00, 0x7e, 0x00, 0x18, 
	0x66, 0x18, 0x30, 0x18, 0x3c, 0x3e, 0x3e, 0x30, 0x3c, 0x7c, 0x18, 0x18, 0x0c, 0x00, 0x30, 0x30, 
	0x76, 0x66, 0x3e, 0x06, 0x66, 0x3e, 0x3e, 0x06, 0x7e, 0x18, 0x60, 0x1e, 0x06, 0xfe, 0x7e, 0x66, 
	0x66, 0x66, 0x66, 0x3c, 0x18, 0x66, 0x66, 0xd6, 0x3c, 0x3c, 0x18, 0x18, 0x0c, 0x18, 0x6c, 0x00, 
	0x7e, 0x60, 0x3e, 0x06, 0x7c, 0x66, 0x7c, 0x66, 0x3e, 0x1c, 0x60, 0x36, 0x18, 0xfe, 0x66, 0x66, 
	0x66, 0x66, 0x66, 0x06, 0x18, 0x66, 0x66, 0xd6, 0x3c, 0x66, 0x30, 0x7e, 0x18, 0x00, 0x00, 0x00, 
	0x33, 0x0c, 0x0c, 0x18, 0x1b, 0x30, 0x33, 0x0c, 0x33, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x18, 0x00, 0x66, 0x60, 0x0c, 0xf6, 0x00, 0x18, 0x18, 0x3c, 0x18, 0x00, 0x00, 0x00, 0x0c, 
	0x66, 0x18, 0x18, 0x30, 0x36, 0x60, 0x66, 0x18, 0x66, 0x60, 0x00, 0x00, 0x18, 0x00, 0x18, 0x18, 
	0x76, 0x66, 0x66, 0x06, 0x66, 0x06, 0x06, 0x76, 0x66, 0x18, 0x60, 0x1e, 0x06, 0xd6, 0x7e, 0x66, 
	0x3e, 0x66, 0x3e, 0x60, 0x18, 0x66, 0x66, 0xfe, 0x3c, 0x18, 0x0c, 0x18, 0x18, 0x18, 0xc6, 0x00, 
	0x7e, 0x7c, 0x66, 0x06, 0x66, 0x7e, 0x18, 0x66, 0x66, 0x18, 0x60, 0x1e, 0x18, 0xfe, 0x66, 0x66, 
	0x66, 0x66, 0x06, 0x3c, 0x18, 0x66, 0x66, 0xfe, 0x18, 0x66, 0x18, 0x7e, 0x18, 0x00, 0x00, 0x00, 
	0x33, 0x0c, 0x06, 0x33, 0x3f, 0x33, 0x33, 0x06, 0x33, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xff, 0x3e, 0x66, 0x66, 0x00, 0x38, 0x1c, 0x66, 0x18, 0x18, 0x00, 0x18, 0x06, 
	0x66, 0x18, 0x0c, 0x66, 0x7e, 0x66, 0x66, 0x0c, 0x66, 0x30, 0x18, 0x18, 0x30, 0x7e, 0x0c, 0x00, 
	0x06, 0x7e, 0x66, 0x66, 0x36, 0x06, 0x06, 0x66, 0x66, 0x18, 0x66, 0x36, 0x06, 0xc6, 0x76, 0x66, 
	0x06, 0x36, 0x36, 0x60, 0x18, 0x66, 0x3c, 0xee, 0x66, 0x18, 0x06, 0x18, 0x30, 0x18, 0x00, 0x00, 
	0x3c, 0x66, 0x66, 0x06, 0x66, 0x06, 0x18, 0x7c, 0x66, 0x18, 0x60, 0x36, 0x18, 0xd6, 0x66, 0x66, 
	0x3e, 0x7c, 0x06, 0x60, 0x18, 0x66, 0x3c, 0x7c, 0x3c, 0x7c, 0x0c, 0x18, 0x18, 0x00, 0x00, 0x00, 
	0x1e, 0x3f, 0x3f, 0x1e, 0x18, 0x1e, 0x1e, 0x06, 0x1e, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x18, 0x00, 0x66, 0x18, 0x62, 0xdc, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x18, 0x00, 0x18, 0x02, 
	0x3c, 0x7e, 0x7e, 0x3c, 0x30, 0x3c, 0x3c, 0x0c, 0x3c, 0x1c, 0x18, 0x18, 0x60, 0x00, 0x06, 0x18, 
	0x7c, 0x66, 0x3e, 0x3c, 0x1e, 0x7e, 0x06, 0x7c, 0x66, 0x7e, 0x3c, 0x66, 0x7e, 0xc6, 0x66, 0x3c, 
	0x06, 0x6c, 0x66, 0x3c, 0x18, 0x7e, 0x18, 0xc6, 0x66, 0x18, 0x7e, 0x78, 0x60, 0x1e, 0x00, 0xff, 
	0x18, 0x7c, 0x3e, 0x3c, 0x7c, 0x3c, 0x18, 0x60, 0x66, 0x3c, 0x60, 0x66, 0x3c, 0xc6, 0x66, 0x3c, 
	0x06, 0x60, 0x06, 0x3e, 0x70, 0x7c, 0x18, 0x6c, 0x66, 0x30, 0x7e, 0x3c, 0x18, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x06, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00
};
