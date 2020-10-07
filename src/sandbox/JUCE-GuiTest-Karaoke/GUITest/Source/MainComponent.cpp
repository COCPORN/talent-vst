#include "MainComponent.h"

using namespace juce;

//==============================================================================
MainComponent::MainComponent()
{
	setSize(1200, 800);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

	auto height = getLocalBounds().getHeight();
	auto width = getLocalBounds().getWidth();

	auto lyrics = { "There's a place I like to go", "Everybody knows", "Leads me to temptootion" };

	auto i = 0;

	auto segmentHeight = height / lyrics.size();

	for (auto lyric : lyrics) {
		// Render the grey parts first (the ones that we have not yet gotten to)
		auto backGlyphArrangement = GlyphArrangement();

		auto top = i * segmentHeight;
		backGlyphArrangement.addFittedText({ 46.0f }, lyric,
			0,
			top,
			width,
			segmentHeight,
			Justification::centred, 1, 1.0f);
		i++;

		auto bounds = backGlyphArrangement.getBoundingBox(0, backGlyphArrangement.getNumGlyphs(), true);

		g.setColour(Colours::grey);

		backGlyphArrangement.draw(g);

		// Just testing to break in the first line
		if (i != 1) continue;

		// Render the white parts of the text
		auto glyphArrangement = GlyphArrangement();

		glyphArrangement.addFittedText({ 46.0f }, "There's a place",
			bounds.getX(),
			top,
			width - bounds.getX(),
			segmentHeight,
			Justification::centredLeft, 1, 1.0f);

		g.setColour(Colours::white);

		glyphArrangement.draw(g);
	}

}

void MainComponent::resized() {

}
