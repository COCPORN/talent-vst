#pragma once

#include <JuceHeader.h>

using namespace juce;

struct LyricLine : public juce::Component {


	LyricLine(const char* lyricLine) {
		lyricLine_ = lyricLine;
	}

	void paint(Graphics& g) override {
		g.setFont(Font(46.0f));
		g.setColour(Colours::white);
		g.drawText(lyricLine_, getLocalBounds(), Justification::centred);
	}

private:
	const char* lyricLine_;
};

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

	void clearLyrics();
	/*void setLyrics();*/

private:
    //==============================================================================
    // Your private member variables go here...
	OwnedArray<LyricLine> lyricLines_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
