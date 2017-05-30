<?php require_once(PHPLIB_DIR."/xml/class.inc");

// parse.inc
//
// Copyright MU Systems Pty Ltd 2000
//
// Author: Murray Shields       Written: 13/12/2000
//
// Modification history
//
//
// This class and related external functions effectively takes and XML string,
// parses it, and stores all data in a multi-dimensional array, ready for use.
//
// Before this class can be used, you must first populate the "xmlparser" table
// in the "WEB" databases with an appropriate list of end tags. An admin
// utility for doing this is available at http://www.abr.com.au/dev/admin/
//
// When complete, you need to initialise the class and call one of 2 functions
// to perform the parsing. XML results are NOT stored in the class, so once it
// is initialised it can be used again and again as necessary. This class is
// included automatically as a part of the "xmlincludes" in phplib.
// 
// Note that initialising the class is not CPU or RAM intensive. Therefore,
// where appropriate, you may simply initialise the class from with "local.inc"
// or at the top of your document, rather than immediately before use.
// 
// 
// To initialise the class:
// 
// 	$ParseClass = new ParseXML($DATABASE_NAME, $TAG_SET);
// 
//	where:
// 		$DATABASE_NAME = "WEB" or "WEBTEST"
// 	and
// 		$TAG_SET = the name you are using in the end tags database
// 
// 	IMPORTANT: YOU MUST NAME THE CLASS $ParseClass AND IT MUST BE A
// 		GLOBALLY ACCESSIBLE VARIABLE DUE TO THE USE OF FUNCTIONS THAT
// 		ARE EXTERNAL TO THE CLASS.
// 
// 	Additionally, there is an optional parameter as follows:
// 
//	 	$ParseClass = new ParseXML($DATABASE_NAME, $TAG_SET, 1);
// 
// 	The addtional flag at the end places the entire class into debug mode.
// 	This means that everything that it does is also echoed directly to the
// 	browser, allowing you to check to see if it is working properly and,
// 	more importantly, see where the class is storing all of your data.
// 
// To parse and XML string into an array:
// 
// 	$MyArray = $ParseClass->GetXMLResults($XMLString);
// 
// To parse the XML and kick off processing automatically:
// 
// 	$ParseClass->ProcessXMLResults($XMLString, $FUNCTION_NAME, $MY_FLAG);
// 
// 	where:
// 		$FUNCTION_NAME = the name of the function that you want called
// 	and
// 		$MY_FLAG = a variable passed directly to that function 
// 
// Therefore:
// 
// 	$SearchType = "NameSearch";
// 	$MyArray = $ParseClass->GetXMLResults($XML);
// 	DisplayResults($MyArray, $SearchType);
// 
// is exactly the same as:
// 
// 	$ParseClass->ProcessXMLResults($XML, "DisplayResults", "NameSearch");
// 
///////////////////////////////////////////////////////////////////////////////


//
// I decided to rationalise some of the code when it started to look like the following ... :-)
//
// $XMLResultsArray[$PC->TAGS[$i]["name"]][$c][$PC->TAGS[$PC->MASTER[$PC->TAGS[$i]["name"]][$m]]["name"]."Table"] 
// = $XMLResultsArray[$PC->TAGS[$PC->MASTER[$PC->TAGS[$i]["name"]][$m]]["name"]."Table"];
//

// Initialise a global array needed by this set of functions
$XMLResultsArray = array();

//
// from the string "A.B.C.D" it returns "D"
//
function ParserGetLastName($NAME)
{
	$PARTS = explode(".", strtoupper($NAME));
	return($PARTS[(sizeof($PARTS) - 1)]);
}

//
// returns the tag name from the full tag path (can be same as ParserGetLastName)
//
function ParserGetDataName($PATH, $NAME)
{
	$NAME = str_replace(strtoupper($PATH), "", strtoupper($NAME));
	$NAME = str_replace(".", "", $NAME);
	return($NAME);
}

//
// processes tag name and data pairs for storage
//
function ParserStoreData($PATH, $DATA)
{
	// globalise the ParseClass and give it a shorter, friendlier name
	global $ParseClass;
	$PC = $ParseClass;

	// initialise the array used for temporary storage of the data
	$Store = array();

	// loop through the list of data pairs
	foreach ($DATA as $KEY => $VAL)
	{
		// if the current tag is NOT a part of the current tag path...
		// eliminates tags initialised earlier but not part of this data
		if ($KEY && $PATH)
			if(stristr($KEY, $PATH))
			{
				// tries to get the name of this tage
				$ShortName = ParserGetDataName($PATH, $KEY);
				if($ShortName)
					$Name = $ShortName;
				else
				// if it failed, get the last part of the tag
					$Name = ParserGetLastName($KEY);
				// store the data in the temporary array
				$Store[$Name] = $VAL;
				// if in debug mode, display the data storage action
				if ($PC->DEBUG && ($VAL != ""))
					echo "<BR>-> DATA: [".$Name."] = ".$VAL;
			}
	}
	// return the processed array of data
	return($Store);
}

//
// processing on start tags is not used at all anywhere in this code
// however a dummy function is needed in order to avoid errors
//
function ParseStartProcess() {}

//
// performs end tag processing
//
// there are two basic types of end tags:
// - repeating tags (multiple table rows etc)
// - non-repeating rows (no repeats - a single tag which *may* CONTAIN repeats)
//
function ParseEndProcess($Tag, $D)
{
	// globalise the results array
	global $XMLResultsArray;
	// globalise the ParseClass and give it a shorter, friendlier name
	global $ParseClass;
	$PC = $ParseClass;

	// find the location of this tag in the class database
	$Index = array();
	$Count = 0;
	for ($i=0;$i<=$PC->SIZE;$i++)
		if ($Tag == $PC->TAGS[$i]["tag"])
		{
			$Count++;
			$Index[$Count] = $i;
		}
	if ($Count == 1)
		$i = $Index[$Count];
	else
	{
		for ($j=1;$j<=$Count;$j++)
		{
			$TriggerPath = $PC->TAGS[$Index[$j]]["path"];
			$TriggerSize = strlen($TriggerPath);

			reset ($D);
			while(list($KEY, $VAL) = each($D))
			{
				if (($TriggerPath == substr($KEY, 0, $TriggerSize)) && ($VAL != ""))
					$i = $Index[$j];
			}
		}
	}

	// get a friendlier name for reusable info
	$ThisName = $PC->TAGS[$i]["name"];
	$ThisRepeat = $PC->TAGS[$i]["repeat"];
	// if this is a repeating tag, we need to store it in an array of arrays
	if ($ThisRepeat == 1)
	{
		// initialise the counter
		$XMLResultsArray[$ThisName."Count"]++;
		// make the counter a MUCH shorter variable name
		$c = $XMLResultsArray[$PC->TAGS[$i]["name"]."Count"];
		// if in debug mode echo this action
		if ($PC->DEBUG)
			echo "<P>Storing in \$XMLResultsArray[".$ThisName."Table][".$c."]";
		// store the results in the multidimensional array
		$XMLResultsArray[$ThisName."Table"][$c] =
			ParserStoreData($PC->TAGS[$i]["path"], $D);
	}
	// if this is NOT a repeating tag
	else
	{
		// if in debug mode echo this action
		if ($PC->DEBUG)
			echo "<P>Storing in \$XMLResultsArray[".$PC->TAGS[$i]["name"]."]";
		// store the results in the multidimensional array
		$XMLResultsArray[$ThisName] =
			ParserStoreData($PC->TAGS[$i]["path"], $D);
	}

	$RangeEnd = 0;
	// this is where associated tag data is copied to the master tag
	for ($m=0;$m<sizeof($PC->MASTER[$ThisName]);$m++)
	{
		$TargetName = $PC->TAGS[$PC->MASTER[$ThisName][$m]]["name"];
		$TargetRepeat = $PC->TAGS[$PC->MASTER[$ThisName][$m]]["repeat"];

		if ($ThisRepeat)
		{
			if ($TargetRepeat)
			{
				$XMLResultsArray[$ThisName."Table"][$c][$TargetName."Table"] = $XMLResultsArray[$TargetName."Table"];
				$XMLResultsArray[$ThisName."Table"][$c][$TargetName."Count"] = $XMLResultsArray[$TargetName."Count"];
			}
			else
				$XMLResultsArray[$ThisName."Table"][$c][$TargetName] = $XMLResultsArray[$TargetName];
		}
		else
		{

			if ($TargetRepeat)
			{
				$XMLResultsArray[$ThisName][$TargetName."Table"] = $XMLResultsArray[$TargetName."Table"];
				$XMLResultsArray[$ThisName][$TargetName."Count"] = $XMLResultsArray[$TargetName."Count"];
			}
			else
				$XMLResultsArray[$ThisName][$TargetName] = $XMLResultsArray[$TargetName];
		}

		if ($TargetRepeat)
		{
			$XMLResultsArray[$TargetName."Table"] = array();
			$XMLResultsArray[$TargetName."Count"] = 0;
		}
		else
			$XMLResultsArray[$TargetName] = array();

		$RangeEnd = 1;
	}
	// display a divider if we are in debug mode
	if ($PC->DEBUG && $RangeEnd)
		echo "<BR>-------------- END GROUPING -------------<P>\n";


	// now we find out if we need to do external processing

	if (is_array($PC->MASTER[$PC->TAGS[$i]["path"]]))
	{
		$FunctionName = $PC->MASTER[$PC->TAGS[$i]["path"]]["function"];

		if ($ThisRepeat)
			$FunctionName($XMLResultsArray[$ThisName."Table"][$c]);
		else
			$FunctionName($XMLResultsArray[$ThisName]);



		if ($PC->MASTER[$PC->TAGS[$i]["path"]]["reset"])
		{
			if ($ThisRepeat)
			{
				$XMLResultsArray[$ThisName."Table"] = array();
				$XMLResultsArray[$ThisName."Count"] = 0;
			}
			else
				$XMLResultsArray[$ThisName] = array();
		}
	}
}

//
// the start of the parse xml class
//
class ParseXML
{
	//
	// initialises the class for repeated use and loads the tag definitions
	// from the specified database
	//
	function __construct($DB_NAME, $TAG_SET, $DEBUG = 0)
	{
		// store the database name in the class
		$this->DB = $DB_NAME;
		// store the tag set in the class
		$this->SET = $TAG_SET;
		// store the debug mode flag in the class
		$this->DEBUG = $DEBUG;
		// initialise the array that will store the tag information
		$this->TAGS = array();
		// initialise the array to hold the master tags
		$this->MASTER = array();
		// initialise the array to hold the external tag processing
		$this->EXTERNAL = array();
		// initialise the index for the arrays
		$i = 0;

		// select the tag information from the database
		$Query = "select *, length(tag_path) as strlen from xmlparser where parser_category = '".$this->SET."' order by strlen";
		$Q = new DBQuery;
		$Q->Run($this->DB, $Query);

		// display a divider if we are in debug mode
		if ($this->DEBUG)
			echo "<BR>============== PRELOADING =============<P>\n";

		// loop through the complete list of tags
		while ($Q->Next())
		{
			// increment the array index
			$i++;
			// store the database fields in the arrays
			$this->TAGS[$i]["tag"] = $Q->GetField("tag_name");
			$this->TAGS[$i]["path"] = $Q->GetField("tag_path");
			$this->TAGS[$i]["name"] = $Q->GetField("data_name");
			$this->TAGS[$i]["repeat"] = $Q->GetField("repeating");

			$this->MASTER[$Q->GetField("master")][] = $i;

			// display the tag information if we are in debug mode
			if ($this->DEBUG)
				echo "[".$i."][".$this->TAGS[$i]["tag"]."][".$this->TAGS[$i]["path"]."][".$this->TAGS[$i]["name"]."][".$this->TAGS[$i]["repeat"]."]<BR>";
		}

		// end the database query
		$Q->EndQuery();

		// store the number of tags in a separate variable
		$this->SIZE = $i;

		// display a divider if we are in debug mode
		if ($this->DEBUG)
			echo "<BR>============== COMPLETED ==============<P>\n";
	}

	//
	// this allows us to set break points for external data processing
	//
	function ExternalFunction($TagPath, $FunctionName, $ResetData = 0)
	{
		$this->MASTER[$TagPath]["function"] = $FunctionName;
		$this->MASTER[$TagPath]["reset"] = $ResetData;
	}

	//
	// initialise the XML parser and the storage arrays
	//
	function StartXMLParser()
	{
		// globalise the xml results and other variables
		global $XMLResultsArray;

		// initialise the parser
		$this->MP = XMLNewParser("ParseStartProcess", "ParseEndProcess");

		// a list of the initialised end tags
		$Used = array();
		// loop through the tags so we can initialise all needed vars
		for ($i=1;$i<=$this->SIZE;$i++)
		{
			// if this is a repeating tag
			if ($this->TAGS[$i]["repeat"] == 1)
			{
				// initialise the array for data storage
				$XMLResultsArray[$this->TAGS[$i]["name"]."Table"] = array();
				// initialise the counter for the array
				$XMLResultsArray[$this->TAGS[$i]["name"]."Count"] = 0;
			}
			else
			{
				// initialise the array for data storage
				$XMLResultsArray[$this->TAGS[$i]["name"]] = array();
			}
			// check to see if we've already initialsed this end tag
			$New = 1;
			for ($j=0;$j<sizeof($Used);$j++)
				if ($this->TAGS[$i]["tag"] == $Used[$j])
					$New = 0;
			// initialise end tag processing if we haven't
			if ($New)
			{
				XMLSetBreak($this->MP, $this->TAGS[$i]["tag"], "END");
				$Used[] = $this->TAGS[$i]["tag"];
			}
		}
	}

	//
	// add some data to the XML parser
	//
	function AddXMLData($XMLData)
	{
		// globalise the xml results and other variables
		global $XMLResultsArray;

		// process the xml
		XMLParseData($this->MP, $XMLData, 0);
	}

	//
	// process the xml and store in arrays
	//
	function StopXMLParser()
	{
		// globalise the xml results and other variables
		global $XMLResultsArray;

		// process the xml
		XMLParseData($this->MP, "", 0);

		// return the results as an array
		return ($XMLResultsArray);
	}
	//
	// process the xml and store in arrays
	//
	function GetXMLResults($XMLResults)
	{
		// globalise the xml results and other variables
		global $XMLResultsArray;

		// initialise the XML parser and the storage arrays
		$this->StartXMLParser();

		// process the xml
		XMLParseData($this->MP, $XMLResults, 1);

		// return the results as an array
		return ($XMLResultsArray);
	}


	//
	// process the xml and store in arrays
	//
	function ProcessXMLResults($XMLResults, $Function, $Flag)
	{
		// get an array containing the results
		$ResultsArray = $this->GetXMLResults($XMLResults);

		// call the passed function name with the results and the flag 
		$Function($ResultsArray, $Flag);
	
	}
}

//////////////////////////////////////////////////////////
// ** Following line must be the last line in the file **
?>
