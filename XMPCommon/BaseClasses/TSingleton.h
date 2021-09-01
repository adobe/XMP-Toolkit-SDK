#ifndef __TSingletonImpl_h__
#define __TSingletonImpl_h__ 1

// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "XMPCommon/XMPCommonDefines_I.h"
#include <stdexcept>
#include <cstdlib>

namespace AdobeXMPCommon_Int {

	template< typename T >
	class TSingleThreaded {
	public:
		typedef T VolatileType;

	protected:
		inline explicit TSingleThreaded() {}
		inline ~TSingleThreaded() {}

	protected:
		class LockThread {
		public:
			inline explicit LockThread() {
				TSingleThreaded::Lock();
			}
			inline ~LockThread() {
				TSingleThreaded::Unlock();
			}

		private:
			inline explicit LockThread( const LockThread & ) {}
			inline LockThread & operator=( const LockThread & ) { return *this; }
		};

	private:
		friend class LockThread;

		inline static void Lock() {}
		inline static void Unlock() {}

	private:
		inline explicit TSingleThreaded( const TSingleThreaded & ) {}
		inline TSingleThreaded & operator=( const TSingleThreaded & ) { return *this; }
	};

	template< typename T >
	class TDefaultLifetime;

	template< typename T >
	class TInitTermiateControlledLifeTime;

	template< typename T >
	class TCreateUsingNew;

	template< typename T, typename CreationPolicy = TCreateUsingNew<T>,
		template < typename > class LifetimePolicy = TInitTermiateControlledLifeTime,
		template < typename > class ThreadingModel = TSingleThreaded >
	class TSingleton
		: public CreationPolicy,
		public LifetimePolicy<T>,
		public ThreadingModel<T>
	{
	public:
		typedef T &		reference;
		typedef T *		pointer;

		static reference Instance() {
			if ( TSingleton::mInstance == 0 ) {
				typename ThreadingModel<T>::LockThread lock;
				if ( TSingleton::mInstance == 0 ) {
					if ( TSingleton::mDestroyed ) {
						LifetimePolicy<T>::OnDeadReference();
						TSingleton::mDestroyed = false;
					}
					TSingleton::mInstance = CreationPolicy::CreateInstance();
					try {
						LifetimePolicy<T>::ScheduleForDestruction( TSingleton::Destroy );
					} catch( ... ) {
						CreationPolicy::DestroyInstance( TSingleton::mInstance );
					}
				}
			}
			return *(TSingleton::mInstance);
		}
		static void Destroy();

	protected:
		inline explicit TSingleton() {
			assert(TSingleton::mInstance == 0);
			TSingleton::mInstance = static_cast< pointer >( this );
			TSingleton::mDestroyed = false;
		}
		inline ~TSingleton() {
			TSingleton::mInstance = 0;
			TSingleton::mDestroyed = true;
		}

	private:
		static pointer										mInstance;
		static bool											mDestroyed;

	private:
		inline explicit TSingleton( const TSingleton & ) {}
		inline TSingleton & operator=( const TSingleton & ) { return *this; }
	};

	template< typename Ty, typename C, template < typename > class L, template < typename > class T >
	void TSingleton< Ty, C, L, T >::Destroy() {
		if ( TSingleton::mInstance != 0 ) {
			typename T<Ty>::LockThread lock;
			if ( TSingleton::mInstance != 0 ) {
				C::DestroyInstance(TSingleton::mInstance);
				TSingleton::mInstance = 0;
				TSingleton::mDestroyed = true;
			}
		}
	}

	template< typename Ty, typename C, template < typename > class L, template < typename > class T >
	typename TSingleton<Ty, C, L, T>::pointer TSingleton<Ty, C, L, T>::mInstance = 0;
	//typename TSingleton<Ty, C, L, T>::template T<Ty>::VolatileType * TSingleton<Ty, C, L, T>::mInstance = 0;

	template< typename Ty, typename C, template < typename > class L, template < typename > class T >
	bool TSingleton<Ty, C, L, T>::mDestroyed = false;

	template< typename T >
	class TDefaultLifetime {
	protected:
		inline explicit TDefaultLifetime() {}
		inline ~TDefaultLifetime() {}

		inline static void OnDeadReference() {
			throw std::logic_error("Dead Reference Detected"); 
		}
		inline static void ScheduleForDestruction(void (*pFun)()) {
			std::atexit(pFun); 
		}

	private:
		inline explicit TDefaultLifetime( const TDefaultLifetime & ) {}
		inline TDefaultLifetime & operator=( const TDefaultLifetime & ) { return *this; }
	};

	template< typename T >
	class TCreateUsingNew {
	public:
		typedef T * pointer;
	protected :
		inline explicit TCreateUsingNew() {}
		inline ~TCreateUsingNew() {}

		inline static pointer CreateInstance() { return new T(); }
		inline static void DestroyInstance(pointer t) { delete t; }

	private : 
		inline explicit TCreateUsingNew( const TCreateUsingNew & ) {}
		inline TCreateUsingNew & operator=( const TCreateUsingNew & ) { return *this; }
	};

	template< typename T >
	class TInitTermiateControlledLifeTime {
	protected:
		inline explicit TInitTermiateControlledLifeTime() {}
		inline ~TInitTermiateControlledLifeTime() {}

		inline static void OnDeadReference() {
			//do nothing
		}
		inline static void ScheduleForDestruction(void (*pFun)()) {
			
		}

	private:
		inline explicit TInitTermiateControlledLifeTime( const TDefaultLifetime< T > & ) {}
		inline TInitTermiateControlledLifeTime & operator=( const TDefaultLifetime< T > & ) { return *this; }
	};

}
#endif  // __TSingletonImpl_h__

