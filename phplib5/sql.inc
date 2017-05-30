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
		if (defined("SQL_FUNC_LOGIN"))
			$User = SQL_FUNC_LOGIN;
		else
			$User = "";
		if (defined("SQL_FUNC_PASS"))
			$Pass = SQL_FUNC_PASS;
		else
			$Pass = "";
		if (defined("SQL_FUNC_HOST"))
			$Host = SQL_FUNC_HOST;
		else
			$Host = "localhost";

		if (trim($User))
		{
			$Link = mysql_connect($Host, $User, $Pass);
			return(mysql_real_escape_string($String));
		}
		else
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
