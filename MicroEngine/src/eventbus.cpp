//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "eventbus.h"

namespace me
{
	/*static*/ EventBus* EventBus::GetInstance()
	{
		static EventBus s_EventBus;
		return &s_EventBus;
	}

	void EventBus::RaiseEvent(const std::shared_ptr<IEvent>& eventData)
	{
		auto& eventHandlers = m_EventHandlersByID[eventData->GetID()];
		for (auto& eventHandler : eventHandlers)
		{
			eventHandler.m_CB(eventData);
		}
	}

	void EventBus::Register(EventID id, size_t callerUID, const OnEventCB& cb)
	{
		auto& eventHandlers = m_EventHandlersByID[id];
        const auto it = std::find_if(eventHandlers.begin(), eventHandlers.end(), [callerUID](EventHandler& handler) { return handler.m_UniqueID == callerUID;  });
		if (it != eventHandlers.end())
		{
			ME_LOG_WARNING("Eventhandler already registered");
		}
		eventHandlers.push_back(EventHandler{ callerUID, cb });
	}

	void EventBus::Unregister(EventID id, size_t callerUID)
	{
		auto& eventHandlers = m_EventHandlersByID[id];
        const auto it = std::find_if(eventHandlers.begin(), eventHandlers.end(), [callerUID](EventHandler& handler) { return handler.m_UniqueID == callerUID;  });
		if (it != eventHandlers.end())
		{
			eventHandlers.erase(it);
		}
	}
};