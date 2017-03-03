<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

//---------------------------------------------------------------------------
//
// header.inc
//
// Displays all Headers and Footers for the Prospects system
//
// Portions Copyright 2001 MU Systems Pty Ltd
// Portions Copyright 2001 Australian Business Research Pty Ltd
//
// Author: Murray Shields  -  29th June 2001
//
//---------------------------------------------------------------------------

//
// automatic time input formatting and validation
//
require_once(PHPLIB_DIR."tools.inc");
require_once(PHPLIB_DIR."field.inc");

class DateField
{
	//
	// add date fields and pre-validate
	//
	function AddDateField($Year, $Month, $Day, $Hour = "", $Minute = "", $Meridian = "")
	{
		// store the date fields
		$this->Year = $Year;
		$this->Month = $Month;
		$this->Day = $Day;
		$this->Hour = $Hour;
		$this->Minute = $Minute;
		$this->Meridian = $Meridian;

		// record the validity of the date
		if (trim($this->Month.$this->Day.$this->Year))
		{
			$this->ValidDate = checkdate($this->Month, $this->Day, $this->Year);

			//16-02-2006::MZ
			//Apparently checkdate allows a month/day/year value through that looks liek '3a' and passes this
			//as a valid date... hmmmm
			if(!is_numeric($this->Month) || !is_numeric($this->Day) || !is_numeric($this->Year))
				$this->ValidDate = 0;
			
			$this->HasDate = 1;
		}
		else
		{
			$this->ValidDate = 1;
			$this->HasDate = 0;
		}

		// time validity defaults to 1
		$this->ValidTime = 1;

		// record that no time was passed (time is therefore "valid")
		if (strlen($this->Hour.$this->Minute) == 0)
			$this->HasTime = 0;
		else
		{
			// a time has been passed
			$this->HasTime = 1;
			// if the minutes OR hours is empty, it can't be a correct time
			if ($this->Hour === "" || $this->Minute === "")
				$this->ValidTime == 0;
			else
			{
				// verify that the minutes are within range
				$Mins = in_array($this->Minute, $this->GetDateSelect("minute"));
				// a 12 hour time needs adjustment
				if (in_array(strtolower($this->Meridian), $this->GetDateSelect("meridian")))
				{
					// verify that the hours are within range
					$Hours = in_array($this->Hour, $this->GetDateSelect("hour_12"));
					// reset the hours to 24 hour format
					if ((strtolower($this->Meridian) == "pm") && ($this->Hour != 12))
						$this->Hour = $this->Hour + 12;
					elseif ((strtolower($this->Meridian) == "am") && ($this->Hour == 12))
						$this->Hour = 0;
				}
				else
					// verify that the hours are within range
					$Hours = in_array($this->Hour, $this->GetDateSelect("hour_24"));
			}
			// time is valid if both the minutes and hours are acceptible
			if (($Mins * $Hours) == 0)
				$this->ValidTime = 0;
		}
	}

	//
	// returns a "validity" flag
	//
	function ValidDate()
	{
		return ($this->ValidDate * $this->ValidTime);
	}

	//
	// returns a formatted date string
	//
	function GetDate($DateFormat = "")
	{
		// creates a time stamp (a different stamp based on whether the input has a time attached)
		if ($this->HasTime == $this->HasDate)
			$Stamp = adodb_mktime($this->Hour, $this->Minute, 0, $this->Month, $this->Day, $this->Year);
		elseif ($this->HasDate)
			$Stamp = adodb_mktime(0, 0, 0, $this->Month, $this->Day, $this->Year);
		elseif ($this->HasTime)
			$Stamp = adodb_mktime($this->Hour, $this->Minute, 0, adodb_date("m"), adodb_date("d"), adodb_date("Y"));

		// if a date format is NOT supplied we return the standard MySQL date and datetime formats.
		if ($DateFormat == "")
		{
			if ($this->HasTime == $this->HasDate)
				$DateFormat = "Y-m-d H:i:s";
			elseif ($this->HasDate)
				$DateFormat = "Y-m-d";
			elseif ($this->HasTime)
				$DateFormat = "H:i:s";
		}

		return (adodb_date($DateFormat, $Stamp));
	}

	//
	// create the Control array
	//
	function CreateDateControlArray($Year = "", $Month = "", $Day = "", $Hour = "", $Minute = "", $Meridian = "")
	{
		return (array($Year, $Month, $Day, $Hour, $Minute, $Meridian));
	}

	//
	// automatic time output formatting
	//
	function CreateDateField($DateTime, $MonthType = "", $HourType = "", $Control = "", $FormType = "")
	{
		// verify the contents of certain fields
		$MonthType = strtolower($MonthType);
		if (!in_array($MonthType, array("month", "month_short", "month_long")))
			$MonthType = "month";
		$HourType = strtolower($HourType);
		if (!in_array($HourType, array("hour_12", "hour_24")))
			$HourType = "hour_12";
		$FormType = strtolower($FormType);
		if (!in_array($FormType, array("", "hidden", "array")))
			$FormType = "";

		$String = "";
		$Parts = array();

		$TimeStamp = CreateTimeStamp($DateTime);
		$Element = array();
		$Element["year"] = adodb_date("Y", $TimeStamp);
		$Element["month"] = adodb_date("m", $TimeStamp);
		$Element["month_short"] = adodb_date("M", $TimeStamp);
		$Element["month_long"] = adodb_date("F", $TimeStamp);
		$Element["day"] = adodb_date("d", $TimeStamp);
		$Element["hour_12"] = adodb_date("h", $TimeStamp);
		$Element["hour_24"] = adodb_date("H", $TimeStamp);
		$Element["minute"] = adodb_date("i", $TimeStamp);
		$Element["meridian"] = adodb_date("a", $TimeStamp);

		if (is_array($Control) && ($FormType == "hidden"))
		{
			if ($Control[0])
				$String .= Field::GetHidden($Control[0], $Element["year"])."\n";
			if ($Control[1])
				$String .= Field::GetHidden($Control[1], $Element["month"])."\n";
			if ($Control[2])
				$String .= Field::GetHidden($Control[2], $Element["day"])."\n";
			if ($Control[3])
				$String .= Field::GetHidden($Control[3], $Element[$HourType])."\n";
			if ($Control[4])
				$String .= Field::GetHidden($Control[4], $Element["minute"])."\n";
			if ($Control[5] && ($HourType == "hour_12"))
					$String .= Field::GetHidden($Control[5], $Element["meridian"])."\n";
			return (trim($String));
		}

		if (is_array($Control))
		{
			if ($Control[2])
			{
				$Str = Field::GetSmartDrop($Control[2], $Element["day"], $this->GetDateSelect("day"));
				$String .= $Str."\n";
				$Parts[] = $Str;
			}
			if ($Control[1])
			{
				$Str = Field::GetSmartDrop($Control[1], $Element["month"], $this->GetDateSelect($MonthType));
				$String .= $Str."\n";
				$Parts[] = $Str;
			}
			if ($Control[0])
			{
				$Str = Field::GetSmartDrop($Control[0], $Element["year"], $this->GetDateSelect("year"));
				$String .= $Str."\n";
				$Parts[] = $Str;
			}
			$String .= " ";
			if ($Control[3])
			{
				$Str = Field::GetSmartDrop($Control[3], $Element[$HourType], $this->GetDateSelect($HourType));
				$String .= $Str."\n";
				$Parts[] = $Str;
			}
			if ($Control[4])
			{
				$Str = Field::GetSmartDrop($Control[4], $Element["minute"], $this->GetDateSelect("minute"));
				$String .= $Str."\n";
				$Parts[] = $Str;
			}
			if ($Control[5] && ($HourType == "hour_12"))
			{
				$Str = Field::GetSmartDrop($Control[5], $Element["meridian"], $this->GetDateSelect("meridian"));
				$String .= $Str."\n";
				$Parts[] = $Str;
			}
			if ($FormType == "array")
				return ($Parts);
			else
				return (trim($String));
		}

		if (trim($Control))
		{
			$Search = array("Y","M","D","h","m","a");
			$Replace = array();
			$Replace[] = $Element["year"];
			$Replace[] = $Element[$MonthType];
			$Replace[] = $Element["day"];
			$Replace[] = $Element[$HourType];
			$Replace[] = $Element["minute"];
			if ($HourType == "hour_12")
				$Replace[] = $Element["meridian"];
			else
				$Replace[] = "";

			$String .= str_replace($Search, $Replace, $Control);
		}
		else
		{
			$String .= 	$Element["day"];
			if ($MonthType == "month")
				$String .= "/";
			else
				$String .= " ";
			$String .= 	$Element[$MonthType];
			if ($MonthType == "month")
				$String .= "/";
			else
				$String .= " ";
			$String .= $Element["year"];
			$String .= " ";
			if ($Element["hour_12"])
			{
				$String .= 	$Element[$HourType];
				$String .= ":";
				$String .= 	$Element["minute"];
				if ($HourType == "hour_12")
					$String .= 	$Element["meridian"];
			}
		}

		return ($String);
	}

	function GetDateSelect($Type, $StartY = 0, $EndY = 0)
	{
		$Type = strtolower($Type);

		if ($Type == "year" && $StartY && $EndY)
		{
			if ($EndY < $StartY)
			{
				$Tmp = $EndY;
				$EndY = $StartY;
				$StartY = $Tmp;
			}
			$this->DateArrayDefinitions["year"] = array();
			for ($i=$StartY;$i<=$EndY;$i++)
				$this->DateArrayDefinitions["year"][$i] = $i;
		}

		if (is_array($this->DateArrayDefinitions[$Type]))
			if (sizeof($this->DateArrayDefinitions[$Type]))
				return ($this->DateArrayDefinitions[$Type]);

		$Start = 0;
		$End = 0;
		switch($Type)
		{
			case "meridian":
				$this->DateArrayDefinitions["meridian"] = array("am" => "am", "pm" => "pm");
				break;

			case "month_short":
				$this->DateArrayDefinitions["month_short"] = $this->GetDateSelect("month_long");
				foreach($this->DateArrayDefinitions["month_short"] as $Key => $Val)
					$this->DateArrayDefinitions["month_short"][$Key] = substr($Val, 0, 3);
				break;

			case "month_long":
				$this->DateArrayDefinitions["month_long"] = array();
				$this->DateArrayDefinitions["month_long"]["01"]  = "January";
				$this->DateArrayDefinitions["month_long"]["02"]  = "February";
				$this->DateArrayDefinitions["month_long"]["03"]  = "March";
				$this->DateArrayDefinitions["month_long"]["04"]  = "April";
				$this->DateArrayDefinitions["month_long"]["05"]  = "May";
				$this->DateArrayDefinitions["month_long"]["06"]  = "June";
				$this->DateArrayDefinitions["month_long"]["07"]  = "July";
				$this->DateArrayDefinitions["month_long"]["08"]  = "August";
				$this->DateArrayDefinitions["month_long"]["09"]  = "September";
				$this->DateArrayDefinitions["month_long"]["10"] = "October";
				$this->DateArrayDefinitions["month_long"]["11"] = "November";
				$this->DateArrayDefinitions["month_long"]["12"] = "December";
				break ;

			case "month":
				$Start = 1;
				$End = 12;
				break;

			case "day":
				$Start = 1;
				$End = 31;
				break;

			case "hour_12":
				$Start = 1;
				$End = 12;
				break;

			case "hour_24":
				$Start = 0;
				$End = 23;
				break;

			case "minute":
				$Start = 0;
				$End = 59;
				break;

			case "year":
				$Start = adodb_date("Y");
				$End = adodb_date("Y");
				break;
		}

		if ($End)
		{
			$this->DateArrayDefinitions[$Type] = array();
			for ($i=$Start;$i<=$End;$i++)
				if ($i < 10)
					$this->DateArrayDefinitions[$Type]["0".$i] = "0".$i;
				else
					$this->DateArrayDefinitions[$Type][$i] = $i;
		}

		return ($this->DateArrayDefinitions[$Type]);
	}
}

?>
