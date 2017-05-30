<?php

//
// create.inc
//
// A base level class for writing XML data
//
// Copyright 2000 MU Systems Pty Ltd
//
// Author: Murray Shields - 21/08/2000
//
//---------------------------------------------------------------------------
//
// This class creates an XML input string.
//
//
// Publicly Accessible Functions: 
//
// StartTag($Name, $Value)
//	This MUST be the first function called, or an error will result.
//	A start tag must ALWAYS have a valid name, but does not necessarily
//	need to have a value.
//
// CloseTag($Name)
//	There MUST be a matching close tag for every start tag. It MUST NOT
//	be overlapping with another tag (but can be wholly nested).
//
// AddValue($Value)
//	Adds a value to the current tag. This allows you to add a value to a
//	tag as the last thing you do, rather than needing to do it first.
//
// AddCDATA($Value)
//	Adds an XML tag with CDATA value. As with "normal" tags, these can
//	accept other tags or attributes.
//
// AddAttribute($Name, $Value)
//	Adds an attribute to the current tag. The attribute name MUST be
//	a valid XML name. $Value CAN be null if necessary.
//
// AddXMLString($String)
//	Adds an string into the xml...the string is not validated at this
//	time, so care is needed to ensure valid and well formed xml is
//	still created.
//
// GenerateXML($Header)
//	$Header allows you to pass a string that will be added to the top of
//	the generated XML document. This allows you to, for example, create
//	a fully compliant, standalone document with all of the necessary and
//	correct header information. For example:
//		GenerateXML("< ?xml version=\"1.0\" standalone=\"yes\"? >");
//	After the above header comes the XML document as entered by the user,
//	which is formatted neatly for readability.
//
//
// NOTES:
// 
// HTML entities such as '&', '<', '>' and '"' are automatically encoded
// by the class for all values.
//
// StartTag functions can be nested as deep as necessary, but you must use
// the CloseTag in the correct order (no overlapping tags).
//
// There is no reasonable limit to the number of AddAttribute functions that
// can be called within a particular tag.
//
// There is no reasonable limit to the number of AddValue functions that
// can be called within a particular tag.
//
//
// There are a number of problems and rules that this class looks for and will
// halt execution if discovered. These include:
// 
// 1. Tag and Attribute names must meet the XML specification. This 
//    states that all characters must be alphanumeric, "_", "-" or ".".
//    Additionally, the first character must be alpha or "_".
// 
// 2. All opened tags must have matching closing tags.
//
// 3. Tags must not overlap.
//
// 4. Attributes and Values cannot be added OUTSIDE a tag.
//
//
// Note that the following is NOT checked:
//
// 1. That all tags are contained within one root-level tag.
//
// 2. The validity of a passed header string is not checked.
//
//
// MODIFICATION HISTORY
// ====================
// Craig Sargent	1/2/2001
// Added in AddXMLString($String) function to
// add in a preformed xml string into the xml created

class CreateXML
{
	var $X;	// An array holding the data that will form the XML
	// 	Contains: [seq_nr][key|tag|att|val]
	var $P;	// A pointer to the current array location
	var $S;	// TAG Level tracking stack (uses stack class)
	var $D;	// Depth when writing XML (used for formatting only)

	//
	// Initialises the necessary variables when the class is first run
	//
	function __construct()
	{
		// resets the array containing the passed value pairs
		$this->X = array();
		// reset the pointer for the array
		$this->P = 0;
		// create an empty stack for tracking the depth
		$this->S = array();

		$this->Tab = 2;
		$this->Space = " ";
		$this->Break = "\n";
	}

	//
	// It checks to see if a tag/attribute name meets correct XML rules.
	//
	function CheckTag($String)
	{
		// Do I know how this function works? Nope.
		// This regex was 'written' using the PHP list in 3 minutes.
		$Success = preg_match('/^[_[:alpha:]][-_.[:alnum:]]*$/', $String);
		// If it fails, halt execution and display an appropriate message
		if(!$Success)
		{
			echo "\n\nXML CREATE ERROR: Invalid Open Tag (".$String.")\n\n";
			echo "It must contain only alphanumeric, \".\", \"_\" or \"-\".";
			echo "\nThe first character must be alpha or \"_\".\n\n";
			exit();
		}

	}

	//
	// Adds a start tag to the XML document
	//
	function StartTag($Name, $Value = "")
	{
		// Checks the validity of the tag name
		$this->CheckTag($Name);

		// Adds a key to the array indicating this is a start tag
		$this->X[$this->P]["key"] = "start";

		// Adds the name of the tag to the array
		$this->X[$this->P]["tag"] = $Name;

		// Adds the value to the array (with encoded characters)
		$this->X[$this->P]["val"] = htmlspecialchars($Value);

		// Pushes the tag name onto a stack
		array_push($this->S, $Name);
		end($this->S);

		// Increments the array pointer by one
		$this->P++;

	}

	//
	// Adds an attribute and value to the current tag
	//
	function AddAttribute($Name, $Value)
	{
		// Checks the stack to see if we are currently outside all tags
		if(current($this->S) == "")
		{
			// If that is the case, display error and halt.
			echo "\n\nXML CREATE ERROR: Attribute added outside tag\n\n";
			exit();
		}
		// Adds a key to the array indicating this is an attribute tag
		$this->X[$this->P]["key"] = "attrb";

		// Adds that tag name to the array, using the 
		// current stack entry
		$this->X[$this->P]["tag"] = current($this->S);

		// Adds the name of the attribute to the array
		$this->X[$this->P]["att"] = $Name;

		// Adds the value to the array (with encoded characters)
		$this->X[$this->P]["val"] = htmlspecialchars($Value);

		// Increments the array pointer by one
		$this->P++;
	}

	//
	// Adds an xmlstring  to the current possition
	//
	//GDTAG - Will need to validate XMLStr eventually
	function AddXMLString($XMLStr)
	{
		//
		// If we are outside of all takes, display error and halt
		//
		
		if(current($this->S) == "")
		{
			echo "\n\nXML CREATE ERROR: Value added outside tag\n\n";
			exit();
		}
		//
		// Key type is xmlstr
		//
		$this->X[$this->P]["key"] = "xmlstr";

		// Adds that tag name to the array, using the current
		// stack entry
		$this->X[$this->P]["tag"] = current($this->S);

		// Adds the string to the array (with encoded characters)
		$this->X[$this->P]["val"] = $XMLStr;

		// Increments the array pointer by one
		$this->P++;
	}
	//
	// Adds a value to the current tag
	//
	function AddValue($Value)
	{
		// Checks the stack to see if we are currently outside all tags
		if(current($this->S) == "")
		{
			// If that is the case, display error and halt.
			echo "\n\nXML CREATE ERROR: Value added outside tag\n\n";
			exit();
		}
		// Adds a key to the array indicating this is a tag value tag
		$this->X[$this->P]["key"] = "value";

		// Adds that tag name to the array, using the current
		// stack entry
		$this->X[$this->P]["tag"] = current($this->S);

		// Adds the value to the array (with encoded characters)
		$this->X[$this->P]["val"] = htmlspecialchars($Value);

		// Increments the array pointer by one
		$this->P++;
	}

	//
	// Adds a CDATA value to the current tag
	//
	function AddCDATA($Value)
	{
		// Checks the stack to see if we are currently outside all tags
		if(current($this->S) == "")
		{
			// If that is the case, display error and halt.
			echo "\n\nXML CREATE ERROR: Value added outside tag\n\n";
			exit();
		}
		// Adds a key to the array indicating this is a tag value tag
		$this->X[$this->P]["key"] = "cdata";

		// Adds that tag name to the array, using the current
		// stack entry
		$this->X[$this->P]["tag"] = current($this->S);

		// Adds the value to the array
		$this->X[$this->P]["val"] = $Value;

		// Increments the array pointer by one
		$this->P++;
	}

	//
	// Closes an XML tag
	//
	function CloseTag($Name)
	{
		// Checks to see if the close tage matches the current start tag
		// as recorded in the stack
		if(current($this->S) != $Name)
		{
			// If not, display an error and halt.
			echo "\n\nXML CREATE ERROR: Unmatched Close Tag (".$Name.")\n\n";
			exit();
		}
		// Adds a key to the array indicating this is a close tag
		$this->X[$this->P]["key"] = "close";

		// Add the full closing tag name to the array
		$this->X[$this->P]["tag"] = "/".$Name;

		// Pop this tag name from the stack
		array_pop($this->S);
		end($this->S);

		// Increments the array pointer by one
		$this->P++;
	}

	function CompleteTag($Name, $Value = "", $Attributes = "")
	{
		$this->StartTag($Name, $Value);
		if (is_array($Attributes))
			foreach ($Attributes as $Key => $Val)
				$this->AddAttribute($Key, $Val);
		$this->CloseTag($Name);
	}

	function ConditionalTag($Name, $Value = "", $Attributes = "")
	{
		if (!is_array($Attributes))
			$Attributes = array();
		if (trim($Value) || sizeof($Attributes))
			$this->CompleteTag($Name, $Value, $Attributes);
	}

	//
	// Returns a number of spaces as a string (used for formatting)
	//
	function Spaces($N)
	{
		// If we are in a tag that is 5 levels dep, then it return
		// 4 spaces as a string for indenting the output.
		// Customised indentation can be done by passing a value $N
		// to the function (e.g., tag values need an additional space).
		$S = ($this->D + $N - 1) * $this->Tab;
		$Space = "";
		for($i=1;$i<=$S;$i++)
			$Space .= $this->Space;
		return($Space);
	}

	//
	// A recursive function that generates the actual XML
	//
	function ProcessValuePairs(&$LastElement = "")
	{
		// This function loops through all of the elements that are 
		// contained within this one tag. When a 'new' tag is found,
		// it recursively calls itself and starts again. When a tag is
		// Conplete, the full tag is returned, which becomes a part of
		// the contents for the tag above. Therefore, the following
		// strings are collected during processing:
		// 
		// $Tag
		// 	Contains the tag name.
		// $Content
		//	Contains the contents of the tag (which includes 
		// 	passed values and other, complete tags).
		// $Attributes
		// 	Contains the attributes for the current tag
		//
		// The Depth counter ($this->D) is used to track the current
		// depth of the tag. This is then used to generate indented
		// formatting using the Spaces function ($this->Spaces($this->D)
		// that is immediately above.

		// Increment the depth counter
		$this->D++;

		// Create the beginning of the tag
		$Tag = "";
		if ($LastElement == "open")
			$Tag .= $this->Break;
		$Tag .= $this->Spaces(0)."<".$this->X[$this->P]["tag"];
		$LastElement = "open";

		// If the tag has a value then add it to the content string
		if($this->X[$this->P]["val"] != "")
		{
			if (in_array($LastElement, array("open")))
				$Content = $this->X[$this->P]["val"];
			else
				$Content = $this->Spaces(1).$this->X[$this->P]["val"].$this->Break;
			$LastElement = "content";
		}
		else
			$Content = "";
		// No attributes can be passed with a start tag
		$Attributes = "";

		// Increment the array pointer
		$this->P++; 

		//
		// Loop until the array pointer is bigger than the array
		// and We have not reached the close of this tag
		//
		$XSize = sizeof($this->X);
		while(($this->P < $XSize) && ($this->X[$this->P]["key"] != "close"))
		{
			// What we do depends on the entry type
			switch ($this->X[$this->P]["key"])
			{
				// If it is a new start tag call this function
				case "start":
					// Entire tag becomes this tag's content
					$Content .= $this->ProcessValuePairs($LastElement);
					break;

				// If it is an attribute then add the name and
				// value to the attribute string
				case "attrb":
					$Attributes .= " ";
					$Attributes .= $this->X[$this->P]["att"];
					$Attributes .= "=\"";
					$Attributes .= $this->X[$this->P]["val"];
					$Attributes .= "\"";
					break;

				// If a value  or xml str then add it to tag contents
				case "value":
				case "xmlstr":
					$Content .= $this->Spaces(1);
					$Content .= $this->X[$this->P]["val"].$this->Break;
					break;

				// If a CDATA value then add it to tag contents
				case "cdata":
					$Content .= "<![CDATA[".$this->X[$this->P]["val"]."]]>";
					$LastElement = "cdata";
					break;
			}
			// Increment th array pointer by one
			$this->P++; 
		}
		// When we are done looping:
		// Add the attributes to the tag to create the full tag name
		$Tag .= current($this->S).$Attributes.">";
//		$LastElement = "open";

		// Add the contents to the tag
		$Tag .= $Content;

		// Add the close to the tag and content

		if (in_array($LastElement, array("open", "content", "cdata")))
			$Tag .= "<".$this->X[$this->P]["tag"].">".$this->Break;
		else
			$Tag .= $this->Spaces(0)."<".$this->X[$this->P]["tag"].">".$this->Break;
		$LastElement = "close";

		// Decrement the Depth counter
		$this->D--;

		// Return the completed tag
		return($Tag);
	}

	//
	// Returns the full formatted XML string or document
	//
	function GenerateXML($Header = "")
	{
		// If we have anything in the stack, then we have unclosed tags
		if(count($this->S))
		{
			// Display appropriate error message and halt
			echo "\n\nXML CREATE ERROR: Not All Tags Closed\n\n";
			exit();
		}
		// Set the array pointer to zero
		$this->P = 0;

		// Set the depth counter to zero
		$this->D = 0;

		// Note that this is used SOLELY for formatting output neatly

		// Call a recursive function for generating the XML
		$XMLString = $this->ProcessValuePairs();

		// If a header was passed, add it to the beginning of the XML
		if ($Header)
			$XMLString = $Header."\n".$XMLString;

		// Return the complete XML string
		return($XMLString);
	}

	//
	// Returns the full formatted XML string or document
	//
	function GenerateCompressedXML($Header = "")
	{
		$this->Space = "";
		$this->Break = "";

		// If we have anything in the stack, then we have unclosed tags
		if(count($this->S))
		{
			// Display appropriate error message and halt
			echo "\n\nXML CREATE ERROR: Not All Tags Closed\n\n";
			exit();
		}
		// Set the array pointer to zero
		$this->P = 0;

		// Set the depth counter to zero
		$this->D = 0;

		// Note that this is used SOLELY for formatting output neatly

		// Call a recursive function for generating the XML
		$XMLString = $this->ProcessValuePairs();

		// If a header was passed, add it to the beginning of the XML
		if ($Header)
			$XMLString = $Header.$this->Break.$XMLString;

		// Return the complete XML string
		return($XMLString);
	}
}

?>
