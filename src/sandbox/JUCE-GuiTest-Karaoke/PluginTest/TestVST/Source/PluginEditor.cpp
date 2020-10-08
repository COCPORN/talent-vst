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

// OK, so somewhat of a messy code, but it might work:
// All data updated in the processor is set protected by the same critical section
// as is blocked on in this method => Should lead to no race.
// All semantics are copy => Should lead to no leak.
// NOTE: This does NOT work with multiple editors. We should consider creating IPC 
// with local sockets to remedy that, it would make this whole thing less dubious
void TestVstAudioProcessorEditor::timerCallback() {
	juce::CriticalSection& criticalSection = processor_.getDataUpdateCriticalSection();	
	juce::ScopedLock dataUpdate(criticalSection);
	auto dirty = processor_.getDirtyAndUpdate();

	if (dirty == false) {
		return;
	}

	auto lyrics = processor_.getCurrentLyrics();
	auto page = processor_.getPage();
	auto progress = processor_.getProgress();
	
	lyrics_ = lyrics;
	progress_ = progress;
	page_ = page;	

	repaint();
}

//==============================================================================
void TestVstAudioProcessorEditor::paint (juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));


	auto height = getLocalBounds().getHeight();
	auto width = getLocalBounds().getWidth();

	if (lyrics_.size() == 0)
	{
		return;
	}

	auto i = 0;

	auto segmentHeight = height / lyrics_.size();

	for (auto lyric : lyrics_) {
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
	//repaint();
}
