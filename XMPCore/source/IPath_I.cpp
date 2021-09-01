// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================


#include "XMPCore/Interfaces/IPath_I.h"
#include "XMPCommon/Utilities/TWrapperFunctions_I.h"
#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/Interfaces/INameSpacePrefixMap.h"
#include "XMPCore/Interfaces/IPathSegment.h"
#include "XMPCommon/Interfaces/IUTF8String.h"

namespace AdobeXMPCore_Int {

	pvoid APICALL IPath_I::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return GetInterfacePointerInternal( interfaceID, interfaceVersion, true );
	}

	pvoid APICALL IPath_I::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunction< IPath_I, pvoid, pvoid, uint64, uint32 >(
			error, this, NULL, &IPath_I::GetInterfacePointer, __FILE__, __LINE__, interfaceID, interfaceVersion );
	}

	pvoid APICALL IPath_I::GetInterfacePointerInternal( uint64 interfaceID, uint32 interfaceVersion, bool isTopLevel ) {
		if ( interfaceID == kIPathID ) {
			switch ( interfaceVersion ) {
			case 1:
				return static_cast< IPath_v1 * >( this );
				break;

			case kInternalInterfaceVersionNumber:
				return this;
				break;

			default:
				throw IError_I::CreateInterfaceVersionNotAvailableError(
					IError_v1::kESOperationFatal, interfaceID, interfaceVersion, __FILE__, __LINE__ );
				break;
			}
		}
		if ( isTopLevel )
			throw IError_I::CreateInterfaceNotAvailableError(
				IError_v1::kESOperationFatal, kIPathID, interfaceID, __FILE__, __LINE__ );
		return NULL;
	}

	pcINameSpacePrefixMap_base APICALL IPath_I::registerNameSpacePrefixMap( pcINameSpacePrefixMap_base map, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IPath_v1, pcINameSpacePrefixMap_base, const INameSpacePrefixMap, const spcINameSpacePrefixMap & >(
			error, this, &IPath_v1::RegisterNameSpacePrefixMap, __FILE__, __LINE__, INameSpacePrefixMap::MakeShared( map ) );
	}

	pIUTF8String_base APICALL IPath_I::serialize( pcINameSpacePrefixMap_base map, pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< IPath_v1, pIUTF8String_base, IUTF8String, const spcINameSpacePrefixMap & >(
			error, this, &IPath_v1::Serialize, __FILE__, __LINE__, INameSpacePrefixMap::MakeShared( map ) );
	}

	void APICALL IPath_I::appendPathSegment( pcIPathSegment_base segment, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningVoid< IPath_v1, const spcIPathSegment &>(
			error, this, &IPath_v1::AppendPathSegment, __FILE__, __LINE__, IPathSegment::MakeShared( segment ) );
	}

	pcIPathSegment_base APICALL IPath_I::removePathSegment( sizet index, pcIError_base & error ) __NOTHROW__ {
		return CallUnSafeFunctionReturningSharedPointer< IPath_v1, pcIPathSegment_base, const IPathSegment, sizet >(
			error, this, &IPath_v1::RemovePathSegment, __FILE__, __LINE__, index );
	}

	pcIPathSegment_base APICALL IPath_I::getPathSegment( sizet index, pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< IPath_v1, pcIPathSegment_base, const IPathSegment, sizet >(
			error, this, &IPath_v1::GetPathSegment, __FILE__, __LINE__, index );
	}

	pIPath_base APICALL IPath_I::clone( sizet startingIndex, sizet countOfSegemetns, pcIError_base & error ) const __NOTHROW__ {
		return CallConstUnSafeFunctionReturningSharedPointer< IPath_v1, pIPath_base, IPath, sizet, sizet >(
			error, this, &IPath_v1::Clone, __FILE__, __LINE__, startingIndex, countOfSegemetns );
	}

}
