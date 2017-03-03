<?php
//---------------------------------------------------------------------------
//
// class.inc
//
// A base level class for parsing XML data
//
// Copyright 2000 MU Systems Pty Ltd
//
// Author: Scott McClintock  -  20th July 2000
//
//---------------------------------------------------------------------------
//
// This class setup a parser and processes XML input strings.  Passing
// multiple strings allows for a unlimited stream of input.
//
//
// Publicly Accessible Functions:
//
// XMLNewParser($StartFunc,
//		$EndFunc)	- Create a new parser and pass the name of the
//				- functions that will process records when
//					a) we reach a start tag and
//					b) when we reach an end tag
//				- Processing is only passed to these functions
//				- when the tag has been specified in the
//				- XMLSetBreak function.
//				- Returns a parser handle for use in the other
//				- function calls
//
// XMLSetBreak($Parser, $Tag,
//		$BreakPoint)	- Set the record marker tag. Note that you can
//				- have multi dimensional records by setting
//				- multiple Breaks in order of outer most to
//				- inner most. The $Breakpoint variable has
//				- a value of either "START" or "END" depending
//				- on where you wnat the break to occur
// XMLParseData($Parser,
//	$Data, $Status)		- Take a string and parse. A $Status of 0 means
//				- that there is more data to come. 1 means that
//				- this is the last call.
//
//
// This parser set is very general purpose and it may be necessary to bypass
// it all together in some circumstances and use the PHP XML libraries direct.
// In particular, if the best possible performance is required, you may find
// the folowing code a little combersome due to the multi-level function calls
// and Data array management.  It should however be ideal for simple situations.
//
// If you actually want this parser to do something useful, you must create 
// a function that can be called by the internals of the parser that will
// process the actual data being parsed. The function is defined as follows:
//
// function MyProcess($Tag, $Data)
//
// $Tag is the tag for the record which has just been completely parsed.  You
// set the name of record tags by using the XMLSetBreak funciton.
//
// $Data is an associative array of data elements parsed with key names that
// are heirachical. eg. <X><Y><Z>Hello</Z></Y></X> is represnted as X.Y.Z hence:
//
//	$Data("X.Y.Z") returns "Hello".
//
// Still confused?  Ask Scott for some assistance!
//
//---------------------------------------------------------------------------
//
// Modification history:
//
// Author: Craig Sargent	Written: 1/8/2000
// Added new class DataStore
// this class will store a name indexed array (generally from an xml data
// object)
//
// Author: Murray Shields	Written: 29/9/2000
// Added a simple function [DisplayXML($XMLString)] for rendering XML with
// special characters and <PRE> tags - used for testing only.
//---------------------------------------------------------------------------

// The Handlers:
//
// This is all slightly bodgey because the PHP XML stuff doesn't yet support
// using Object Methods as handlers... so...
//
// 1) All access to the class is actually through a normal set of functions
//    that create and manipulate a class object for each unique parser created.
//
// 2) The user of the XML functions must keep track of the parser number (not
//    real hard!!).
//

// Automatically create the Global Objects Array

$XMLObjects = array();

//
// Create an object name - use by a number of other functions
//

function XMLCreateName($Parser)
{
	$O = "XML_".$Parser;

	return($O);
}

//
// Create a parser object
//

function XMLNewObject($Parser, $StartFunction, $EndFunction)
{
	global $XMLObjects;

	// Create an Object Name

	$O = XMLCreateName($Parser);

	// Add it to the global Array

	$XMLObjects[$O] = new XML($StartFunction, $EndFunction);
}

//
// Get the given XML Object - Warning ... this returns a copy of the object
//				so is only useful for readonly purposes!
//

function XMLGetObject($Parser)
{
	global $XMLObjects;

	$O = XMLCreateName($Parser);

	return($XMLObjects[$O]);
}

//
// StartElement Handler
//

function XMLStartElement($Parser, $Tag, $Attributes)
{
	global $XMLObjects;

	$O = XMLCreateName($Parser);

	$XMLObjects[$O]->Start($Tag, $Attributes);
}

//
// EndElement Handler
//

function XMLEndElement($Parser, $Tag)
{
	global $XMLObjects;

	$O = XMLCreateName($Parser);

	$XMLObjects[$O]->End($Tag);
}

//
// EndElement Handler
//

function XMLElement($Parser, $Data)
{
	global $XMLObjects;

	$O = XMLCreateName($Parser);

	$XMLObjects[$O]->Element($Data);
}

//
// Process an XML Data Block
//

function XMLParseData($Parser, $Data, $Status)
{
	if(!xml_parse($Parser, $Data, $Status))
	{
		die(sprintf("XML error: %s at line %d",
					xml_error_string(xml_get_error_code($Parser)),
					xml_get_current_line_number($Parser)));
	}
}

//
// XMLSetBreak Handler
//

function XMLSetBreak($Parser, $Tag, $BreakPoint)
{
	global $XMLObjects;

	$O = XMLCreateName($Parser);

	$XMLObjects[$O]->SetBreak($Tag, $BreakPoint);
}

//
// XMLSetObject, allows start and end functions to be within an object.
//

function XMLSetObject($Parser, &$Object)
{
	global $XMLObjects;

	$O = XMLCreateName($Parser);

	$XMLObjects[$O]->SetObject($Object);
}

function XMLSetDelete($Parser, $Tag)
{
	global $XMLObjects;

	$O = XMLCreateName($Parser);

	$XMLObjects[$O]->SetDelete($Object);
}

//
// Create a new Parser and Parser Object
//

function XMLNewParser($StartFunction, $EndFunction)
{
	// Create a Parser

	$Parser = xml_parser_create();
	// Create an Object name

	XMLNewObject($Parser, $StartFunction, $EndFunction);

	// Link handlers to this parser
	// Note that these handlers will simply be stubbs that link into
	// the relevant object

	xml_set_element_handler($Parser, "XMLStartElement", "XMLEndElement");
	xml_set_character_data_handler($Parser, "XMLElement");

	return($Parser);
}

//
// The actual XML Processing class.
//
// Each parser creates its own instance of this class which it uses to
// deal with the data it is parsing. The trick here is to allow for different
// backend processing requirements.  This is done by setting a processing
// function when the class instance is created.  This function should be
// created by the class user.
// 
class XML
{
	var $D;		// An array holding record components
	var $S;		// TAG Level tracking stack
	var $ST;	// An array holding a list of record marker tags
	// A call will be made to the $StartFunc
	// When ever one of these tags in encountered.
	var $ET;	// An array holding a list of record marker tags
	// A call will be made to the $EndFunc
	// When ever one of these tags in encountered.
	var $DT;	// The previous information stored under will be erased
	// whenever one of these tags is started.
	var $StartFunc;	// The name of a function that should be called
	// when processing start of record markers
	var $EndFunc;	// The name of a function that should be called
	// when processing end of record markers
	var $Debug;	// Set to 1 to turn on debug

	var $FuncObject; // Reference to class start and end funcs are in
	var $Object; // Boolean. true if start and end funcs are in a class.


	// Constructor function - Sets things up

	function __construct($StartFunction, $EndFunction)
	{
		// Set the process Functions

		$this->StartFunc = $StartFunction;
		$this->EndFunc = $EndFunction;

		// Setup TAG Level tracking stacks

		$this->S = array();

		// Establish the Record Array

		$this->D = array();

		// Establish the Tag Marker Arrays

		$this->ST = array();
		$this->ET = array();
		$this->DT = array();

		$this->Debug = 0;
	}

	function SetObject(&$FuncObject)
	{
		$this->FuncObject =& $FuncObject;
		$this->Object = is_object($FuncObject);
	}
	// Add a Record break Tag to the list of such

	function SetBreak($Tag, $BreakPoint)
	{
		switch($BreakPoint)
		{
			case "START":
				$this->ST[] = $Tag;
			break;
			case "END":
				$this->ET[] = $Tag;
			break;
		}
	}

	function SetDelete($Tag)
	{
		$this->DT[] = $Tag;
	}

	// Tag Start Function
	function Start($Tag, $Attributes)
	{
		// Create a Heirarchical Tag Name
		if(count($this->S))
			$FullTag = current($this->S).".".$Tag;
		else
			$FullTag = $Tag;

		//
		// Only delete if we have been told to!
		//
		if(in_array($Tag,$this->DT))
		{
			$Offset = strlen($FullTag);
			foreach ($this->D as $Key => $Value)
			{
				$DotOffset = $Offset + 1;
				//
				// GDTAG - WHY DO WE NEED TO DELETE HERE AT ALL?
				// If we delete after the end tag is called???
				//
				// We need to set the full tag to 'TAG.' and then
				// Take the $Key and check it against this longer DotTag to avoid
				// deleting things with similar names that aren't actually children
				// of the tag. 
				// We also need to delete the tag itself
				//
				$DotKey = substr($Key, 0 , $DotOffset);
				$DotFullTag = $FullTag.".";

				if($DotKey == $DotFullTag || $Key == $FullTag)
				{
					$this->D[$Key] = "";
				}
			}
		}

		// Remember this Tag
		array_push($this->S, $FullTag);
		end($this->S);

		// Now fetch any attributes

		foreach($Attributes as $key => $val)
		{
			$this->D[$FullTag.".".$key] = trim($val);
		}

		// Check to see if this tag is in the list of start record
		// markers and if found and there is a valid $ProcFunc
		// then call it.

		if(in_array($Tag,$this->ST))
		{
			if($this->StartFunc != "")
			{
				$TheFunc = $this->StartFunc;
				if ($this->Object)
					$this->FuncObject->$TheFunc($Tag, $this->D);
				else
					$TheFunc($Tag, $this->D);
			}
		}
	}

	// Tag End Function
	function End($Tag)
	{
		$FullTag = array_pop($this->S);
		end($this->S);

		if($this->Debug)
			echo $FullTag."=[".$this->D[$FullTag]."]\n";

		// Trim white space etc from the start and end of the value
		// This functionality may have to be revised...

		$this->D[$FullTag] = trim($this->D[$FullTag]);

		// Check to see if this tag is in the list of end record
		// markers and if found and there is a valid $ProcFunc
		// then call it.


		if(in_array($Tag,$this->ET))
		{
			if($this->EndFunc != "")
			{
				$TheFunc = $this->EndFunc;
				if ($this->Object)
					$this->FuncObject->$TheFunc($Tag, $this->D);
				else
					$TheFunc($Tag, $this->D);
			}

			// Now clear this level of items
			// This means that anything below the level
			// of $FullTag will be wiped!  The implication
			// is that its the responsibility of the
			// user to "use it before they lose it"!

			$Offset = strlen($FullTag);

			reset($this->D);

			foreach($this->D as $Key => $Val)
			{
				$DotOffset = $Offset + 1;
				//
				// We need to set the full tag to 'TAG.' and then
				// Take the $Key and check it against this longer DotTag to avoid
				// deleting things with similar names that aren't actually children
				// of the tag. 
				// We also need to delete the tag itself
				//
				$DotFullTag = $FullTag.".";

				$DotKey = substr($Key, 0 , $DotOffset);
				if($DotKey == $DotFullTag || $Key == $FullTag)
				{
					$this->D[$Key] = "";
				}
			}
		}
	}

	// Element processing

	function Element($Data)
	{
		// Get the current Tag

		$Tag = current($this->S);

		$this->D[$Tag] .= $Data;
	}
}

class DataStore
{
	var $Data;
	//
	// Constructor function will parse xml array and store values
	// that match StoreStr
	//
	function DataStore($StoreStr ,$XMLData)
	{
		$Len = strlen($StoreStr);
		foreach($XMLData as $Key => $Val)
		{
			$tmp = strstr($Key,$StoreStr);
			if($tmp)
			{
				//
				// strip out StoreStr and '.' after it.
				//
				$tmp = substr($tmp,$Len+1);
				$this->Data[$tmp]=$Val;
			}
		}
	}

	function GetData($Index)
	{
		return $this->Data[$Index];
	}

	function GetStore()
	{
		return $this->Data;
	}

	function DumpStore()
	{
		echo "<TABLE WIDTH=50% BORDER=1><TR>\n";
		echo "<TH>Key</TH><TH>Value</TH></TR>\n";
		foreach($this->Data as $key => $val)
		{
			echo "<TR><TD>$key</TD><TD>$val</TD></TR>\n";
		}
		echo "</TABLE>\n";
	}
}

//
// simply displays an xml string for browser rendering
//

function DisplayXML($XMLString)
{
	echo "<HR><PRE>";
	echo htmlspecialchars($XMLString);
	echo "</PRE><HR>";
}
?>
