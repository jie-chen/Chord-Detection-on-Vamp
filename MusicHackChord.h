
// This is a skeleton file for use in creating your own plugin
// libraries.  Replace MusicHackChord and MusicHackChord throughout with the name
// of your first plugin class, and fill in the gaps as appropriate.


// Remember to use a different guard symbol in each header!
#ifndef _MY_PLUGIN_H_
#define _MY_PLUGIN_H_

#define NUM_BINS 84
#define NUM_CHROMAS 12
#define MEAN_CHORD 0.25
#define BUFFER_SIZE 22050
#define FILTER_C (-0.9984)

#include <queue>
#include <vamp-sdk/Plugin.h>

using std::string;

class MusicHackChord : public Vamp::Plugin
{
public:
    MusicHackChord(float inputSampleRate);
    virtual ~MusicHackChord();

    string getIdentifier() const;
    string getName() const;
    string getDescription() const;
    string getMaker() const;
    int getPluginVersion() const;
    string getCopyright() const;

    InputDomain getInputDomain() const;
    size_t getPreferredBlockSize() const;
    size_t getPreferredStepSize() const;
    size_t getMinChannelCount() const;
    size_t getMaxChannelCount() const;

    ParameterList getParameterDescriptors() const;
    float getParameter(string identifier) const;
    void setParameter(string identifier, float value);

    ProgramList getPrograms() const;
    string getCurrentProgram() const;
    void selectProgram(string name);

    OutputList getOutputDescriptors() const;

    bool initialise(size_t channels, size_t stepSize, size_t blockSize);
    void reset();

    FeatureSet process(const float *const *inputBuffers,
                       Vamp::RealTime timestamp);

    FeatureSet getRemainingFeatures();

protected:
    // plugin-specific data and methods go here
	static const double FILTER_DS[NUM_BINS];
	static const float MAJOR[NUM_CHROMAS];
	static const float MINOR[NUM_CHROMAS];
	float xhs[2][NUM_BINS];
	std::deque<float> chromas[NUM_BINS];
	float apBandPass(float x, int index);
	float mean(float x[], int size);
	int max(float x[], int size);
	int m_counter;
	int m_blockSize;
	int m_stepSize;
	int m_startPos;
};



#endif
