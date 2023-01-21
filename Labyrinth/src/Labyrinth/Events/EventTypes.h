#pragma once

#include "Labyrinth/Core/System/Base.h"

namespace Laby {

	using EventTypeFlag = u32;

	// Use enum in namespace over enum class so they can be used in bitwise operations but remain properly scoped.
	namespace EventType {

		enum Flag : u32
		{	// New events must be added to AllEvents struct in same order
			None = 0,

			WindowClose			= LAB_BIT(0), 
			WindowResize		= LAB_BIT(1), 
			WindowFocus			= LAB_BIT(2), 
			WindowLostFocus		= LAB_BIT(3), 
			WindowMoved			= LAB_BIT(4),

			AppTick				= LAB_BIT(5), 
			AppUpdate			= LAB_BIT(6), 
			AppRender			= LAB_BIT(7),

			KeyPressed			= LAB_BIT(8), 
			KeyReleased			= LAB_BIT(9), 
			KeyTyped			= LAB_BIT(10),

			MouseButtonPressed	= LAB_BIT(11), 
			MouseButtonReleased	= LAB_BIT(12), 
			MouseMoved			= LAB_BIT(13), 
			MouseScrolled		= LAB_BIT(14),

			NetMessage			= LAB_BIT(15)
		};

		static constexpr EventTypeFlag EVENT_CATEGORY_APP	= EventType::WindowClose | EventType::WindowResize | EventType::WindowFocus
															| EventType::WindowLostFocus | EventType::WindowMoved
															| EventType::AppTick | EventType::AppUpdate | EventType::AppRender;
		static constexpr EventTypeFlag EVENT_CATEGORY_KEY	= EventType::KeyPressed | EventType::KeyReleased | EventType::KeyTyped;
		static constexpr EventTypeFlag EVENT_CATEGORY_MOUSE = EventType::MouseButtonPressed | EventType::MouseButtonReleased | EventType::MouseMoved | EventType::MouseScrolled;
		static constexpr EventTypeFlag EVENT_CATEGORY_INPUT = EVENT_CATEGORY_KEY | EVENT_CATEGORY_MOUSE;
	}

#define EVENT_DATA_TYPE(type)	static EventType::Flag GetStaticType() { return EventType::type; }

	// Add extra events here also
#define EVENT_TYPE1(a)									EventType::##a
#define EVENT_TYPE2(a,b)								EventType::##a | EventType::##b
#define EVENT_TYPE3(a,b,c)								EventType::##a | EventType::##b | EventType::##c
#define EVENT_TYPE4(a,b,c,d)							EventType::##a | EventType::##b | EventType::##c | EventType::##d
#define EVENT_TYPE5(a,b,c,d,e)							EventType::##a | EventType::##b | EventType::##c | EventType::##d | EventType::##e
#define EVENT_TYPE6(a,b,c,d,e,f)						EventType::##a | EventType::##b | EventType::##c | EventType::##d | EventType::##e | EventType::##f
#define EVENT_TYPE7(a,b,c,d,e,f,g)						EventType::##a | EventType::##b | EventType::##c | EventType::##d | EventType::##e | EventType::##f | EventType::##g
#define EVENT_TYPE8(a,b,c,d,e,f,g,h)					EventType::##a | EventType::##b | EventType::##c | EventType::##d | EventType::##e | EventType::##f | EventType::##g | EventType::##h
#define EVENT_TYPE9(a,b,c,d,e,f,g,h,i)					EventType::##a | EventType::##b | EventType::##c | EventType::##d | EventType::##e | EventType::##f | EventType::##g | EventType::##h | EventType::##i
#define EVENT_TYPE10(a,b,c,d,e,f,g,h,i,j)				EventType::##a | EventType::##b | EventType::##c | EventType::##d | EventType::##e | EventType::##f | EventType::##g | EventType::##h | EventType::##i | EventType::##j
#define EVENT_TYPE11(a,b,c,d,e,f,g,h,i,j,k)				EventType::##a | EventType::##b | EventType::##c | EventType::##d | EventType::##e | EventType::##f | EventType::##g | EventType::##h | EventType::##i | EventType::##j | EventType::##k
#define EVENT_TYPE12(a,b,c,d,e,f,g,h,i,j,k,l)			EventType::##a | EventType::##b | EventType::##c | EventType::##d | EventType::##e | EventType::##f | EventType::##g | EventType::##h | EventType::##i | EventType::##j | EventType::##k | EventType::##l
#define EVENT_TYPE13(a,b,c,d,e,f,g,h,i,j,k,l,m)			EventType::##a | EventType::##b | EventType::##c | EventType::##d | EventType::##e | EventType::##f | EventType::##g | EventType::##h | EventType::##i | EventType::##j | EventType::##k | EventType::##l | EventType::##m
#define EVENT_TYPE14(a,b,c,d,e,f,g,h,i,j,k,l,m,n)		EventType::##a | EventType::##b | EventType::##c | EventType::##d | EventType::##e | EventType::##f | EventType::##g | EventType::##h | EventType::##i | EventType::##j | EventType::##k | EventType::##l | EventType::##m | EventType::##n
#define EVENT_TYPE15(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o)		EventType::##a | EventType::##b | EventType::##c | EventType::##d | EventType::##e | EventType::##f | EventType::##g | EventType::##h | EventType::##i | EventType::##j | EventType::##k | EventType::##l | EventType::##m | EventType::##n | EventType::##o
#define EVENT_TYPE16(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p)	EventType::##a | EventType::##b | EventType::##c | EventType::##d | EventType::##e | EventType::##f | EventType::##g | EventType::##h | EventType::##i | EventType::##j | EventType::##k | EventType::##l | EventType::##m | EventType::##n | EventType::##o | EventType::##p
#define GET_TYPE(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, EVENT_TYPEN, ...) EVENT_TYPEN
#define EXPAND_EVENTS(...) LAB_EXPAND_MACRO(GET_TYPE(__VA_ARGS__, EVENT_TYPE16, EVENT_TYPE15, EVENT_TYPE14, EVENT_TYPE13, EVENT_TYPE12, EVENT_TYPE11, EVENT_TYPE10, EVENT_TYPE9, EVENT_TYPE8, EVENT_TYPE7, EVENT_TYPE6, EVENT_TYPE5, EVENT_TYPE4, EVENT_TYPE3, EVENT_TYPE2, EVENT_TYPE1)(__VA_ARGS__))
}