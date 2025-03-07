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
    : AudioProcessorEditor (&p), processor_(p), lyrics_(0), page_(0), progress_(0), activeLine_(0)
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

	lyrics_ = processor_.getCurrentLyrics();
	page_ = processor_.getPage();
	progress_ = processor_.getProgress();
	activeLine_ = processor_.getActiveLine();

	if (activeLine_ > lyrics_.size()) {
		activeLine_ = 0;
	}

	repaint();
}

// Extra weight to add to the line, so that there is a gap in the CC progression
// Should be grabbed from config
const int lineWeight = 10;

int totalCharacters(std::vector<std::string> lyrics) {
	int r_total = 0;
	for (auto lyric : lyrics) {
		r_total += lyric.length() + lineWeight;
	}
	return r_total;
}

int numWhiteCharacters(int totalCharacters, int progress) {
	// Make it easier to automate, anything over CC 100 is "full progress"
	if (progress > 100) {
		progress = 100;
	}
	auto retval = totalCharacters * ((float)progress / (float)100);

	return retval;
}

std::vector<std::string> getWhiteCharacters(std::vector<std::string>& originalLyrics, 
	int numWhiteCharacters) 
{
	std::vector<std::string> r_whiteCharacters;

	if (numWhiteCharacters == 0) {
		return r_whiteCharacters;
	}

	std::vector<std::string> weightedLyrics;

	// Add lineweight
	for (const auto lyric : originalLyrics) {
		weightedLyrics.push_back(lyric + std::string(lineWeight, ' '));
	}

	auto charactersLeft = numWhiteCharacters;

	for (auto lyric : weightedLyrics) {
		if (lyric.length() < charactersLeft) {
			r_whiteCharacters.push_back(lyric.substr(0, lyric.length() - lineWeight));
			charactersLeft -= lyric.length();
		}
		else if (lyric.length() == charactersLeft) {
			r_whiteCharacters.push_back(lyric.substr(0, lyric.length() - lineWeight));
			return r_whiteCharacters;
		}
		else {
			r_whiteCharacters.push_back(lyric.substr(0, charactersLeft));
			return r_whiteCharacters;
		}
	}
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

	auto whiteLyrics = getWhiteCharacters(lyrics_, numWhiteCharacters(totalCharacters(lyrics_), progress_));

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
		
		auto bounds = backGlyphArrangement.getBoundingBox(0, backGlyphArrangement.getNumGlyphs(), true);

		if (activeLine_ == 0 || i != activeLine_ - 1) {
			g.setColour(Colours::grey);			
		}
		else {
			g.setColour(Colours::lightsteelblue);
		}

		backGlyphArrangement.draw(g);					

		if (whiteLyrics.size() <= i) {
			i++;
			continue;
		}

		// Render the white parts of the text
		auto glyphArrangement = GlyphArrangement();

		glyphArrangement.addFittedText({ 46.0f }, whiteLyrics[i],
			bounds.getX(),
			top,
			width - bounds.getX(),
			segmentHeight,
			Justification::centredLeft, 1, 1.0f);

		g.setColour(Colours::white);

		glyphArrangement.draw(g);
		i++;
	}
}

void TestVstAudioProcessorEditor::resized()
{
	//repaint();
}
