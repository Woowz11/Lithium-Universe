#ifndef FONT_CHAR_H
#define FONT_CHAR_H

class FontChar {
public:
	int Char;
	int X = -1;
	int Y = -1;
	double W = -1;

	FontChar() { Char = -1; }

	FontChar(int Char_) {
		Char = Char_;
	}
};

#endif