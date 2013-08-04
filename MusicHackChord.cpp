
// This is a skeleton file for use in creating your own plugin
// libraries.  Replace MusicHackChord and MusicHackChord throughout with the name
// of your first plugin class, and fill in the gaps as appropriate.


#include "MusicHackChord.h"


MusicHackChord::MusicHackChord(float inputSampleRate) :
    Plugin(inputSampleRate)
    // Also be sure to set your plugin parameters (presumably stored
    // in member variables) to their default values here -- the host
    // will not do that for you
{
}

MusicHackChord::~MusicHackChord()
{
}

string
MusicHackChord::getIdentifier() const
{
    return "MusicHackChord";
}

string
MusicHackChord::getName() const
{
    return "Music Hack Chord";
}

string
MusicHackChord::getDescription() const
{
    // Return something helpful here!
    return "Jie Chen - Music Hack Chord";
}

string
MusicHackChord::getMaker() const
{
    // Your name here
    return "Jie Chen";
}

int
MusicHackChord::getPluginVersion() const
{
    // Increment this each time you release a version that behaves
    // differently from the previous one
    return 1;
}

string
MusicHackChord::getCopyright() const
{
    // This function is not ideally named.  It does not necessarily
    // need to say who made the plugin -- getMaker does that -- but it
    // should indicate the terms under which it is distributed.  For
    // example, "Copyright (year). All Rights Reserved", or "GPL"
    return "";
}

MusicHackChord::InputDomain
MusicHackChord::getInputDomain() const
{
    return TimeDomain;
}

size_t
MusicHackChord::getPreferredBlockSize() const
{
    return 0; // 0 means "I can handle any block size"
}

size_t 
MusicHackChord::getPreferredStepSize() const
{
    return 0; // 0 means "anything sensible"; in practice this
              // means the same as the block size for TimeDomain
              // plugins, or half of it for FrequencyDomain plugins
}

size_t
MusicHackChord::getMinChannelCount() const
{
    return 1;
}

size_t
MusicHackChord::getMaxChannelCount() const
{
    return 1;
}

MusicHackChord::ParameterList
MusicHackChord::getParameterDescriptors() const
{
    ParameterList list;

    // If the plugin has no adjustable parameters, return an empty
    // list here (and there's no need to provide implementations of
    // getParameter and setParameter in that case either).

    // Note that it is your responsibility to make sure the parameters
    // start off having their default values (e.g. in the constructor
    // above).  The host needs to know the default value so it can do
    // things like provide a "reset to default" function, but it will
    // not explicitly set your parameters to their defaults for you if
    // they have not changed in the mean time.

    ParameterDescriptor d;
    d.identifier = "parameter";
    d.name = "Some Parameter";
    d.description = "";
    d.unit = "";
    d.minValue = 0;
    d.maxValue = 10;
    d.defaultValue = 5;
    d.isQuantized = false;
    list.push_back(d);

    return list;
}

float
MusicHackChord::getParameter(string identifier) const
{
    if (identifier == "parameter") {
        return 5; // return the ACTUAL current value of your parameter here!
    }
    return 0;
}

void
MusicHackChord::setParameter(string identifier, float value) 
{
    if (identifier == "parameter") {
        // set the actual value of your parameter
    }
}

MusicHackChord::ProgramList
MusicHackChord::getPrograms() const
{
    ProgramList list;

    // If you have no programs, return an empty list (or simply don't
    // implement this function or getCurrentProgram/selectProgram)

    return list;
}

string
MusicHackChord::getCurrentProgram() const
{
    return ""; // no programs
}

void
MusicHackChord::selectProgram(string name)
{
}

MusicHackChord::OutputList
MusicHackChord::getOutputDescriptors() const
{
    OutputList list;

    // See OutputDescriptor documentation for the possibilities here.
    // Every plugin must have at least one output.

    OutputDescriptor d;
    d.identifier = "chord";
    d.name = "Chord";
    d.description = "";
    d.unit = "";
    d.hasFixedBinCount = true;
    d.binCount = 1;
    d.hasKnownExtents = false;
    d.isQuantized = false;
    d.sampleType = OutputDescriptor::OneSamplePerStep;
    d.hasDuration = false;
    list.push_back(d);

    return list;
}

bool
MusicHackChord::initialise(size_t channels, size_t stepSize, size_t blockSize)
{
    if (channels < getMinChannelCount() ||
	channels > getMaxChannelCount()) return false;

    // Real initialisation work goes here!
	memset(xhs, 0, sizeof(xhs));
	m_counter=0;

	m_stepSize = stepSize;
	m_blockSize = blockSize;
	m_startPos = blockSize - stepSize;
	if(m_startPos < 0)
		m_startPos = 0;

    return true;
}

void
MusicHackChord::reset()
{
    // Clear buffers, reset stored values, etc
}

MusicHackChord::FeatureSet
MusicHackChord::process(const float *const *inputBuffers, Vamp::RealTime timestamp)
{
    // Do actual work!
	int size = chromas[0].size();
	for(int i = m_startPos; i < m_blockSize; i++)
	{
		for(int j = 0; j < NUM_BINS; j++)
		{
			if(size >= BUFFER_SIZE)
				chromas[j].pop_front();
			chromas[j].push_back(apBandPass(inputBuffers[0][i], j));
		}	
	}
	m_counter += m_stepSize;
	if(m_counter >= BUFFER_SIZE)
	{	
		float chromaOctaves[NUM_CHROMAS];
		float correlations[2][NUM_CHROMAS];
		memset(chromaOctaves, 0, sizeof(chromaOctaves));
		memset(correlations, 0, sizeof(correlations));
		for(int i = 0; i < NUM_BINS; i++)
		{
			float sum = 0;
			for(int j = 0; j < chromas[j].size(); j++)
				sum += chromas[i][j] * chromas[i][j];
			chromaOctaves[i % NUM_CHROMAS] += sum;
		}
		float m = mean(chromaOctaves, NUM_CHROMAS);
		for(int i = 0; i < NUM_CHROMAS; i++)
		{	
			for(int j = 0; j < NUM_CHROMAS; j++)
			{
				correlations[0][i] += (chromaOctaves[j] - m) * (MAJOR[(j - i + NUM_CHROMAS) % NUM_CHROMAS] - MEAN_CHORD);
				correlations[1][i] += (chromaOctaves[j] - m) * (MINOR[(j - i + NUM_CHROMAS) % NUM_CHROMAS] - MEAN_CHORD); 
			}
		}
		int majorMaxIndex = max(correlations[0], NUM_CHROMAS);
		int minorMaxIndex = max(correlations[1], NUM_CHROMAS);
		Feature f;
		f.hasTimestamp = false;
		if(correlations[0][majorMaxIndex] > correlations[1][minorMaxIndex])
			f.values.push_back(majorMaxIndex);
		else
			f.values.push_back(minorMaxIndex + NUM_CHROMAS);

		FeatureSet fs;
		fs[0].push_back(f);
		m_counter=0; 
		return fs;
	}
    return FeatureSet();
}

MusicHackChord::FeatureSet
MusicHackChord::getRemainingFeatures()
{
    return FeatureSet();
}

float MusicHackChord::apBandPass(float x, int index)
{
	float xhNew = x - FILTER_DS[index] * (1 - FILTER_C) * xhs[0][index] + FILTER_C * xhs[1][index];
	float apY = -FILTER_C * xhNew + FILTER_DS[index] * (1 - FILTER_C) * xhs[0][index] + xhs[1][index];
	xhs[1][index] = xhs[0][index];
	xhs[0][index] = xhNew;
	return 0.5 * (x - apY);
}

float MusicHackChord::mean(float x[], int size)
{
	float sum = 0;
	for(int i = 0; i < size; i++)
		sum += x[i];
	sum /= size;
	return sum;
}

int MusicHackChord::max(float x[], int size)
{
	int index = 0;
	float max = x[0];
	for(int i = 1; i < size; i++)
	{
		if(max < x[i])
		{
			index = i;
			max = x[i];
		}
	}
	return index;
}

const double MusicHackChord::FILTER_DS[NUM_BINS]={-0.999877, -0.999862, -0.999845, -0.999826, -0.999805, -0.999781, -0.999754, -0.999724, -0.999691, -0.999653, -0.999610, -0.999562,
-0.999509, -0.999449, -0.999381, -0.999305, -0.999220, -0.999125, -0.999018, -0.998897, -0.998762, -0.998611, -0.998441, -0.998250,
-0.998036, -0.997795, -0.997525, -0.997222, -0.996882, -0.996501, -0.996073, -0.995592, -0.995053, -0.994447, -0.993768, -0.993006,
-0.992150, -0.991191, -0.990113, -0.988905, -0.987549, -0.986028, -0.984321, -0.982407, -0.980260, -0.977851, -0.975150, -0.972121,
-0.968725, -0.964917, -0.960649, -0.955866, -0.950506, -0.944502, -0.937777, -0.930247, -0.921818, -0.912385, -0.901835, -0.890038,
-0.876855, -0.862131, -0.845694, -0.827360, -0.806925, -0.784168, -0.758851, -0.730718, -0.699495, -0.664894, -0.626612, -0.584336,
-0.537750, -0.486538, -0.430398, -0.369049, -0.302255, -0.229839, -0.151710, -0.067897, 0.021413, 0.115832, 0.214715, 0.317102};

const float MusicHackChord::MAJOR[NUM_CHROMAS] = {1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0}; 
const float MusicHackChord::MINOR[NUM_CHROMAS] = {1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0};
