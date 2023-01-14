//	MIT License
//	
//	Copyright(c) 2017 Thomas Monkman
//	
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files(the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions :
//	
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//	
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.

#pragma once

#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <vector>
#include <array>
#include <map>
#include <system_error>
#include <string>
#include <algorithm>
#include <type_traits>
#include <future>
#include <regex>
#include <iostream>
#include <cstdlib>
#include <cstdio>

#include <Labyrinth/Core/System/Base.h>

#ifdef LAB_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
    #define NOMINMAX
#endif
#include <windows.h>
#include <tchar.h>
#include <Pathcch.h>
#include <shlwapi.h>
#endif // Windows

#ifdef LAB_PLATFORM_LINUX
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>
#endif // Linux

namespace Laby {
    
	enum class FSEvent {
		Added,
		Removed,
		Modified,
		PreRename,
		PostRename
	};

	using FSEventCallback = Action<const std::filesystem::path&, const FSEvent>;

	/**
	* \class FileWatcher
	*
	* \brief Watches a folder or file, and will notify of changes via function callback.
	*
	* \author Thomas Monkman
	*
	*/
	class FileWatcher
	{
    private:
        using T = std::filesystem::path;
		using C = typename T::value_type;
        
		using UnderpinningString = std::basic_string<C, std::char_traits<C>>;
		using UnderpinningRegex  = std::basic_regex<C, std::regex_traits<C>>;

		struct PathParts
		{
			T directory;
			T filename;
            
			PathParts(T dir, T file) : directory(dir), filename(file) {}
		};

	public:
		FileWatcher(T path, UnderpinningRegex pattern, FSEventCallback callback) :
			mPath(path),
			mPattern(pattern),
			mCallback(callback),
			mDirectory(get_directory(path))
		{
			init();
		}

		FileWatcher(T path, FSEventCallback callback) :
			FileWatcher(path, UnderpinningRegex(REGEX_ALL), callback) {}

		~FileWatcher() 
        {
			destroy();
		}

		FileWatcher(const FileWatcher& other) : FileWatcher(other.mPath, other.mCallback) {}

		//FileWatcher& operator=(const FileWatcher& other) 
		//{
		//	if (this == &other) 
  //              return *this;

		//	destroy();
		//	mPath = other.mPath;
		//	mCallback = other.mCallback;
		//	mDirectory = get_directory(other.mPath);
		//	init();
		//	return *this;
		//}

		// Const memeber variables don't let me implent moves nicely, if moves are really wanted std::unique_ptr should be used and move that.
		FileWatcher(FileWatcher&&) = delete;
		FileWatcher& operator=(FileWatcher&&) & = delete;

	private:
		static constexpr C REGEX_ALL[] = { '.', '*', '\0' };
		static constexpr C THIS_DIRECTORY[] = { '.', '/', '\0' };
		static constexpr usize BUFFER_SIZE = 1024 * 256;

		const T mPath;
		UnderpinningRegex mPattern;

		// only used if watch a single file
		bool mWatchingSingleFile = { false };
		T mFilename;

		std::atomic<bool> mDestroy = { false };
		FSEventCallback mCallback;

		std::thread mWatchThread;

		std::condition_variable mCV;
		std::mutex mCallbackMutex;
		std::vector<std::pair<T, FSEvent>> mCallbackInfo;
		std::thread mCallbackThread;

		std::promise<void> mRunning;
#ifdef LAB_PLATFORM_WINDOWS
		HANDLE mDirectory = { nullptr };
		HANDLE mCloseEvent = { nullptr };

		const DWORD mListenFilters =
			FILE_NOTIFY_CHANGE_SECURITY |
			FILE_NOTIFY_CHANGE_CREATION |
			FILE_NOTIFY_CHANGE_LAST_ACCESS |
			FILE_NOTIFY_CHANGE_LAST_WRITE |
			FILE_NOTIFY_CHANGE_SIZE |
			FILE_NOTIFY_CHANGE_ATTRIBUTES |
			FILE_NOTIFY_CHANGE_DIR_NAME |
			FILE_NOTIFY_CHANGE_FILE_NAME;

		const std::map<DWORD, FSEvent> mEventTypeMapping = {
			{ FILE_ACTION_ADDED,            FSEvent::Added },
			{ FILE_ACTION_REMOVED,          FSEvent::Removed },
			{ FILE_ACTION_MODIFIED,         FSEvent::Modified },
			{ FILE_ACTION_RENAMED_OLD_NAME, FSEvent::PreRename },
			{ FILE_ACTION_RENAMED_NEW_NAME, FSEvent::PostRename }
		};
#endif // LAB_PLATFORM_WINDOWS

#if LAB_PLATFORM_LINUX
		struct FolderInfo 
        {
			int folder;
			int watch;
		};

		FolderInfo mDirectory;

		const u32 mListenFilters = IN_MODIFY | IN_CREATE | IN_DELETE;

		const static usize sEventSize = (sizeof(struct inotify_event));
#endif // LAB_PLATFORM_LINUX

		void init() 
		{
#ifdef LAB_PLATFORM_WINDOWS
			mCloseEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			if (!mCloseEvent)
				throw std::system_error(GetLastError(), std::system_category());
            
#endif // LAB_PLATFORM_WINDOWS
			mCallbackThread = std::move(std::thread([this]() 
            {
				try 
                {
					callback_thread();
				} catch (...) 
                {
					try 
                    {
						mRunning.set_exception(std::current_exception());
					}
					catch (...) {} // set_exception() may throw too
				}
			}));
			mWatchThread = std::move(std::thread([this]() 
            { 
				try 
                {
					monitor_directory();
				} catch (...) 
                {
					try 
                    {
						mRunning.set_exception(std::current_exception());
					}
					catch (...) {} // set_exception() may throw too
				}
			}));

			std::future<void> future = mRunning.get_future();
			future.get(); //block until the monitor_directory is up and running
		}

		void destroy()
		{
			mDestroy = true;
			mRunning = std::promise<void>();
#ifdef LAB_PLATFORM_WINDOWS
			SetEvent(mCloseEvent);
#elif LAB_PLATFORM_LINUX
			inotify_rm_watch(mDirectory.folder, mDirectory.watch);
#endif // LAB_PLATFORM_LINUX
			mCV.notify_all();
			mWatchThread.join();
			mCallbackThread.join();
#ifdef LAB_PLATFORM_WINDOWS
			CloseHandle(mDirectory);
#elif LAB_PLATFORM_LINUX
			close(mDirectory.folder);
#endif // LAB_PLATFORM_LINUX
		}

		const PathParts split_directory_and_file(const T& path) const 
		{
			const auto predict = [](C character) {
#ifdef LAB_PLATFORM_WINDOWS
				return character == C('\\') || character == C('/');
#elif LAB_PLATFORM_LINUX
				return character == C('/');
#endif // LAB_PLATFORM_LINUX
			};

			UnderpinningString path_string = path;
			const auto pivot = std::find_if(path_string.rbegin(), path_string.rend(), predict).base();
			//if the path is something like "test.txt" there will be no directory part, however we still need one, so insert './'
			const T directory = [&]() 
            {
				const auto extracted_directory = UnderpinningString(path_string.begin(), pivot);
				return (extracted_directory.size() > 0) ? extracted_directory : UnderpinningString(THIS_DIRECTORY);
			}();
			const T filename = UnderpinningString(pivot, path_string.end());
			return PathParts(directory, filename);
		}

		bool pass_filter(const UnderpinningString& file_path)
		{ 
			if (mWatchingSingleFile) 
			{
				const UnderpinningString extracted_filename = split_directory_and_file(file_path).filename;
				//if we are watching a single file, only that file should trigger action
				return extracted_filename == mFilename;
			}
			return std::regex_match(file_path, mPattern);
		}

#ifdef LAB_PLATFORM_WINDOWS
		template<typename... Args> 
        DWORD GetFileAttributesX(const char* lpFileName, Args... args) 
        {
			return GetFileAttributesA(lpFileName, args...);
		}
		template<typename... Args> 
        DWORD GetFileAttributesX(const wchar_t* lpFileName, Args... args) 
        {
			return GetFileAttributesW(lpFileName, args...);
		}

		template<typename... Args> 
        HANDLE CreateFileX(const char* lpFileName, Args... args) 
        {
			return CreateFileA(lpFileName, args...);
		}
		template<typename... Args> 
        HANDLE CreateFileX(const wchar_t* lpFileName, Args... args) 
        {
			return CreateFileW(lpFileName, args...);
		}

		HANDLE get_directory(const T& path) 
		{
			auto file_info = GetFileAttributesX(path.c_str());

			if (file_info == INVALID_FILE_ATTRIBUTES)
				throw std::system_error(GetLastError(), std::system_category());
            
			mWatchingSingleFile = (file_info & FILE_ATTRIBUTE_DIRECTORY) == false;

			const T watch_path = [this, &path]() 
            {
				if (!mWatchingSingleFile)
                    return path;
                
                const auto parsed_path = split_directory_and_file(path);
                mFilename = parsed_path.filename;
                return parsed_path.directory;
			}();

			HANDLE directory = CreateFileX(
				watch_path.c_str(),           // pointer to the file name
				FILE_LIST_DIRECTORY,    // access (read/write) mode
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // share mode
				nullptr, // security descriptor
				OPEN_EXISTING,         // how to create
				FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, // file attributes
				HANDLE(0));                 // file with attributes to copy

			if (directory == INVALID_HANDLE_VALUE)
				throw std::system_error(GetLastError(), std::system_category());
            
			return directory;
		}

		void convert_wstring(const std::wstring& wstr, std::string& out)
		{
			int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
			out.resize(size_needed, '\0');
			WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &out[0], size_needed, NULL, NULL);
		}

		void convert_wstring(const std::wstring& wstr, std::wstring& out)
		{
			out = wstr;
		}

		void monitor_directory() 
		{
			std::vector<BYTE> buffer(BUFFER_SIZE);
			DWORD bytes_returned = 0;
			OVERLAPPED overlapped_buffer{ 0 };

			overlapped_buffer.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			if (!overlapped_buffer.hEvent)
				std::cerr << "Error creating monitor event" << std::endl;

			std::array<HANDLE, 2> handles{ overlapped_buffer.hEvent, mCloseEvent };

			auto async_pending = false;
			mRunning.set_value();
			do {
				std::vector<std::pair<T, FSEvent>> parsed_information;
				ReadDirectoryChangesW(
					mDirectory,
					buffer.data(), static_cast<DWORD>(buffer.size()),
					TRUE,
					mListenFilters,
					&bytes_returned,
					&overlapped_buffer, NULL);
			
				async_pending = true;
			
				switch (WaitForMultipleObjects(2, handles.data(), FALSE, INFINITE))
				{
				case WAIT_OBJECT_0:
				{
					if (!GetOverlappedResult(mDirectory, &overlapped_buffer, &bytes_returned, TRUE))
						throw std::system_error(GetLastError(), std::system_category());
                        
					async_pending = false;

					if (bytes_returned == 0)
						break;

					FILE_NOTIFY_INFORMATION *file_information = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&buffer[0]);
					do
					{
						std::wstring changed_file_w{ file_information->FileName, file_information->FileNameLength / sizeof(file_information->FileName[0]) };
						UnderpinningString changed_file;
						convert_wstring(changed_file_w, changed_file);
                        
						if (pass_filter(changed_file))
							parsed_information.emplace_back(T{ changed_file }, mEventTypeMapping.at(file_information->Action));

						if (file_information->NextEntryOffset == 0)
							break;

						file_information = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<BYTE*>(file_information) + file_information->NextEntryOffset);
                        
					} while (true);
					break;
				}
				case WAIT_OBJECT_0 + 1:
					// quit
					break;
				case WAIT_FAILED:
					break;
				}
				//dispatch callbacks
				{
					std::lock_guard<std::mutex> lock(mCallbackMutex);
					mCallbackInfo.insert(mCallbackInfo.end(), parsed_information.begin(), parsed_information.end());
				}
				mCV.notify_all();
			} while (mDestroy == false);

			if (async_pending)
			{
				//clean up running async io
				CancelIo(mDirectory);
				GetOverlappedResult(mDirectory, &overlapped_buffer, &bytes_returned, TRUE);
			}
		}
#endif // LAB_PLATFORM_WINDOWS

#if LAB_PLATFORM_LINUX

		bool is_file(const T& path) const
		{
			struct stat statbuf = {};
			if (stat(path.c_str(), &statbuf) != 0)
				throw std::system_error(errno, std::system_category());
            
			return S_ISREG(statbuf.st_mode);
		}

		FolderInfo get_directory(const T& path) 
		{
			const auto folder = inotify_init();
			if (folder < 0)
				throw std::system_error(errno, std::system_category());
            
			const auto listen_filters = mListenFilters;

			mWatchingSingleFile = is_file(path);

			const T watch_path = [this, &path]() 
            {
				if (!mWatchingSingleFile)
					return path;
                
                const auto parsed_path = split_directory_and_file(path);
                mFilename = parsed_path.filename;
                return parsed_path.directory;
			}();

			const auto watch = inotify_add_watch(folder, watch_path.c_str(), IN_MODIFY | IN_CREATE | IN_DELETE);
			if (watch < 0)
				throw std::system_error(errno, std::system_category());
            
			return { folder, watch };
		}

		void monitor_directory() 
		{
			std::vector<Byte> buffer(BUFFER_SIZE);

			mRunning.set_value();
			while (mDestroy == false) 
			{
				const auto length = read(mDirectory.folder, static_cast<void*>(buffer.data()), buffer.size());
				if (length <= 0) 
                    continue;
                
                int i = 0;
                std::vector<std::pair<T, FSEvent>> parsed_information;
                while (i < length) 
                {
                    struct inotify_event *event = reinterpret_cast<struct inotify_event *>(&buffer[i]); // NOLINT
                    if (event->len) 
                    {
                        const UnderpinningString changed_file{ event->name };
                        if (pass_filter(changed_file))
                        {
                            if (event->mask & IN_CREATE) 
                                parsed_information.emplace_back(T{ changed_file }, FSEvent::Added);
                            else if (event->mask & IN_DELETE)
                                parsed_information.emplace_back(T{ changed_file }, FSEvent::Removed);
                            else if (event->mask & IN_MODIFY)
                                parsed_information.emplace_back(T{ changed_file }, FSEvent::Modified);
                        }
                    }
                    i += sEventSize + event->len;
                }
                //dispatch callbacks
                {
                    std::lock_guard<std::mutex> lock(mCallbackMutex);
                    mCallbackInfo.insert(mCallbackInfo.end(), parsed_information.begin(), parsed_information.end());
                }
                mCV.notify_all();
			}
		}
#endif // LAB_PLATFORM_LINUX

		void callback_thread()
		{
			while (mDestroy == false) {
				std::unique_lock<std::mutex> lock(mCallbackMutex);
				if (mCallbackInfo.empty() && mDestroy == false)
					mCV.wait(lock, [this] { return mCallbackInfo.size() > 0 || mDestroy; });
                
				decltype(mCallbackInfo) callback_information = {};
				std::swap(callback_information, mCallbackInfo);
				lock.unlock();

				for (const auto& file : callback_information) 
                {
					if (!mCallback)
                        continue;
                    
                    try
                    {
                        mCallback(file.first, file.second);
                    }
                    catch (const std::exception&) {}
                        
				}
			}
		}
	};

    constexpr typename FileWatcher::C FileWatcher::REGEX_ALL[];
    constexpr typename FileWatcher::C FileWatcher::THIS_DIRECTORY[];
}
