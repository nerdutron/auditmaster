<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

require_once(PHPLIB_DIR."adodb-time.inc");

// strips all html tags from a string
function StripTags($String)
{
	$Exploded = explode("<", ">".$String);

	for ($i=0;$i<sizeof($Exploded);$i++)
	{
		if (strchr($Exploded[$i], ">"))
		{
			$Start = strrpos($Exploded[$i], ">") + 1;
			$Length = strlen($Exploded[$i]) - $Start;
			$Exploded[$i] = substr($Exploded[$i], $Start, $Length);
		}
		else
	 		$Exploded[$i] = "";
	}
	return (implode($Exploded, ""));
}

function CreateUniqueKey($L)
{
	$salt = "abcdefghijklmnopqrstuvwxyz0123456789";
	srand((double)microtime()*1000000);
	$i = 0;
	while ($i <= ($L-1))
	{
		$num = rand() % 36;
		$tmp = substr($salt, $num, 1);
		$str = $str . $tmp;
		$i++;
	}
	return ($str);
}

function CreateUniquePassword($L)
{
	$salt = "abcdefghijkmnpqrstuvwxyz23456789";
	srand((double)microtime()*1000000);
	$i = 0;
	while ($i <= ($L-1))
	{
		$num = rand() % 32;
		$tmp = substr($salt, $num, 1);
		$str = $str . $tmp;
		$i++;
	}
	return ($str);
}

function OpenJSBox($FName, $W, $H)
{
	echo "<SCRIPT><!--\n";
	echo "function ".$FName."(EditProg)\n";
	echo "{\n";
	echo "	Rand = Math.round(Math.random()*1000);\n";
	echo "	EditProg = EditProg + '&RN=' + Rand;\n";
	echo "	window.open(EditProg, '".$FName."', 'toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=".$W.",height=".$H."');\n";
	echo "}\n";
	echo "// --></SCRIPT>\n";
}

//	Converts any MySQL date/time into a PHP timestamp
// 	Input must be exactly:
//		HH:MM:SS		(8 characters)
//		YYYY-MM-DD		(10 characters)
//		YYYYMMDDHHMMSS		(14 characters)
//		YYYY-MM-DD HH:MM:SS	(19 characters)
//
//	Usage: $TimeStamp = CreateTimeStamp($MySQLDate);
//	Murray Shields -> 03/10/1998
//	Murray Shields -> 12/09/2002
function CreateTimeStamp($D, $ModifyString = "")
{
	$TimeStamp = 0;

	switch (strlen($D))
	{
		case 8:
			$TimeStamp = adodb_mktime(
				substr($D,0,2),
				substr($D,3,2),
				substr($D,6,2),
				adodb_date("n"),
				adodb_date("j"),
				adodb_date("Y")
				);
			break;
		case 10:
			$TimeStamp = adodb_mktime(
				0,
				0,
				0,
				substr($D,5,2),
				substr($D,8,2),
				substr($D,0,4)
				);
			break;
		case 14:
			$TimeStamp = adodb_mktime(
				substr($D,8,2),
				substr($D,10,2),
				substr($D,12,2),
				substr($D,4,2),
				substr($D,6,2),
				substr($D,0,4)
				);
			break;
		case 19:
			$TimeStamp = adodb_mktime(
				substr($D,11,2),
				substr($D,14,2),
				substr($D,17,2),
				substr($D,5,2),
				substr($D,8,2),
				substr($D,0,4)
				);
			break;
	}

	if ($TimeStamp && ($ModifyString != ""))
		$TimeStamp = strtotime($ModifyString, $TimeStamp);

	return ($TimeStamp);
}

function ObscureEmail($Email)
{
	$Conversion = array
		("@" => "064", "." => "046", "-" => "045", "_" => "095",

		 "0" => "048", "1" => "049", "2" => "050", "3" => "051", "4" => "052",
		 "5" => "053", "6" => "054", "7" => "055", "8" => "056", "9" => "057",

		 "A" => "065", "B" => "066", "C" => "067", "D" => "068", "E" => "069",
		 "F" => "070", "G" => "071", "H" => "072", "I" => "073", "J" => "074",
		 "K" => "075", "L" => "076", "M" => "077", "N" => "078", "O" => "079",
		 "P" => "080", "Q" => "081", "R" => "082", "S" => "083", "T" => "084",
		 "U" => "085", "V" => "086", "W" => "087", "X" => "088", "Y" => "089",
		 "Z" => "090",

		 "a" => "097", "b" => "098", "c" => "099", "d" => "100", "e" => "101",
		 "f" => "102", "g" => "103", "h" => "104", "i" => "105", "j" => "106",
		 "k" => "107", "l" => "108", "m" => "109", "n" => "110", "o" => "111",
		 "p" => "112", "q" => "113", "r" => "114", "s" => "115", "t" => "116",
		 "u" => "117", "v" => "118", "w" => "119", "x" => "120", "y" => "121",
		 "z" => "122"
		);
	$Encoded = "";
	for ($i=0;$i<strlen($Email);$i++)
	{
		$ThisCHR = substr($Email, $i, 1);
		if ($Conversion[$ThisCHR] != "")
			$Encoded .= "&#".$Conversion[$ThisCHR].";";
		else
			$Encoded .= $ThisCHR;
	}
	return ($Encoded);
}

function GetArraySpaces($n)
{
	$Str = "";
	for ($i=0;$i<$n;$i++)
		$Str .= " ";
	return ($Str);
}

function GetArrayContents($ArrayName, $Depth = 0)
{
	$Str = "";

	foreach ($ArrayName as $Key => $Val)
	{
		if (is_array($Val))
		{
			$Str .= GetArraySpaces($Depth);
			$Str .= "Key[".$Key."] => array:\n";
			$Str .= GetArrayContents($Val, ($Depth + 3));
		}
		else
		{
			$Str .= GetArraySpaces($Depth);
			$Str .= "Key[".$Key."] => ".$Val."\n";
		}
	}

	return ($Str);
}

function PrintArrayContents($ArrayName)
{
	echo "<HR><PRE>\n";
	if (is_array($ArrayName))
		echo GetArrayContents($ArrayName);
	else
		echo "Not an array.";
	echo "\n</PRE><HR>";
}

function ConvertToRoman($Number)
{
	$Map = array(
		array("roman" => "I", "arabic" => 1),
		array("roman" => "V", "arabic" => 5),
		array("roman" => "X", "arabic" => 10),
		array("roman" => "L", "arabic" => 50),
		array("roman" => "C", "arabic" => 100),
		array("roman" => "D", "arabic" => 500),
		array("roman" => "M", "arabic" => 1000),
		array("roman" => "V", "arabic" => 5000)
	);

	$RomanNumber = "";
	for ($i=sizeof($Map)-2;$i>=0;$i=$i-2)
	{
		$ArabicCount = floor($Number / $Map[$i]["arabic"]);
		$Number = $Number - $ArabicCount * $Map[$i]["arabic"];
		switch($ArabicCount)
		{
			case 1:
			case 2:
			case 3:
				for ($c=1;$c<=$ArabicCount;$c++)
					$RomanNumber .= $Map[$i]["roman"];
				break;
			case 4:
				$RomanNumber .= $Map[$i]["roman"];
				$RomanNumber .= $Map[$i + 1]["roman"];
				break;
			case 5:
			case 6:
			case 7:
			case 8:
				$RomanNumber .= $Map[$i + 1]["roman"];
				for ($c=1;$c<=($ArabicCount-5);$c++)
					$RomanNumber .= $Map[$i]["roman"];
				break;
			case 9:
				$RomanNumber .= $Map[$i]["roman"];
				$RomanNumber .= $Map[$i + 2]["roman"];
				break;
		}
	}

	return ($RomanNumber);
}

function ConvertToAlpha($Number, $Max = 0)
{
	if ($Max)
		$MaxNum = ConvertToAlpha($Max);
	$MinLength = strlen($MaxNum);

	$TransposeArray["0"] = "A"; //  1
	$TransposeArray["1"] = "B"; //  2
	$TransposeArray["2"] = "C"; //  3
	$TransposeArray["3"] = "D"; //  4
	$TransposeArray["4"] = "E"; //  5
	$TransposeArray["5"] = "F"; //  6
	$TransposeArray["6"] = "G"; //  7
	$TransposeArray["7"] = "H"; //  8
	$TransposeArray["8"] = "I"; //  9
	$TransposeArray["9"] = "J"; // 10
	$TransposeArray["a"] = "K"; // 11
	$TransposeArray["b"] = "L"; // 12
	$TransposeArray["c"] = "M"; // 13
	$TransposeArray["d"] = "N"; // 14
	$TransposeArray["e"] = "O"; // 15
	$TransposeArray["f"] = "P"; // 16
	$TransposeArray["g"] = "Q"; // 17
	$TransposeArray["h"] = "R"; // 18
	$TransposeArray["i"] = "S"; // 19
	$TransposeArray["j"] = "T"; // 20
	$TransposeArray["k"] = "U"; // 21
	$TransposeArray["l"] = "V"; // 22
	$TransposeArray["m"] = "W"; // 23
	$TransposeArray["n"] = "X"; // 24
	$TransposeArray["o"] = "Y"; // 25
	$TransposeArray["p"] = "Z"; // 26

	$Number--;

	$Str = base_convert($Number, 10, 26);

	$StrLen = strlen($Str);

	for($i=0;$i<$StrLen;$i++)
	{
		$Bit = substr($Str,$i,1);
		$FileBit = $TransposeArray[$Bit];
		$NumberString .= $FileBit;
	}

	for ($i=0;$i<($MinLength - strlen($NumberString));$i++)
		$NumberString = "A".$NumberString;

	return ($NumberString);
}

function GetXML($XML)
{
	$Text  = "";
	$Text .= "<pre>";
	$Text .= htmlspecialchars($XML);
	$Text .= "</pre>";
	return($Text);
}

function ShowXML($XML)
{
	echo GetXML($XML);
}

function DiskLogFile($Name)
{
	$GLOBALS["DISK_LOG_FILE"] = $Name;
}

function DiskLogRecord($Category)
{
	if ($GLOBALS["DISK_LOG_MODE"] === "ALL")
		return;
	if (strtoupper($Category) == "ALL")
	{
		$GLOBALS["DISK_LOG_MODE"] = "ALL";
		return;
	}
	if (!is_array($GLOBALS["DISK_LOG_MODE"]))
		$GLOBALS["DISK_LOG_MODE"] = array();
	$GLOBALS["DISK_LOG_MODE"][] = strtoupper($Category);
	
}

function DiskLogEntry($Category, $String)
{
	if (is_array($GLOBALS["DISK_LOG_MODE"]))
	{
		if ($GLOBALS["DISK_LOG_MODE"] === "ALL" || in_array(strtoupper($Category), $GLOBALS["DISK_LOG_MODE"]))
		{
			if ($GLOBALS["DISK_LOG_FILE"])
			{
				$Command = "echo \"".date("Y-m-d H:i:s")." >> ".$String."\" >> /tmp/".$GLOBALS["DISK_LOG_FILE"].".log";
				exec($Command);
			}
		}
	}
}

function ShowCSV($Data)
{
	$T = new Table;
	$Lines = explode("\n", $Data);
	foreach ($Lines as $Line)
	{
		$Words = explode("\n", $Line);
		foreach ($Words as $Word)
			$T->AddData(trim($Word)."&nbsp;");
		$T->EndRow();
	}
	$T->Put();
}

function ShowTSV($Data)
{
	$T = new Table;
	$Lines = explode("\n", $Data);
	foreach ($Lines as $Line)
	{
		$Words = explode("\t", $Line);
		foreach ($Words as $Word)
			$T->AddData(trim($Word)."&nbsp;");
		$T->EndRow();
	}
	$T->Put();
}

function ParseDateString($String)
{
	$Parts = explode("/", $String);
	$D = $Parts[0] * 1;
	$M = $Parts[1] * 1;
	$Y = $Parts[2] * 1;
	if ($Y < 100)
	{
		$Current = date("y") + 5;
		if ($Y <=$Current)
			$Y = $Y + 2000;
		else
			$Y = $Y + 1900;
	}
	if (checkdate($M, $D, $Y))
		return(date("Y-m-d", mktime(0, 0, 0, $M, $D, $Y)));
	else
		return("");
}

function DeliverInlineHeaders($MIME)
{
	Header("Content-type: ".$MIME);
}

function DeliverInline(&$Data, $MIME)
{
	DeliverInlineHeaders($MIME);
	echo $Data;
	exit;
}

function DeliverDownloadHeaders($Filename, $Length)
{
	Header("Content-Disposition: attachment; filename=".$Filename);
	Header("Content-Length: ".$Length);
}

function DeliverDownload(&$Data, $MIME, $Filename)
{
	DeliverDownloadHeaders($Filename, strlen($Data));
	DeliverInline($Data, $MIME);
}

?>
