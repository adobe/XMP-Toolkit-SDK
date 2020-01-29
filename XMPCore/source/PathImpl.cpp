// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCore/ImplHeaders/PathImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/XMPCoreErrorCodes.h"
#include "XMPCommon/Interfaces/IUTF8String_I.h"
#include "XMPCore/Interfaces/IPathSegment.h"
#include "XMPCore/Interfaces/INameSpacePrefixMap_I.h"
#include "XMPCommon/Utilities/UTF8String.h"
#include "XMPCommon/Utilities/TSmartPointers_I.h"

namespace AdobeXMPCore_Int {

	// All virtual functions

	spcINameSpacePrefixMap APICALL PathImpl::RegisterNameSpacePrefixMap( const spcINameSpacePrefixMap & map ) {
		spcINameSpacePrefixMap old = mNameSpacePrefixMap;
		mNameSpacePrefixMap = map->Clone();
		return old;
	}

	static const char * sSegmentSeperator = "/";
	static const char * sValueSeperator = ":";
	static const char * sQualifierIndicator = "@";
	static const char * sValueIndicatorBegin = "[";
	static const char * sValueIndicatorEnd = "]";
	static const char * sQualifierValueIndicator = "?";
	static const char * sQualifierValueSeperator = "=";
	static const char * sQuotes = "\"";

	spIUTF8String APICALL PathImpl::Serialize( const spcINameSpacePrefixMap & map ) const {
		bool firstSegment = true;

		auto it = mSegments.begin();
		auto endIt = mSegments.end();
		spIUTF8String serailizedOutput = IUTF8String_I::CreateUTF8String();

		for (it = mSegments.begin(); it != endIt; it++) {
			spcIUTF8String nameSpace = it->get()->GetNameSpace();
			spcIUTF8String nameSpaceOrPrefix;

			if (!map && !mNameSpacePrefixMap)
				nameSpaceOrPrefix = nameSpace;

			if (map)
				nameSpaceOrPrefix = map->GetINameSpacePrefixMap_I()->GetPrefix(nameSpace);
			if (!nameSpaceOrPrefix && mNameSpacePrefixMap)
				nameSpaceOrPrefix = mNameSpacePrefixMap->GetINameSpacePrefixMap_I()->GetPrefix(nameSpace);
			
			if (!nameSpaceOrPrefix)
				NOTIFY_ERROR(IError_v1::kEDDataModel, kDMECNameSpacePrefixMapEntryMissing,
				"A required entry missing in the provided mapping table", IError_v1::kESOperationFatal, false, false);

			switch (it->get()->GetType()) {
			case IPathSegment_v1::kPSTProperty:
				if (!firstSegment) {
					serailizedOutput->append(sSegmentSeperator, npos);
				}
				serailizedOutput->append(nameSpaceOrPrefix)->append(sValueSeperator, npos)->append(it->get()->GetName());
				break;

			case IPathSegment_v1::kPSTArrayIndex:
			{
				//std::string strIndex = std::to_string(it->get()->GetIndex());
                std::ostringstream oss;
                oss << (it->get()->GetIndex());
                std::string strIndex = oss.str();
				serailizedOutput->append(sValueIndicatorBegin, npos)->append(strIndex.c_str(), strIndex.size())->append(sValueIndicatorEnd, npos);
			}
			break;

			case IPathSegment_v1::kPSTQualifier:
				if (!firstSegment) {
					serailizedOutput->append(sSegmentSeperator, npos);
				}
				serailizedOutput->append(sQualifierIndicator, npos)->append(nameSpaceOrPrefix)->append(sValueSeperator, npos)->append(it->get()->GetName());
				break;

			case IPathSegment_v1::kPSTQualifierSelector:
				serailizedOutput->append(sValueIndicatorBegin, npos)->append(sQualifierValueIndicator, npos)->append(it->get()->GetName())->
					append(sQualifierValueSeperator, npos)->append(sQuotes, npos)->append(it->get()->GetValue())->
					append(sQuotes, npos)->append(sValueIndicatorEnd, npos);

			default:
				break;
			}

			if (firstSegment) {
				firstSegment = false;
			}
		}

		return serailizedOutput;
		
	}

	void APICALL PathImpl::AppendPathSegment( const spcIPathSegment & segment ) {
		if ( !segment )
			NOTIFY_ERROR( IError_v1::kEDGeneral, kGECParametersNotAsExpected, 
				"Parameters to PathImpl::AppendPathSegment are not as expected", IError_v1::kESOperationFatal,
				true, ( void * ) segment.get() );

		mSegments.push_back( segment );
	}

	spcIPathSegment APICALL PathImpl::RemovePathSegment( sizet index ) {
		if ( index - 1 >= mSegments.size() ) {
			NOTIFY_ERROR( IError_v1::kEDGeneral, kGECIndexOutOfBounds,
				"Requested Index to PathImpl::RemovePathSegment is out of bounds", IError_v1::kESOperationFatal, true, Size(), true, index );
		} else {
			spcIPathSegment returnValue = mSegments[ index - 1 ];
			mSegments.erase( mSegments.begin() + index - 1 );
			return returnValue;
		}
		return spcIPathSegment();
	}

	spcIPathSegment APICALL PathImpl::GetPathSegment( sizet index ) const {
		if ( index - 1 >= mSegments.size() ) {
			NOTIFY_ERROR( IError_v1::kEDGeneral, kGECIndexOutOfBounds,
				"Requested Index to PathImpl::GetPathSegment is out of bounds", IError_v1::kESOperationFatal, true, Size(), true, index );
		} else {
			return mSegments[ index - 1 ];
		}
		return spcIPathSegment();
	}

	AdobeXMPCommon::sizet PathImpl::Size() const __NOTHROW__ {
		return mSegments.size();
	}

	void APICALL PathImpl::Clear() __NOTHROW__ {
		mSegments.clear();
	}

	spIPath APICALL PathImpl::Clone( sizet startingIndex, sizet countOfSegments ) const {
		sizet totalCountOfSegments = mSegments.size();

		if ( startingIndex - 1 >= totalCountOfSegments ) {
			NOTIFY_ERROR( IError_v1::kEDGeneral, kGECIndexOutOfBounds,
				"Requested startIndex to PathImpl::Clone is out of bounds", IError_v1::kESOperationFatal, true, Size(), true, startingIndex );
		}
		spIPath newPath = MakeUncheckedSharedPointer( new PathImpl(), __FILE__, __LINE__, true );

		for ( sizet index = startingIndex; countOfSegments != 0; countOfSegments--, index++ ) {
			newPath->AppendPathSegment( mSegments[ index - 1 ] );
			if ( index == totalCountOfSegments )
				break;
		}
		if ( mNameSpacePrefixMap )
			newPath->RegisterNameSpacePrefixMap( mNameSpacePrefixMap->Clone() );
		return newPath;
	}

}

#if BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB
namespace AdobeXMPCore {
	using namespace AdobeXMPCore_Int;

	spIPath IPath_v1::MakeShared( pIPath_base ptr ) {
		if ( !ptr ) return spIPath();
		pIPath p = IPath::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IPath >() : ptr;
		return MakeUncheckedSharedPointer( p, __FILE__, __LINE__, false );
	}

	spIPath IPath_v1::CreatePath() {
		return MakeUncheckedSharedPointer( new PathImpl(), __FILE__, __LINE__, true );
	}

	spIPath IPath_v1::ParsePath( const char * path, sizet pathLength, const spcINameSpacePrefixMap & map ) {
		NOTIFY_ERROR( IError::kEDGeneral, kGECNotImplemented, "API is not implemented", IError::kESOperationFatal, true, "ParsePath" );
	}

}
#endif  // BUILDING_XMPCORE_LIB || SOURCE_COMPILING_XMPCORE_LIB
