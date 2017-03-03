<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

class sql
{
	function keep_allowed($String, $Allowed)
	{
		$Characters = str_split($String);
		$Acceptible = str_split($Allowed);
		$Text = "";
		foreach($Characters as $Char)
			if (in_array($Char, $Acceptible))
				$Text .= $Char;
		return($Text);
	}

	function text($String)
	{
	//	if (defined("DB_SERVER"))
	//		$Link = mysql_connect(DB_SERVER, $GLOBALS["MY_USER"], $GLOBALS["MY_PASSWORD"]);
	//	else
	//		$Link = mysql_connect("localhost", $GLOBALS["MY_USER"], $GLOBALS["MY_PASSWORD"]);
//		$Link = mysql_connect("localhost", "", "");
//		return(mysql_real_escape_string($String));
		return(addslashes($String));
	}

	function text_array($Array)
	{
		foreach ($Array as $Key => $Val)
			$Array[$Key] = sql::text($Val);
		return($Array);
	}

	function number($String)
	{
		$Allowed = "01234567890.-";
		return(sql::keep_allowed($String, $Allowed));
	}

	function integer_array($Array)
	{
		foreach ($Array as $Key => $Val)
			$Array[$Key] = sql::integer($Val);
		return($Array);
	}

	function integer($String)
	{
		return(trim($String) * 1);
	}

	function stamp($String)
	{
		$Allowed = "01234567890 :-";
		return(sql::keep_allowed($String, $Allowed));
	}

	function date($String)
	{
		return(sql::stamp($String));
	}

	function time($String)
	{
		return(sql::stamp($String));
	}

	function datetime($String)
	{
		return(sql::stamp($String));
	}
}

?>
