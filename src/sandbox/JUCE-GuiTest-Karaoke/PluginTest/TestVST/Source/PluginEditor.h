/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class TestVstAudioProcessorEditor  : public juce::AudioProcessorEditor, 
	private juce::Timer
{
public:
    TestVstAudioProcessorEditor (TestVstAudioProcessor&);
    ~TestVstAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

	//void lyricsChanged(TestVstAudioProcessor* processor) override;
	//void pageChanged(TestVstAudioProcessor* processor) override;

	void timerCallback() override;
	

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TestVstAudioProcessor& processor_;

	std::vector<std::string> lyrics_;
	int page_;
	int progress_;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestVstAudioProcessorEditor)
};
