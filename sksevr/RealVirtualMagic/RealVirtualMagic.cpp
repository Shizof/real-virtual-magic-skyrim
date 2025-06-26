#include "RealVirtualMagic.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <numeric>
#include <algorithm>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;
using namespace lsl;

namespace RealVirtualMagic
{
	bool IXRInitialized = false;
	bool eventMarkersInitialized = false;
	bool IXRIsInitializing = false;
	bool eventMarkersIsInitializing = false;

	// Use pointers for stream_inlet and stream_outlet
	stream_inlet* IXRStream = nullptr;
	stream_outlet* eventOutlet = nullptr;

	int creationCounter = 0;
	int creationCounterMax = 9999999; // LSL is re-initialized every 10s if it hadnt found a stream after 5s. set this to a lower number to give up at some point

	void InitializeIXRStream()
	{
		IXRIsInitializing = true;
		LOG("Resolving LSL streams");
		std::vector<stream_info> results = resolve_stream("type", "IXR-metric", 1, 5.0);
		if (results.empty()) {
			LOG("No LSL streams found!");
		}
		else 
		{
			// Log found stream
			std::string lslstring = results[0].as_xml();
			LOG("Found stream: %s", lslstring.c_str());

			if (results[0].name() == "BrainPower" && results[0].source_id() == "ixrflow_transmit_power") 
			{
				LOG("Creating stream inlet for BrainPower stream");
				IXRStream = new stream_inlet(results[0], 1, 1);
				IXRStream->set_postprocessing(post_clocksync | post_monotonize);
				LOG("--------------- IXR Suite connected!!! ---------------");
				IXRInitialized = true;
			}
			else {
				LOG("Found stream is not the BrainPower stream we're looking for");
			}
		}
		IXRIsInitializing = false;
	}

	void CreateEventStream()
	{
		eventMarkersIsInitializing = true;
		LOG("Creating event marker stream outlet");
		// Marker streams are irregular and usually have channel format string
		stream_info info("RVM-events", "Markers", 1, IRREGULAR_RATE, cf_string, "RVM_Events_01");
		eventOutlet = new stream_outlet(info);  // Create a new stream_outlet with the defined stream_info
		eventMarkersIsInitializing = false;
		eventMarkersInitialized = true;
		LOG("--------------- Marker stream created!!! ---------------");
	}

	void CreateSystem()
	{
		if (creationCounter < creationCounterMax)
		{
			creationCounter++;
			LOG("Creating LSL system %i / %i", creationCounter, creationCounterMax);
			
			if (!IXRInitialized && !IXRIsInitializing)
			{
				InitializeIXRStream();
			}

			if (!eventMarkersInitialized && !eventMarkersIsInitializing)
			{
				CreateEventStream();
			}
		}

		// wait for 10s to make sure there are no two initialization processes going on
		Sleep(10000);
	}

	double GetFocusValue()
	{
		if (!IXRInitialized) {
			LOG("IXRStream is not initialized. Attempting to initialize.");
			CreateSystem();
			return 0.0;
		}

		double latest_timestamp = 0.0;
		double latest_value = 0.0;
		std::vector<double> sample;

		// Brute force method
		double timestamp = IXRStream->pull_sample(sample, 0.1); // 100ms timeout for first sample
		if (timestamp != 0.0) 
		{
			latest_timestamp = timestamp;
			latest_value = sample[0];

			while (true) 
			{
				timestamp = IXRStream->pull_sample(sample, 0.005); // 5ms timeout for additional samples
				if (timestamp == 0.0) 
				{
					break;
				}
				if (timestamp > latest_timestamp)
				{
					latest_timestamp = timestamp;
					latest_value = sample[0];
				}
			}
		}

		if (latest_value < 0.0 || latest_value > 1.0)
		{
			LOG("Warning: Brain power value outside expected range 0.0-1.0");
		}

		if (latest_value == 0.0)
		{
			delete IXRStream;
			IXRStream = nullptr;
			IXRInitialized = false;
			LOG("--------------- No sample available. IXRStream stream inlet closed!!! ---------------");
			return 0.0;
		}

		//LOG_ERR("Received raw brain power value: %g", value);

		LOG("LSL-timestamp: %g, brain power: %g", latest_timestamp, latest_value);
		return latest_value;
	}

	void WriteEventMarker(const std::string& eventType)
	{
		//board->insert_marker(eventType);
		LOG("marker: %s", eventType.c_str());

		// Send the event marker via LSL
		if (!eventMarkersInitialized) {
			LOG("LSL event marker stream is not initialized. Attempting to initialize.");
			CreateSystem();
			return;
		}
		std::vector<std::string> sample(1);
		sample[0] = eventType;
		eventOutlet->push_sample(sample);
	}
}
