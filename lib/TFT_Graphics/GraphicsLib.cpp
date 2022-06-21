#include "GraphicsLib.h"
TextButton::TextButton(int x, int y, int width, int height, int textSize , String text, uint32_t bgColor, uint32_t fgColor) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->textSize = textSize;
	this->text = text;
	this->bgColor = bgColor;
	this->fgColor = fgColor;
}

void TextButton::setText(String text) {
	this->text = text;
}

void TextButton::setColor(uint32_t fgColor, uint32_t bgColor) {
	this->fgColor = fgColor;
	this->bgColor = bgColor;
}

void TextButton::draw(TFT_eSPI* tft) {
	tft->fillRect(x,y,width,height,bgColor);
	tft->drawRect(x,y,width,height,fgColor);
	tft->setTextDatum(CC_DATUM);
	tft->setTextSize(textSize);
	tft->setTextColor(fgColor);
	tft->drawString(text,x+width/2,y+height/2);
}

bool TextButton::inBounds(int pX, int pY) {
	return (pX > x && pX < (x+width)) && (pY > y && pY < (y+height));
}

Button::Button(int x, int y, int width, int height,uint32_t bgColor, uint32_t fgColor) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->bgColor = bgColor;
	this->fgColor = fgColor;
}

void Button::setColor(uint32_t fgColor, uint32_t bgColor) {
	this->fgColor = fgColor;
	this->bgColor = bgColor;
}

void Button::draw(TFT_eSPI* tft) {
	//ESP_LOGI("TimerProgram","X:%i Y:%i W:%i H:%i",x,y,width,height);
	tft->fillRect(x,y,width,height,bgColor);
	tft->drawRect(x,y,width,height,fgColor);
}

bool Button::inBounds(int pX, int pY) {
	return (pX > x && pX < (x+width)) && (pY > y && pY < (y+height));
}

void drawText(TFT_eSPI* tft,int x, int y, int size, String text, uint32_t fgColor, uint32_t bgColor) {
	tft->setTextSize(size);
	tft->setTextDatum(CC_DATUM);
	if(bgColor != 478932) {
		tft->setTextColor(fgColor,bgColor);
	} else {
		tft->setTextColor(fgColor);
	}
	tft->drawString(text,x,y);
}