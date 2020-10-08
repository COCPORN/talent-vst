/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class TestVstAudioProcessor : 
	public juce::AudioProcessor, 
	public juce::ChangeBroadcaster
{
public:
    //==============================================================================
    TestVstAudioProcessor();
    ~TestVstAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

	class JUCE_API Listener {
	public:
		virtual ~Listener() = default;
		virtual void lyricsChanged(TestVstAudioProcessor* processor) = 0;
		virtual void pageChanged(TestVstAudioProcessor* processor) = 0;
	};

	void setSong(std::map<int, std::vector<std::string> > & song);
	std::map<int, std::vector<std::string> >& getSong();

	std::vector<std::string>& getCurrentLyrics();
	void setLyrics(std::vector<std::string> &lyrics);
	int getPage();
	void setPage(int page);
	int getProgress();
	void setProgress(int progress);
	bool getDirtyAndUpdate();
	juce::CriticalSection& getDataUpdateCriticalSection();

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================	
	bool dirty_;
	bool latched_;
	
	int page_;
	std::map<int, std::vector<std::string> > song_;
	std::vector<std::string> currentLyrics_;
	int lastControlValue_;
	int progress_;
	juce::CriticalSection dataUpdateLock_;
	int numMidiEvents_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestVstAudioProcessor)
};
