#pragma once
//#include "config.h"
#include "string.h"
#include "libraries/TFT_eSPI/TFT_eSPI.h"
class TextButton {
	private:
		int x;
		int y;
		int width;
		int height;
		int textSize;
		String text;
		uint32_t bgColor;
		uint32_t fgColor;
	public:
		TextButton(int x, int y, int width, int height, int textSize, String text, uint32_t bgColor, uint32_t fgColor);
		void setText(String text);
		void setColor(uint32_t fgColor,uint32_t bgColor);
		void draw(TFT_eSPI* tft);
		bool inBounds(int x, int y);
};

class Button {
	private:

		uint32_t bgColor;
		uint32_t fgColor;
	public:
		int x;
		int y;
		int width;
		int height;
		Button(int x,int y, int width, int height, uint32_t bgColor, uint32_t fgColor);
		void setColor(uint32_t fgColor, uint32_t bgColor);
		void draw(TFT_eSPI* tft);
		bool inBounds(int x, int y);
};

void drawText(TFT_eSPI* tft,int x, int y, int size, String text, uint32_t fgColor, uint32_t bgColor);