#pragma once

class MeterComponent : public Component
{
public:
	MeterComponent();
	~MeterComponent();

	void paint(Graphics &g);
	void resized();

	void setLinear(float linear);
	void setDecibels(float dB);
	float dBToPosition(float dB);
	int dBToX(float dB);

protected:
	void drawOffImage();
	void drawOnImage();
	void drawScale(Image &image, Colour c);
	Image off;
	Image on;

	float currentDecibels;

	static float const minDecibels;
	static float const maxDecibels;
	static float const marginFloat;
	static int const marginInt;
};
