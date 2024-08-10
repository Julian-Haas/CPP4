//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <map>
#include <functional>
#include <memory>
#include "util.h"
#include "me_interface.h"

namespace me
{
	typedef uint32_t EventID;
	namespace EngineEventID
	{
		enum Enum : EventID
		{
			FIRST,
			INVALID = FIRST,
			FileChanged,
			EntityChanged,
			WindowSizeChanged,
			COUNT
		};
	};

	class IEvent
	{
	public:
		IEvent() = default;
		virtual ~IEvent() = default;
		virtual EventID GetID() const = 0;
	private:
		ME_MOVE_COPY_NOT_ALLOWED(IEvent);
	};

	typedef std::function<void(const std::shared_ptr<IEvent>& event)> OnEventCB;

	class EventBus
	{
	public:
		ME_API static EventBus* GetInstance();

		ME_API EventBus() = default;
		ME_API ~EventBus() = default;

		ME_API void RaiseEvent(const std::shared_ptr<IEvent>& event);
		ME_API void Register(EventID id, size_t callerUID, const OnEventCB& cb);
		ME_API void Unregister(EventID id, size_t callerUID);
	private:
		ME_MOVE_COPY_NOT_ALLOWED(EventBus);

		struct EventHandler
		{
			size_t m_UniqueID;
			OnEventCB m_CB;
		};

		std::map<EventID, std::vector<EventHandler>> m_EventHandlersByID;
	};
};