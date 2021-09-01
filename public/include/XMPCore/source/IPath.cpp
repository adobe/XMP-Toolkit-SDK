// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

namespace AdobeXMPCore {
	class IPathProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCore::IPathProxy;

#include "XMPCore/Interfaces/IPath.h"

#if !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCore/Interfaces/INameSpacePrefixMap.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPCore/Interfaces/IPathSegment.h"
#include "XMPCore/Interfaces/ICoreObjectFactory.h"

#include <assert.h>

namespace AdobeXMPCore {

	class IPathProxy
		: public virtual IPath
	{
	private:
		pIPath mRawPtr;

	public:
		IPathProxy( pIPath ptr )
			: mRawPtr( ptr )
		{
			mRawPtr->Acquire();
		}

		~IPathProxy() __NOTHROW__ { mRawPtr->Release(); }

		pIPath APICALL GetActualIPath() __NOTHROW__ { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ { assert( false ); }

		void APICALL Release() const __NOTHROW__ { assert( false ); }

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
			return mRawPtr->GetISharedObject_I();
		}

		AdobeXMPCore_Int::pIPath_I APICALL GetIPath_I() __NOTHROW__ {
			return mRawPtr->GetIPath_I();
		}

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
		}

		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
		}

		virtual spcINameSpacePrefixMap APICALL RegisterNameSpacePrefixMap( const spcINameSpacePrefixMap & map ) {
			return CallSafeFunctionReturningPointer< IPath_v1, pcINameSpacePrefixMap_base, const INameSpacePrefixMap, pcINameSpacePrefixMap_base >(
				mRawPtr, &IPath_v1::registerNameSpacePrefixMap, map ? map->GetActualINameSpacePrefixMap() : NULL );
		}

		virtual pcINameSpacePrefixMap_base APICALL registerNameSpacePrefixMap( pcINameSpacePrefixMap_base map, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->registerNameSpacePrefixMap( map, error );
		}

		virtual spIUTF8String APICALL Serialize( const spcINameSpacePrefixMap & map ) const {
			return CallConstSafeFunctionReturningPointer< IPath_v1, pIUTF8String_base, IUTF8String, pcINameSpacePrefixMap_base >(
				mRawPtr, &IPath_v1::serialize, map ? map->GetActualINameSpacePrefixMap() : NULL );
		}

		virtual pIUTF8String_base APICALL serialize( pcINameSpacePrefixMap_base map, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->serialize( map, error );
		}

		virtual void APICALL AppendPathSegment( const spcIPathSegment & segment ) {
			return CallSafeFunctionReturningVoid< IPath_v1, pcIPathSegment_base >(
				mRawPtr, &IPath_v1::appendPathSegment, segment ? segment->GetActualIPathSegment() : NULL );
		}

		virtual void APICALL appendPathSegment( pcIPathSegment_base segment, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->appendPathSegment( segment, error );
		}

		virtual spcIPathSegment APICALL RemovePathSegment( sizet index ) {
			return CallSafeFunctionReturningPointer< IPath_v1, pcIPathSegment_base, const IPathSegment, sizet >(
				mRawPtr, &IPath_v1::removePathSegment, index );
		}

		virtual pcIPathSegment_base APICALL removePathSegment( sizet index, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->removePathSegment( index, error );
		}

		virtual spcIPathSegment APICALL GetPathSegment( sizet index ) const {
			return CallConstSafeFunctionReturningPointer< IPath_v1, pcIPathSegment_base, const IPathSegment, sizet >(
				mRawPtr, &IPath_v1::getPathSegment, index );
		}

		virtual pcIPathSegment_base APICALL getPathSegment( sizet index, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getPathSegment( index, error );
		}

		virtual sizet APICALL Size() const __NOTHROW__ {
			return mRawPtr->Size(  );
		}

		virtual void APICALL Clear() __NOTHROW__ {
			return mRawPtr->Clear(  );
		}

		virtual spIPath APICALL Clone( sizet startingIndex, sizet countOfSegments ) const {
			return CallConstSafeFunctionReturningPointer< IPath_v1, pIPath_base, IPath, sizet, sizet >(
				mRawPtr, &IPath_v1::clone, startingIndex, countOfSegments );
		}

		virtual pIPath_base APICALL clone( sizet startingIndex, sizet countOfSegemetns, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->clone( startingIndex, countOfSegemetns, error );
		}

	};

	spIPath IPath_v1::MakeShared( pIPath_base ptr ) {
		if ( !ptr ) return spIPath();
		pIPath p = IPath::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IPath >() : ptr;
		return shared_ptr< IPath >( new IPathProxy( p ) );
	}

	spIPath IPath_v1::CreatePath() {
		return CallSafeFunctionReturningPointer< ICoreObjectFactory, pIPath_base, IPath >(
			ICoreObjectFactory::GetCoreObjectFactory(), &ICoreObjectFactory::CreatePath );
	}

	spIPath IPath_v1::ParsePath( const char * path, sizet pathLength, const spcINameSpacePrefixMap & map ) {
		return CallSafeFunctionReturningPointer< ICoreObjectFactory, pIPath_base, IPath, const char *, sizet, pcINameSpacePrefixMap_base >(
			ICoreObjectFactory::GetCoreObjectFactory(), &ICoreObjectFactory::ParsePath, path, pathLength, map ? map->GetActualINameSpacePrefixMap() : NULL );
	}

}

#endif  // BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB
