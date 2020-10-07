#include "MainComponent.h"

using namespace juce;

//==============================================================================
MainComponent::MainComponent()
{
	auto lyrics = { "There's a place I like to go", "Everybody knows", "Leads me to temptation" };

	for (auto lyric : lyrics) {
		auto lyricComponent = new LyricLine(lyric);

		addAndMakeVisible(lyricLines_.add(lyricComponent));
	}

	

    setSize (1200, 800);
}

MainComponent::~MainComponent()
{
}


void MainComponent::clearLyrics() {
	lyricLines_.clear();
}


//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

	auto height = getLocalBounds().getHeight();
	auto width = getLocalBounds().getWidth();

	auto horizontalMargins = 200;

	const auto margins = height / 7;
	const auto line_spacing = height / 10;

	auto lyrics = { "There's a place I like to go", "Everybody knows", "Leads me to temptation" };

	auto i = 0;	

	
	
	

	auto segmentHeight = height / lyrics.size();

	for (auto lyric : lyrics) {
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

		if (i != 1) continue;

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

	

	// (Our component is opaque, so we must completely fill the background with a solid colour)
	
}

void MainComponent::resized() {

	//Grid grid;

	//grid.rowGap = 20_px;
	//grid.columnGap = 20_px;

	//using Track = Grid::TrackInfo;

	//grid.autoColumns = Track(1_fr);
	//grid.autoRows = Track(1_fr);

	//grid.autoFlow = Grid::AutoFlow::row;	

	//for (auto lyricLine : lyricLines_) {		
	//	
	//	grid.items.add(lyricLine);
	//}
	//grid.performLayout(getLocalBounds());
    
}
