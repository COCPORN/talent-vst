/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TestVstAudioProcessor::TestVstAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	), page_(0), currentLyrics_(0), progress_(0), numMidiEvents_(0), latched_(false), lastControlValue_(0)
#endif
{
	std::vector<std::string> firstPage = { "There's a place I like to go", "Everybody knows", "Leads me to temptation" };
	std::vector<std::string> secondPage = { "When all eyes are trained on me", "And they see what they want to see", "Single beat across the nation" };
	std::vector<std::string> thirdPage = { "And you show me what to do", "And you show me what to do" , "And you show me what to do" , "And you show me what to do" };
	std::map<int, std::vector<std::string> > song = {
		{ 1, firstPage },
		{ 2, secondPage },
		{ 3, thirdPage }
	};
	setSong(song);
}

TestVstAudioProcessor::~TestVstAudioProcessor()
{
}

//void TestVstAudioProcessor::addListener(Listener* listener) {
//	this->addChangeListener(listener);
//}
//
//void TestVstAudioProcessor::removeListener(Listener* listener) {
//	this->removeChangeListener(listener);
//}

std::vector<std::string>& TestVstAudioProcessor::getCurrentLyrics() {
	return currentLyrics_;
}

bool TestVstAudioProcessor::getDirtyAndUpdate() {
	juce::ScopedLock dirty(dataUpdateLock_);
	auto r_dirty = dirty_;
	dirty_ = false;
	return r_dirty;
}

void TestVstAudioProcessor::setSong(std::map<int, std::vector<std::string> >& song) {
	juce::ScopedLock dataUpdate(dataUpdateLock_);
	dirty_ = true;
	song_ = song;
}

std::map<int, std::vector<std::string> >& TestVstAudioProcessor::getSong() {
	return song_;
}

void TestVstAudioProcessor::setLyrics(std::vector<std::string>& lyrics) {
	juce::ScopedLock dataUpdate(dataUpdateLock_);
	currentLyrics_ = lyrics;
	dirty_ = true;
}

int TestVstAudioProcessor::getPage() {
	return page_;
}

void TestVstAudioProcessor::setPage(int page) {
	juce::ScopedLock dataUpdate(dataUpdateLock_);
	page_ = page;
	auto mapIterator = song_.find(page);

	if (mapIterator != song_.end()) {
		currentLyrics_ = mapIterator->second;
	}
	else {
		currentLyrics_ = std::vector<std::string>();
	}
	progress_ = 0;
	if (lastControlValue_ != 0) {
		latched_ = false;
	}
	dirty_ = true;
}

int TestVstAudioProcessor::getProgress() {
	return progress_;
}

void TestVstAudioProcessor::setProgress(int progress) {
	juce::ScopedLock dataUpdate(dataUpdateLock_);
	progress_ = progress;
	dirty_ = true;
}

void TestVstAudioProcessor::setActiveLine(int activeLine)
{
	juce::ScopedLock dataUpdate(dataUpdateLock_);
	activeLine_ = activeLine;
	dirty_ = true;
}

void TestVstAudioProcessor::removeActiveLine(int activeLine)
{
	// Only remove the active line if it matches the one previously
	// set as this will support overlapping notes for lines
	if (activeLine == activeLine_) {
		juce::ScopedLock dataUpdate(dataUpdateLock_);
		activeLine_ = 0;
		dirty_ = true;
	}
}

int TestVstAudioProcessor::getActiveLine()
{
	return activeLine_;
}

juce::CriticalSection& TestVstAudioProcessor::getDataUpdateCriticalSection() {
	return dataUpdateLock_;
}

//==============================================================================
const juce::String TestVstAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool TestVstAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool TestVstAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool TestVstAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double TestVstAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int TestVstAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int TestVstAudioProcessor::getCurrentProgram()
{
	return 0;
}

void TestVstAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String TestVstAudioProcessor::getProgramName(int index)
{
	return {};
}

void TestVstAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void TestVstAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
	setPage(0);
	latched_ = true;
}

void TestVstAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
	setPage(0);
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TestVstAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void TestVstAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	int time;
	juce::MidiMessage m;
	auto dirty = false;

	int noteNumber = 0;
	int pageNumber = 0;
	int activeLine = 0;
	auto gotNoteOnEvent = false;
	auto gotNoteOffEvent = false;
	auto gotPageChangeEvent = false;
	auto gotSetActiveLineEvent = false;
	auto gotRemoveActiveLineEvent = false;
	auto gotModWheelEvent = false;
	int progress = 0;
	for (const auto midiData : midiMessages)
	{
		const auto midiMessage = midiData.getMessage();

		if (midiMessage.isControllerOfType(01)) {
			// Make it so that the controller wheel must latch onto the bottom
			// before the write on can start
			lastControlValue_ = midiMessage.getControllerValue();
			if (latched_ == true 
				|| lastControlValue_ < 1) {
				latched_ = true;
				progress = lastControlValue_;
				gotModWheelEvent = true;
			}
		}
		else if (midiMessage.isNoteOn()) {
			noteNumber = midiMessage.getNoteNumber();
			gotNoteOnEvent = true;

			// 35 allows you to set the blank page

			// Set active page
			if (noteNumber >= 35 && noteNumber < 36 + 24) {
				pageNumber = noteNumber - 35;
				gotPageChangeEvent = true;
			}

			// Set active line
			else if (noteNumber >= 60 && noteNumber < 60 + 6) {
				gotSetActiveLineEvent = true;
				activeLine = noteNumber - 59;
				setActiveLine(activeLine);
			}
		}
		else if (midiMessage.isNoteOff()) {
			noteNumber = midiMessage.getNoteNumber();
			gotNoteOffEvent = true;

			// Turn off active line
			if (noteNumber >= 36 + 24 && noteNumber < 36 + 24 + 6) {

				gotRemoveActiveLineEvent = true;
				activeLine = noteNumber - 59;
				removeActiveLine(activeLine);
			}
		}

		numMidiEvents_++;
		dirty = true;
	}

	//for (juce::MidiBuffer::Iterator i(midiMessages); i.getNextEvent(m, time);) {
	//	numMidiEvents_++;
	//	dirty = true;		
	//}

	if (dirty == true) {
		if (gotPageChangeEvent == true) {
			if (pageNumber != page_) {
				setPage(pageNumber);
			}
		}
		if (gotModWheelEvent == true) {

			if (progress_ != progress) {
				setProgress(progress);
			}
		}

	}
}

//==============================================================================
bool TestVstAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TestVstAudioProcessor::createEditor()
{
	return new TestVstAudioProcessorEditor(*this);
}

//==============================================================================
void TestVstAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void TestVstAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new TestVstAudioProcessor();
}
