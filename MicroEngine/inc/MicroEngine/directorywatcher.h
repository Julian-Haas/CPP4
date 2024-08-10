//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <utility>
#include "util.h"
#include "eventbus.h"
#include "me_interface.h"

namespace me
{
	class FileChangedEvent : public IEvent
	{
	public:
		FileChangedEvent(std::string fileName, bool removed)
			: m_FileName(std::move(fileName))
		    , m_Removed(removed)
		{}
		virtual ~FileChangedEvent() = default;
		static const EventID ID = EngineEventID::FileChanged;
		virtual EventID GetID() const override { return ID; }
		const std::string m_FileName;
		const bool m_Removed;

	private:
		ME_MOVE_COPY_NOT_ALLOWED(FileChangedEvent);
	};

	class DirectoryWatcher
	{
	public:
		ME_API DirectoryWatcher(const char* folder);
		ME_API virtual ~DirectoryWatcher();
		
		ME_API void CheckForChanges();
	private:
		ME_MOVE_COPY_NOT_ALLOWED(DirectoryWatcher);

		std::string m_Folder;
		std::thread m_Thread;
		std::atomic_bool m_IsRunning;
		std::mutex m_ChangedFilesMutex;
		std::map<std::string, std::shared_ptr<FileChangedEvent>> m_ChangedFiles;
		ScopedHandle m_DirectoryHandle;
		ScopedHandle m_DirectoryIOCP;
		uint8_t m_FileNotifyInfoBuffer[32 * 1024];
		OVERLAPPED m_OL;

		void AsyncCheckForChanges();
	};
};