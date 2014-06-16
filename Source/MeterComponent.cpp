#include "base.h"
#include "MeterComponent.h"

float const MeterComponent::minDecibels = -60.0f;
float const MeterComponent::maxDecibels = 0.0f;
float const MeterComponent::marginFloat = 4.0f;
int const MeterComponent::marginInt = int(marginFloat);

MeterComponent::MeterComponent() :
	currentDecibels(-FLT_MAX)
{
	//currentDecibels = Random::getSystemRandom().nextFloat() * minDecibels;
}

MeterComponent::~MeterComponent()
{  
}

void MeterComponent::paint(Graphics &g)
{
	int x = dBToX(currentDecibels);
	int h = getHeight();
	int w = getWidth();

	if (x < marginInt)
	{
		g.drawImageAt(off, 0, 0, false);
		return;
	}

	g.drawImage(on, 0, 0, x, h,
		0, 0, x, h, false);
	g.drawImage(off, x, 0, w - x, h,
		x, 0, w - x, h, false);
}

void MeterComponent::resized()
{
	if (off.getWidth() != getWidth())
	{
		drawOffImage();
	}

	if (on.getWidth() != getWidth())
	{
		drawOnImage();
	}
}

void MeterComponent::drawOffImage()
{
	Image canvas(Image::ARGB, getWidth(), getHeight(), true);
	Graphics g(canvas);
	g.fillAll(Colours::transparentWhite);

	g.setColour(Colours::black);
	juce::Rectangle<float> r(getLocalBounds().toFloat());
	g.fillRoundedRectangle(r, getHeight() * 0.35f);

	drawScale(canvas, Colours::white);

	off = canvas;
}

void MeterComponent::drawOnImage()
{
	Image canvas(Image::ARGB, getWidth(), getHeight(), true);
	Graphics g(canvas);
	g.fillAll(Colours::transparentWhite);

	juce::Rectangle<float> r(getLocalBounds().toFloat());
	g.fillRoundedRectangle(r, getHeight() * 0.25f);

	ColourGradient gradient(Colours::limegreen.withAlpha(0.5f), 0.0f, 0.0f,
		Colours::red, (float)getWidth(), 0.0f, false);
	gradient.addColour( dBToPosition(-12.0f), Colours::limegreen);
	gradient.addColour( dBToPosition(-6.0f), Colours::yellow);
	g.setGradientFill(gradient);
	r.reduce(marginFloat, marginFloat);
	g.fillRoundedRectangle(r, getHeight() * 0.25f);

	drawScale(canvas, Colours::white);

	on = canvas;
}

void MeterComponent::drawScale(Image &image, Colour c)
{
	Graphics g(image);
	Font f(g.getCurrentFont());
	f.setBold(true);
	g.setColour(c);
	g.setFont(f);

	for (float dB = -50.0f; dB < maxDecibels; dB += 10.0f)
	{
		int x = dBToX(dB);
		String text(dB);
		int w = f.getStringWidth(text);
		x -= w/2;
		g.drawText( text, x, 0, w, image.getHeight(), Justification::centred, false);
	}
}

float MeterComponent::dBToPosition(float dB)
{
	return (dB - minDecibels) / (maxDecibels - minDecibels);
}

int MeterComponent::dBToX(float dB)
{
	float pos = dBToPosition(dB);
	pos *= getWidth() - marginFloat * 2;
	return roundFloatToInt(pos) + marginInt;
}

void MeterComponent::setDecibels( float dB )
{
	if (currentDecibels != dB)
	{
		currentDecibels = dB;
		repaint();
	}
}

void MeterComponent::setLinear( float linear )
{
	linear = jmax(linear, -FLT_MIN);
	setDecibels(20.0f * log10(linear));
}

