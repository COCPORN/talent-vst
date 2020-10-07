/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

//==============================================================================
TestVstAudioProcessorEditor::TestVstAudioProcessorEditor (TestVstAudioProcessor& p)
    : AudioProcessorEditor (&p), processor_(p), lyrics_(0), page_(0), progress_(0)
{		
	setResizable(true, true);
	startTimerHz(60);
    setSize (1200, 800);
}

TestVstAudioProcessorEditor::~TestVstAudioProcessorEditor()
{
	/*processor_->removeListener(this);*/
}


//void TestVstAudioProcessorEditor::lyricsChanged(TestVstAudioProcessor* processor) {
//	lyrics_ = processor->getLyrics();
//
//}
//
//void TestVstAudioProcessorEditor::pageChanged(TestVstAudioProcessor* processor) {
//
//}


void TestVstAudioProcessorEditor::timerCallback() {
	auto lyrics = processor_.getLyrics();
	auto page = processor_.getPage();
	auto progress = processor_.getProgress();

	auto dirty = false;

	if (lyrics_ != lyrics) {
		lyrics_ = lyrics;
		dirty = true;
	}

	if (page_ != page) {
		page_ = page;
		dirty = true;
	}

	if (progress_ != progress) {
		progress_ = progress;
		dirty = true;
	}
	
	if (dirty == true) {
		repaint();
	}

}

//==============================================================================
void TestVstAudioProcessorEditor::paint (juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));


	if (lyrics_ == NULL) {
		return;
	}

	auto height = getLocalBounds().getHeight();
	auto width = getLocalBounds().getWidth();


	auto i = 0;

	auto segmentHeight = height / lyrics_->size();

	for (auto lyric : *lyrics_) {
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
		//auto glyphArrangement = GlyphArrangement();

		//glyphArrangement.addFittedText({ 46.0f }, "There's a place",
		//	bounds.getX(),
		//	top,
		//	width - bounds.getX(),
		//	segmentHeight,
		//	Justification::centredLeft, 1, 1.0f);

		//g.setColour(Colours::white);

		//glyphArrangement.draw(g);
	}
}

void TestVstAudioProcessorEditor::resized()
{
	repaint();
}
