#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>

#include <string>
#include <sstream>
#include <thread>

namespace Labyrinth {

	struct ProfileResult
	{
		std::string name;
		long long start, end;
		std::thread::id threadID;
	};

	struct ProfilingSession
	{
		std::string name;
	};

	class Profiler
	{
	public:
		Profiler(const Profiler&) = delete;
		Profiler(Profiler&&) = delete;

		void BeginSession(const std::string& name, const std::string& filepath = "results.json")
		{
			std::lock_guard lock(mMutex);
			if (mCurrentSession) {
				// If there is already a current session, then close it before beginning new one.
				// Subsequent profiling output meant for the original session will end up in the
				// newly opened session instead.  That's better than having badly formatted
				// profiling output.
				if (Log::GetCoreLogger()) { // Edge case: BeginSession() might be before Log::Init()
					LAB_CORE_ERROR("Instrumentor::BeginSession('{0}') when session '{1}' already open.", name, mCurrentSession->name);
				}
				InternalEndSession();
			}

			mOutputStream.open(filepath);
			if (mOutputStream.is_open()) {
				mCurrentSession = new ProfilingSession({ name });
				WriteHeader();
			}
			else 
			{
				if (Log::GetCoreLogger()) // Edge case: BeginSession() might be before Log::Init()
				{ 
					LAB_CORE_ERROR("Instrumentor could not open results file '{0}'.", filepath);
				}
			}
		}

		void EndSession()
		{
			std::lock_guard lock(mMutex);
			InternalEndSession();
		}

		void WriteProfile(const ProfileResult& result)
		{
			std::stringstream json;

			json << ",{";
			json << "\"cat\":\"function\",";
			json << "\"dur\":" << (result.end - result.start) << ',';
			json << "\"name\":\"" << result.name << "\",";
			json << "\"ph\":\"X\",";
			json << "\"pid\":0,";
			json << "\"tid\":" << result.threadID << ",";
			json << "\"ts\":" << result.start;
			json << "}";

			std::lock_guard lock(mMutex);
			if (mCurrentSession) {
				mOutputStream << json.str();
				mOutputStream.flush();
			}
		}

		static Profiler& Get() {
			static Profiler instance;
			return instance;
		}

	private:
		Profiler()
			: mCurrentSession(nullptr)
		{
		}

		~Profiler()
		{
			EndSession();
		}

		void WriteHeader()
		{
			mOutputStream << "{\"otherData\": {},\"traceEvents\":[{}";
			mOutputStream.flush();
		}

		void WriteFooter()
		{
			mOutputStream << "]}";
			mOutputStream.flush();
		}

		// Note: you must already own lock on mMutex before
		// calling InternalEndSession()
		void InternalEndSession() 
		{
			if (mCurrentSession) {
				WriteFooter();
				mOutputStream.close();
				delete mCurrentSession;
				mCurrentSession = nullptr;
			}
		}

	private:
		std::mutex mMutex;
		ProfilingSession* mCurrentSession;
		std::ofstream mOutputStream;
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

			Profiler::Get().WriteProfile({ mName, start, end, std::this_thread::get_id() });

			mStopped = true;
		}
	private:
		const char* mName;
		std::chrono::time_point<std::chrono::high_resolution_clock> mStartTimepoint;
		bool mStopped;
	};

	namespace ProfilingUtils {

		template <size_t N>
		struct ChangeResult
		{
			char data[N];
		};

		template <size_t N, size_t K>
		constexpr auto CleanupOutputString(const char(&expr)[N], const char(&remove)[K])
		{
			ChangeResult<N> result = {};

			size_t srcIndex = 0;
			size_t dstIndex = 0;
			while (srcIndex < N)
			{
				size_t matchIndex = 0;
				while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
					matchIndex++;
				if (matchIndex == K - 1)
					srcIndex += matchIndex;
				result.data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
				srcIndex++;
			}
			return result;
		}
	}
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
	#elif (defined(__FUNCSIG__) || (_MSC_VER))
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
	#define LAB_PROFILE_SCOPE_LINE2(name, line) constexpr auto fixedName##line = ::Labyrinth::ProfilingUtils::CleanupOutputString(name, "__cdecl ");\
												   ::Labyrinth::ProfilingTimer timer##line(fixedName##line.data)
	#define LAB_PROFILE_SCOPE_LINE(name, line) LAB_PROFILE_SCOPE_LINE2(name, line)
	#define LAB_PROFILE_SCOPE(name) LAB_PROFILE_SCOPE_LINE(name, __LINE__)
	#define LAB_PROFILE_FUNCTION() LAB_PROFILE_SCOPE(LAB_FUNC_SIG)
#else
	#define LAB_PROFILE_BEGIN_SESSION(name, filepath)
	#define LAB_PROFILE_END_SESSION()
	#define LAB_PROFILE_SCOPE(name)
	#define LAB_PROFILE_FUNCTION()
#endif 