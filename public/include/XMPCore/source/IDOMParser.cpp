// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

namespace AdobeXMPCore {
	class IDOMParserProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCore::IDOMParserProxy;

#include "XMPCore/Interfaces/IDOMParser.h"

#if !BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include <assert.h>
#include "XMPCore/Interfaces/IMetadata.h"

namespace AdobeXMPCore {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	class IDOMParserProxy
		: public virtual IDOMParser
		, public virtual IConfigurableProxy
	{
	private:
		pIDOMParser mRawPtr;

	public:
		IDOMParserProxy( pIDOMParser ptr )
			: IConfigurableProxy( ptr )
			, mRawPtr( ptr )
		{
			mRawPtr->Acquire();
		}

		~IDOMParserProxy() __NOTHROW__ { mRawPtr->Release(); }

		pIDOMParser APICALL GetActualIDOMParser() __NOTHROW__ { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ { assert( false ); }

		void APICALL Release() const __NOTHROW__ { assert( false ); }

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
			return mRawPtr->GetISharedObject_I();
		}

		AdobeXMPCore_Int::pIDOMParser_I APICALL GetIDOMParser_I() __NOTHROW__ {
			return mRawPtr->GetIDOMParser_I();
		}

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
		}

		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
		}

		virtual spIDOMParser APICALL Clone() const {
			return CallConstSafeFunctionReturningPointer< IDOMParser, pIDOMParser_base, IDOMParser >(
				mRawPtr, &IDOMParser::clone );
		}

		virtual spIMetadata APICALL Parse( const char * buffer, sizet bufferLength ) {
			return CallSafeFunctionReturningPointer< IDOMParser, pIMetadata_base, IMetadata, const char *, sizet >(
				mRawPtr, &IDOMParser::parse, buffer, bufferLength );
		}

		virtual void APICALL ParseWithSpecificAction( const char * buffer, sizet bufferLength, eActionType actionType, spINode & node ) {
			return CallSafeFunctionReturningVoid< IDOMParser, const char *, sizet, uint32, pINode_base >(
				mRawPtr, &IDOMParser::parseWithSpecificAction, buffer, bufferLength, static_cast< uint32 >( actionType ), node ? node->GetActualINode() : NULL );
		}

		virtual pIDOMParser_base APICALL clone( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->clone( error );
		}

		virtual pIMetadata_base APICALL parse( const char * buffer, sizet bufferLength, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->parse( buffer, bufferLength, error );
		}

		virtual void APICALL parseWithSpecificAction( const char * buffer, sizet bufferLength, uint32 actionType, pINode_base node, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->parseWithSpecificAction( buffer, bufferLength, actionType, node, error );
		}

	};

#if XMP_WinBuild
	#pragma warning( pop )
#endif

	spIDOMParser IDOMParser_v1::MakeShared( pIDOMParser_base ptr ) {
		if ( !ptr ) return spIDOMParser();
		pIDOMParser p = IDOMParser::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IDOMParser >() : ptr;
		return shared_ptr< IDOMParser >( new IDOMParserProxy( p ) );
	}

}

#endif  // BUILDING_XMPCORE_LIB && !SOURCE_COMPILING_XMPCORE_LIB
