<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

class Browser
{
	function __construct()
	{
		$this->SetupArrays();

		$this->Agent = $_SERVER["HTTP_USER_AGENT"];

		$this->Agent = strtolower($this->Agent);
		$this->Agent = str_replace (array(";","/","  "), " ", $this->Agent);
		$this->Agent = str_replace ("mozilla firebird", "firebird", $this->Agent);
		$this->Parts = explode (" ", $this->Agent);
		$this->End["Parts"] = sizeof ($this->Parts) - 1;

		$this->OperatingSystem = $this->UStr;

		$this->Browser["Real"] = $this->Identify("Real", $RealID);
		if($this->Browser["Real"] != $this->UStr)
			$this->Version["Real"] = $RealID;

		$this->Browser["Effective"] = $this->Identify("Effective", $EffectiveID);
		if($this->Browser["Effective"] != $this->UStr)
			$this->Version["Effective"] = $EffectiveID;

		$this->OperatingSystem = $this->Identify("OS", $DudID);

		if ($this->OperatingSystem == "mac" && $this->Browser["Effective"] == "msie")
			$this->Version["Effective"]++;
	}

	function Verification()
	{
		echo "<H1>Actual Browser:</H1>";
		echo "<H3>".$this->Map[$this->Browser["Real"]]." ".$this->Version["Real"]."</H3>";

		echo "<H1>Effective Browser:</H1>";
		echo "<H3>".$this->Map[$this->Browser["Effective"]]." ".$this->Version["Effective"]."</H3>";

		echo "<H1>Operating System:</H1>";
		echo "<H3>".$this->Map[$this->OperatingSystem]."</H3>";
	}

	function Identify($Type, &$ID)
	{
		$i = 0;
		while ($i <= $this->End[$Type])
		{
			$j = $this->End["Parts"];
			while ($j >= 0)
			{
				if (substr_count($this->Parts[$j], $this->Data[$Type][$i]))
				{
					$ID = $this->CleanseVersion($this->Parts[$j + 1]);
					return($this->Data[$Type][$i]);
				}
				$j--;
			}
			$i++;
		}
		return($this->UStr);
	}

	function CleanseVersion($Version)
	{
		return ($Version * 1);
	}

	function SetupArrays()
	{
		$this->UStr = "Unidentified";
		$this->Browser = array();
		$this->Browser["Real"] = $this->UStr;
		$this->Browser["Effective"] = $this->UStr;

		$this->Version = array();
		$this->Version["Real"] = 0;
		$this->Version["Effective"] = 0;

		$this->Data = array();
		$this->Data["Real"] = array("lynx", "opera", "konqueror", "navigator", "firebird", "msie", "mozilla");
		$this->Data["Effective"] = array("lynx", "msie", "mozilla", "opera");
		$this->Data["OS"] = array("win", "mac", "linux");

		$this->End = array();
		$this->End["Real"] = sizeof($this->Data["Real"]) - 1;
		$this->End["Effective"] = sizeof($this->Data["Effective"]) - 1;
		$this->End["OS"] = sizeof($this->Data["OS"]) - 1;

		$this->Map = array();
		$this->Map["lynx"] = "Lynx";
		$this->Map["opera"] = "Opera";
		$this->Map["konqueror"] = "Konqueror";
		$this->Map["navigator"] = "Netscape Navigator";
		$this->Map["firebird"] = "Mozilla Firebird";
		$this->Map["mozilla"] = "Mozilla";
		$this->Map["msie"] = "Microsoft Internet Explorer";
		$this->Map["win"] = "Windows";
		$this->Map["mac"] = "Macintosh";
		$this->Map["linux"] = "Linux";
		$this->Map["Unidentified"] = "Unidentified";
	}

	function CheckOS($OS)
	{
		return (strtolower($OS) == $this->OperatingSystem);
	}

	function CheckBrowser($Browser, $Lower = 0, $Upper = 99)
	{
		if ($this->Browser["Effective"] != $Browser)
			return(0);
		if ($this->Version["Effective"] < $Lower)
			return(0);
		if ($this->Version["Effective"] > $Upper)
			return(0);
		return(1);
	}

	function Name()
	{
		if ($this->Version["Real"])
			return ($this->Map[$this->Browser["Real"]]." ".$this->Version["Real"]);
		else
			return ($this->Map[$this->Browser["Real"]]." Browser");
	}

	function SmartCheck($String)
	{
		$String = strtolower($String);
		$String = str_replace (array("(", ")"), array(" ( ", " ) "), $String);
		$Parts = explode(" ", $String);
		$New = array();
		$Command = "";
		$Params = 0;
		foreach ($Parts as $Word)
		{
			switch ($Command)
			{
				case "browser":
					if ($Params)
					{
						$Ver = explode ("-", $Word);
						$New[] = ", ".($Ver[0] * 1);
						if ($Ver[1])
							$New[] = ", ".($Ver[1] * 1);
						$New[] = ")";
						$Command = "";
						$Params = 0;
					}
					else
					{
						$New[] = "\"".$Word."\"";
						$Params++;
					}
					break;
				case "os":
					$New[] = "\"".$Word."\")";
					$Command = "";
					$Params = 0;
					break;
				default:
					switch ($Word)
					{
						case "browser":
							$New[] = "\$this->CheckBrowser(";
							$Command = "browser";
							$Params = 0;
							break;
						case "os":
							$New[] = "\$this->CheckOS(";
							$Command = "os";
							$Params = 0;
							break;
						default:
							$New[] = $Word;
							break;
					}
					break;
			}
		
		}

		$String = implode(" ", $New);

		$String = str_replace (array(" and "," or ", "not "), array(" && ", " || ", " !"), $String);
		$String = "\$Result = ".$String.";";
		eval($String);

		return ($Result);
	}
}

function BrowserCheckOS($OS)
{
	GLOBAL $BROWSER_CLASS;
	if (gettype($BROWSER_CLASS) == "object")
	{
		if (strtolower(get_class($BROWSER_CLASS)) != "browser")
			$BROWSER_CLASS = new Browser;
	}
	else
		$BROWSER_CLASS = new Browser;
	return ($BROWSER_CLASS->CheckOS($OS));
}

function BrowserCheck($Browser, $Lower = 0, $Upper = 99)
{
	GLOBAL $BROWSER_CLASS;
	if (gettype($BROWSER_CLASS) == "object")
	{
		if (strtolower(get_class($BROWSER_CLASS)) != "browser")
			$BROWSER_CLASS = new Browser;
	}
	else
		$BROWSER_CLASS = new Browser;
	return ($BROWSER_CLASS->CheckBrowser($Browser, $Lower, $Upper));
}

function BrowserSmartCheck($String)
{
	GLOBAL $BROWSER_CLASS;
	if (gettype($BROWSER_CLASS) == "object")
	{
		if (strtolower(get_class($BROWSER_CLASS)) != "browser")
			$BROWSER_CLASS = new Browser;
	}
	else
		$BROWSER_CLASS = new Browser;
	return ($BROWSER_CLASS->SmartCheck($String));
}

function ShowBrowserName()
{
	GLOBAL $BROWSER_CLASS;
	if (gettype($BROWSER_CLASS) == "object")
	{
		if (strtolower(get_class($BROWSER_CLASS)) != "browser")
			$BROWSER_CLASS = new Browser;
	}
	else
		$BROWSER_CLASS = new Browser;
	return ($BROWSER_CLASS->Name());
}

?>
