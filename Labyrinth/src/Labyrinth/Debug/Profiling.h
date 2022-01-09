#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>

namespace Labyrinth {

	struct ProfileResult
	{
		std::string name;
		long long start, end;
		uint32_t threadID;
	};

	struct ProfilingSession
	{
		std::string name;
	};

	class Profiler
	{
	private:
		ProfilingSession* mCurrentSession;
		std::ofstream mOutputStream;
		int mProfileCount;

	public:
		Profiler()
			: mCurrentSession(nullptr), mProfileCount(0)
		{
		}

		void BeginSession(const std::string& name, const std::string& filepath = "results.json")
		{
			mOutputStream.open(filepath);
			WriteHeader();
			mCurrentSession = new ProfilingSession{ name };
		}

		void EndSession()
		{
			WriteFooter();
			mOutputStream.close();
			delete mCurrentSession;
			mCurrentSession = nullptr;
			mProfileCount = 0;
		}

		void WriteProfile(const ProfileResult& result)
		{
			if (mProfileCount++ > 0)
				mOutputStream << ",";

			std::string name = result.name;
			std::replace(name.begin(), name.end(), '"', '\'');

			mOutputStream << "{";
			mOutputStream << "\"cat\":\"function\",";
			mOutputStream << "\"dur\":" << (result.end - result.start) << ',';
			mOutputStream << "\"name\":\"" << name << "\",";
			mOutputStream << "\"ph\":\"X\",";
			mOutputStream << "\"pid\":0,";
			mOutputStream << "\"tid\":" << result.threadID << ",";
			mOutputStream << "\"ts\":" << result.start;
			mOutputStream << "}";

			mOutputStream.flush();
		}

		void WriteHeader()
		{
			mOutputStream << "{\"otherData\": {},\"traceEvents\":[";
			mOutputStream.flush();
		}

		void WriteFooter()
		{
			mOutputStream << "]}";
			mOutputStream.flush();
		}

		static Profiler& Get()
		{
			static Profiler instance;
			return instance;
		}
	};

	class ProfilingTimer
	{
	public:
		ProfilingTimer(const char* name)
			: mName(name), mStopped(false)
		{
			mStartTimepoint = std::chrono::high_resolution_clock::now();
		}

		~ProfilingTimer()
		{
			if (!mStopped)
				Stop();
		}

		void Stop()
		{
			auto endTimepoint = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(mStartTimepoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

			uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
			Profiler::Get().WriteProfile({ mName, start, end, threadID });

			mStopped = true;
		}
	private:
		const char* mName;
		std::chrono::time_point<std::chrono::high_resolution_clock> mStartTimepoint;
		bool mStopped;
	};
}

#ifdef LAB_DEBUG
	#define LAB_PROFILE 1
#endif

#if LAB_PROFILE
	// Resolve which function signature macro will be used. Note that this only
	// is resolved when the (pre)compiler starts, so the syntax highlighting
	// could mark the wrong one in your editor!
	#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
		#define LAB_FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__DMC__) && (__DMC__ >= 0x810)
		#define LAB_FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__FUNCSIG__)
		#define LAB_FUNC_SIG __FUNCSIG__
	#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
		#define LAB_FUNC_SIG __FUNCTION__
	#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
		#define LAB_FUNC_SIG __FUNC__
	#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
		#define LAB_FUNC_SIG __func__
	#elif defined(__cplusplus) && (__cplusplus >= 201103)
		#define LAB_FUNC_SIG __func__
	#else
		#define LAB_FUNC_SIG "LAB_FUNC_SIG unknown!"
	#endif

	#define LAB_PROFILE_BEGIN_SESSION(name, filepath) ::Labyrinth::Profiler::Get().BeginSession(name, filepath)
	#define LAB_PROFILE_END_SESSION() ::Labyrinth::Profiler::Get().EndSession()
	#define LAB_PROFILE_SCOPE(name) ::Labyrinth::ProfilingTimer timer##__LINE__(name);
	#define LAB_PROFILE_FUNCTION() LAB_PROFILE_SCOPE(LAB_FUNC_SIG)
#else
	#define LAB_PROFILE_BEGIN_SESSION(name, filepath)
	#define LAB_PROFILE_END_SESSION()
	#define LAB_PROFILE_SCOPE(name)
	#define LAB_PROFILE_FUNCTION()
#endif 