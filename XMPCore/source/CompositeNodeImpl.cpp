// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#define IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED 1
	#include "XMPCore/ImplHeaders/CompositeNodeImpl.h"
#undef IMPLEMENTATION_HEADERS_CAN_BE_INCLUDED

#include "XMPCommon/Interfaces/IError_I.h"
#include "XMPCore/XMPCoreErrorCodes.h"
#include "XMPCore/Interfaces/IPath.h"
#include "XMPCore/Interfaces/IPathSegment.h"
#include "XMPCommon/Utilities/TSmartPointers_I.h"
#include "XMPCommon/Interfaces/IUTF8String_I.h"
#include "XMPCore/Interfaces/ISimpleNode.h"
#include "XMPCore/Interfaces/IStructureNode_I.h"
#include "XMPCore/Interfaces/IArrayNode.h"
#include "XMPCore/Interfaces/INodeIterator.h"

namespace AdobeXMPCore_Int {

	static spISimpleNode GetArrayItemBasedOnSimpleQual( const spIArrayNode & arrayNode, const spcIUTF8String & nameSpace, const spcIUTF8String & name, const spcIUTF8String & value ) {
		spINodeIterator it = arrayNode->Iterator();
		while ( it ) {
			spINode node = it->GetNode();
			if ( node->HasQualifiers() ) {
				spISimpleNode simpleQualifer = node->GetINode_I()->GetSimpleQualifier( nameSpace, name );
				if ( simpleQualifer ) {
					if ( value->compare( simpleQualifer->GetValue() ) == 0 )
						return simpleQualifer;
				}
			}
			it = it->Next();
		}
		return spISimpleNode();
	}

	INode_v1::eNodeType APICALL CompositeNodeImpl::GetNodeTypeAtPath( const spcIPath & path ) const {
		auto node = const_cast< CompositeNodeImpl * >( this )->GetNodeAtPath( path );
		if ( node ) return node->GetNodeType();
		return INode_v1::kNTNone;
	}

	spINode APICALL CompositeNodeImpl::GetNodeAtPath( const spcIPath & path ) {
		if ( !path )
			return spINode();

		spINode current = MakeUncheckedSharedPointer( this, __FILE__, __LINE__ );
		spINode parent = current, result = current;

		sizet segCount = path->Size();
		for ( sizet i = 0; i < segCount && current; ++i ) {
			spcIPathSegment segment = path->GetPathSegment( i + 1 );

			switch ( segment->GetType() ) {

			case IPathSegment::kPSTProperty:
				{
					eNodeType nodeType = parent->GetNodeType();
					switch (nodeType)
					{
						case kNTSimple:
						{
							auto node = parent->ConvertToSimpleNode();
							if (node)
								current = node;
							else
								current = spINode();
							break;
						}
						case kNTArray:
						{
							auto node = parent->ConvertToArrayNode ( );
							if ( node )
								current = node;
							else
								current = spINode ( );
							break;
						}
						case kNTStructure:
						{
							auto node = parent->ConvertToStructureNode();
							if (node)
								current = node->GetIStructureNode_I()->GetNode(segment->GetNameSpace(), segment->GetName());
							else
								current = spINode();
							break;
						}
						default:
							current = spINode();
							break;
					}
					
				}
				break;

			case IPathSegment::kPSTArrayIndex:
				{
					auto node = parent->ConvertToArrayNode();
					if ( node )
						current = node->GetNodeAtIndex( segment->GetIndex() );
					else
						current = spINode();
				}
				break;

			case IPathSegment::kPSTQualifier:
				if ( parent->HasQualifiers() ) {
					current = current->GetINode_I()->GetQualifier( segment->GetNameSpace(), segment->GetName() );
				} else {
					current = spINode();
				}
				break;

			case IPathSegment::kPSTQualifierSelector:
				{
					auto node = parent->ConvertToArrayNode();
					if ( node ) {
						current = GetArrayItemBasedOnSimpleQual( node, segment->GetNameSpace(), segment->GetName(), segment->GetValue() );
					} else {
						current = spINode();
					}
				}
				break;

			default:
				NOTIFY_ERROR( IError_v1::kEDGeneral, kGECLogicalError,
					"unhandled situtation occured", IError_v1::kESOperationFatal, true, static_cast< uint32 >( segment->GetType() ) );
				break;
			}

			parent = result = current;
		}
		return result;
	}

	void APICALL CompositeNodeImpl::InsertNodeAtPath( const spINode & node, const spcIPath & path ) {
		NOTIFY_ERROR( IError_v1::kEDGeneral, kGECNotImplemented,
			"InsertNodeAtPath( path ) is not yet implemented", IError_v1::kESOperationFatal, false, false );
	}

	spINode APICALL CompositeNodeImpl::ReplaceNodeAtPath( const spINode & node, const spcIPath & path ) {
		NOTIFY_ERROR( IError_v1::kEDGeneral, kGECNotImplemented,
			"ReplaceNodeAtPath( path ) is not yet implemented", IError_v1::kESOperationFatal, false, false );
	}

	spINode APICALL CompositeNodeImpl::RemoveNodeAtPath( const spcIPath & path ) {
		spINode node = GetNodeAtPath( path );
		if ( node ) {
			auto parentNode_I = node->GetINode_I()->GetRawParentPointer()->GetINode_I();
			if ( node->IsQualifierNode() ) {
				node = parentNode_I->ConvertToStructureNode()->GetIStructureNode_I()->RemoveNode( node->GetNameSpace(), node->GetName() );
			} else if ( node->IsArrayItem() ) {
				auto parentArrayNode = parentNode_I->ConvertToArrayNode();
				node = parentArrayNode->RemoveNodeAtIndex( node->GetIndex() );
			} else {
				auto parentStructureNode = parentNode_I->ConvertToStructureNode()->GetIStructureNode_I();
				node = parentStructureNode->RemoveNode( node->GetNameSpace(), node->GetName() );
			}
		}
		return node;
	}

	bool CompositeNodeImpl::CheckSuitabilityToBeUsedAsChildNode( const spcINode & node ) const {
		if ( node ) {
			if ( node->GetParentNodeType() == INode_v1::kNTNone ) {
				return true;
			} else {
				NOTIFY_ERROR( IError_v1::kEDDataModel, kDMECNodeAlreadyAChild,
					"node is already a part of tree", IError_v1::kESOperationFatal, false, false );
			}
		} else {
			NOTIFY_ERROR( IError_v1::kEDGeneral, kGECParametersNotAsExpected,
				"invalid shared pointer", IError_v1::kESOperationFatal, false, false );
		}
		return false;
	}

}
