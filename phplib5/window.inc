<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

require_once(PHPLIB_DIR."variable.inc");

class window
{
	function load()
	{
		if (!window::sized())
			return(" onload=\"alertSize();\"");
		else
			return("");
	}

	function script($TargetURL)
	{
		if (!window::sized())
		{
			$JavaScript  = "";
			$JavaScript .= "<script>\n";
			$JavaScript .= "	function alertSize()\n";
			$JavaScript .= "	{\n";
			$JavaScript .= "		var myWidth = 0, myHeight = 0;\n";
			$JavaScript .= "		if(typeof(window.innerWidth) == 'number')\n";
			$JavaScript .= "		{\n";
			$JavaScript .= "			//Non-IE\n";
			$JavaScript .= "			myWidth = window.innerWidth;\n";
			$JavaScript .= "			myHeight = window.innerHeight;\n";
			$JavaScript .= "		}\n";
			$JavaScript .= "		else if(document.documentElement && (document.documentElement.clientWidth || document.documentElement.clientHeight))\n";
			$JavaScript .= "		{\n";
			$JavaScript .= "			//IE 6+ in 'standards compliant mode'\n";
			$JavaScript .= "			myWidth = document.documentElement.clientWidth;\n";
			$JavaScript .= "			myHeight = document.documentElement.clientHeight;\n";
			$JavaScript .= "		}\n";
			$JavaScript .= "		else if(document.body && (document.body.clientWidth || document.body.clientHeight))\n";
			$JavaScript .= "		{\n";
			$JavaScript .= "			//IE 4 compatible\n";
			$JavaScript .= "			myWidth = document.body.clientWidth;\n";
			$JavaScript .= "			myHeight = document.body.clientHeight;\n";
			$JavaScript .= "		}\n";

			if (strstr($TargetURL, "?"))
				$TargetURL .= "&WinC=1&WinW=";
			else
				$TargetURL .= "?WinC=1&WinW=";

			$JavaScript .= "		var reload_url = '".$TargetURL."' + myWidth + '&WinH=' + myHeight;\n";
		//	$JavaScript .= "		window.alert(reload_url);\n";
			$JavaScript .= "		location.href=reload_url;\n";

			$JavaScript .= "	}\n";
			$JavaScript .= "</script>\n";
			return($JavaScript);
		}
		else
			return("");
	}

	function sized()
	{
		if (defined("WINDOW_SIZED"))
			return(1);
		else
		{
			if (variable::GetItem("WinC"))
			{
				$W = variable::GetItem("WinW");
				$H = variable::GetItem("WinH");
				if ($W * $H)
					define("WINDOW_SIZED", $W."|".$H);
				else
					define("WINDOW_SIZED", "800|400");
				return(1);
			}
			return(0);
		}
	}

	function width()
	{
		if (window::sized())
		{
			$Parts = explode("|", WINDOW_SIZED);
			return($Parts[0]);
		}
		else
			return(0);
	}

	function height()
	{
		if (window::sized())
		{
			$Parts = explode("|", WINDOW_SIZED);
			return($Parts[1]);
		}
		else
			return(0);
	}
}

?>
