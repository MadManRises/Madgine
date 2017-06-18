#pragma once

#include "message.h"

namespace Engine {
namespace Exceptions {

	const auto argumentCountMismatch = message("Argument Count doesnt match! Expected: ", " Actual: ", "");
	const auto corruptData = message("Corrupt Data!");
	const auto deserializationFailure = message("Deserialization failed!");
	const auto doubleComponent = message("Add Double Component: '", "'");
	const auto doubleEntityMethod = message("Double entity method definition ", " in ", "!");
	const auto doubleEventName = message("Double Event definition: '", "'");
	const auto doubleGlobalMethodDefinition = message("Double global method definition: '", "'");	
	const auto doubleTypeDefinition = message("Double type definition: '", "'");
	const auto guiHandlerInitializationFailed = message("Initialization of GuiHandler for Window '", "' failed!");
	//const auto incompatiblePackets = message("Incompatible Datapackets!");
	const auto invalidExpression = message("Invalid Expression at Line ", "!");
	const auto invalidReturn = message("Invalid return-Statement!");
	const auto invalidValueType = message("Invalid ValueType!");
	const auto invalidTypesForOperator = message("Invalid Types for Operator '", "': ", ", ", "");
	const auto loadLayoutFailure = message("Failed to load Layout: '", "'!");
	//const auto missingEvent = message("Reference to non-existing Event: '", "'");
	//const auto moreThanOneAttachedObject = message("Entity with more than one attached Object!");
	//const auto noAttachedObject = message("Entity without attached Object!");
	const auto nodeNotCleared = message("Node is not cleared on Entity-Destructor!");
	const auto noGlobalMethod = message("No global Method :'", "'");
	const auto noMethods = message("No Methods in ", "!");
	//const auto notEnoughArguments = message("Not enough Arguments for '", "'!");
	const auto notValueType = message("No ", " Value!");
	const auto openFileFailure = message("Couldnt open File ", "!");
	const auto reservedKeyword = message("Use of reserved Keyword '", "'!");
	const auto unexpectedParseType = message("Unexpected Type ", " during Parsing in File ", ":(", ",", ")!");
	const auto unexpectedScopeType = message("Unexpected Scope Type: got ", ", expected ", "!");
	const auto unknownArithmeticalOperator = message("Unknown Arithmetical-Operator: ", "");
	const auto unknownCompareOperator = message("Unknown Compare-Operator: ", "");
	const auto unknownComponent = message("Unknown Component-Name: '", "'");
	const auto unknownDeserializationType = message("Unknown Type to Deserialize!");
	const auto unknownEntityDescription = message("Unkown Entity-Description: '", "'");
	const auto unknownLogicalOperator = message("Unknown Bool-Operator: ", "");
	const auto unknownMethod = message("No method ", " in ", "!");
	const auto unknownOperator = message("Unknown Operator: '", "'");
	const auto unknownOwner = message("Unknown Owner: ", "!");
	const auto unknownPrototype = message("Unknown Prototype: '", "'");
	//const auto unknownScriptType = message("Unknown Script-Type ", "!");
	const auto unknownSerializationType = message("Unknown Type to Serialize!");
	const auto unknownValue = message("Unknown Value: '", "'");
	const auto unknownVariable = message("Unknown Variable: ", "");
	const auto windowNotFound = message("Window '", "' not found!");
	





}
}


