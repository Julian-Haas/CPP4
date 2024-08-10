//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "directorywatcher.h"
#include "util.h"

namespace me
{
	DirectoryWatcher::DirectoryWatcher(const char* folder)
		: m_Folder(folder)
	    , m_IsRunning(true)
	    , m_FileNotifyInfoBuffer{}
	    , m_OL{}
	{
		if (StartsWith(m_Folder, "./"))
			m_Folder.erase(0, 2);
		if (!EndsWith(folder, "/"))
			m_Folder.append("/");
		m_DirectoryHandle.Set(CreateFileW(UTF8ToUTF16(folder).c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr));

		ME_ASSERTF(m_DirectoryHandle != INVALID_HANDLE_VALUE, "Error: 0x%X", GetLastError());
		if (m_DirectoryHandle != INVALID_HANDLE_VALUE)
		{
			m_DirectoryIOCP.Set(CreateIoCompletionPort(m_DirectoryHandle, nullptr, reinterpret_cast<ULONG_PTR>(m_DirectoryHandle.GetHandle()), 1));
			ME_ASSERTF(m_DirectoryIOCP, "Error: 0x%X", GetLastError());
			if(m_DirectoryIOCP)
				m_Thread = std::thread(&DirectoryWatcher::AsyncCheckForChanges, this);
		}
	}

	DirectoryWatcher::~DirectoryWatcher()
	{
		m_IsRunning = false;
		if (m_DirectoryHandle != INVALID_HANDLE_VALUE)
		{
			CancelIo(m_DirectoryHandle);
			if(m_Thread.joinable())
				m_Thread.join();
		}
	}

	void DirectoryWatcher::CheckForChanges()
	{
		m_ChangedFilesMutex.lock();
		for (auto& kvp : m_ChangedFiles)
		{
			ME_LOG_INFO("File changed: %s", kvp.second->m_FileName.c_str());
			EventBus::GetInstance()->RaiseEvent(kvp.second);
		}
		m_ChangedFiles.clear();
		m_ChangedFilesMutex.unlock();
	}
	
	void DirectoryWatcher::AsyncCheckForChanges()
	{
		while (m_IsRunning)
		{
			if (!ReadDirectoryChangesW(m_DirectoryHandle, m_FileNotifyInfoBuffer, sizeof(m_FileNotifyInfoBuffer), FALSE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION, nullptr, &m_OL, nullptr))
			{
				ME_LOG_ERROR("Failed to read directory changes. Last error: 0x%X", GetLastError());
				return;
			}

			DWORD		bytes = 0;
			ULONG_PTR	key = 0;
			OVERLAPPED*	pOl;
			if (!GetQueuedCompletionStatus(m_DirectoryIOCP, &bytes, &key, &pOl, 500))
			{
				if (GetLastError() == WAIT_TIMEOUT)
					continue;
				ME_LOG_ERROR("Failed to get IO completion status. Last error: 0x%X", GetLastError());
				return;
			}

			ME_ASSERT(key == reinterpret_cast<ULONG_PTR>(m_DirectoryHandle.GetHandle()));

			m_ChangedFilesMutex.lock();
			PFILE_NOTIFY_INFORMATION notifyInfo = nullptr;
			DWORD offset = 0;
			do
			{
				notifyInfo = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(&m_FileNotifyInfoBuffer[offset]);
				offset += notifyInfo->NextEntryOffset;

				std::wstring changedFileNameUTF16;
				changedFileNameUTF16.append(notifyInfo->FileName, notifyInfo->FileNameLength / sizeof(WCHAR));
				std::string changedFileName = UTF16ToUTF8(changedFileNameUTF16);

				if (notifyInfo->Action == FILE_ACTION_REMOVED || notifyInfo->Action == FILE_ACTION_RENAMED_OLD_NAME)
					m_ChangedFiles[changedFileName] = std::make_shared<FileChangedEvent>(m_Folder + changedFileName, true);
				else if (notifyInfo->Action == FILE_ACTION_MODIFIED || notifyInfo->Action == FILE_ACTION_RENAMED_NEW_NAME)
				    m_ChangedFiles[changedFileName] = std::make_shared<FileChangedEvent>(m_Folder + changedFileName, false);
			} while (notifyInfo->NextEntryOffset != 0);

			m_ChangedFilesMutex.unlock();
		}
	}
};