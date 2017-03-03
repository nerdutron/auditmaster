<?php

/* Copyright Murray Shields 2006 */

require_once(PHPLIB_DIR."variable.inc");
require_once(PHPLIB_DIR."tools.inc");
require_once(PHPLIB_DIR."table.inc");

class ajax_date extends Variable
{
	function __construct($URL, $ImageSize = 15)
	{
		$this->ImageSize = $ImageSize;
		parent::__construct();
		if ($this->StripGetV("Go"))
			$this->arrow($this->StripGetV("Go"));
		$this->URL = $URL;
		$this->Set = array();
		$this->Mode = array();
	}

	function define($Name, $Date, $Label = "Change Date", $Format = "d/m/Y", $Mode = "hidden")
	{
		if (!is_array($this->Set))
			$this->Set = array();
		if (!is_array($this->Mode))
			$this->Mode = array();
		$NameID = str_replace(array("[", "]"), "_", $Name);
		if (in_array(strtolower($Mode), array("text","hidden","edit")))
			$this->Mode[$NameID] = strtolower($Mode);
		else
			$this->Mode[$NameID] = "hidden";
//		if (in_array(strtolower($Mode), array("text","edit")))
//			$this->Mode[$NameID] = "text\" size=\"10\" maxlength=\"10";
		$this->Set[$NameID]["Name"]   = $Name;
		$this->Set[$NameID]["Date"]   = $Date;
		$this->Set[$NameID]["Label"]  = $Label;
		$this->Set[$NameID]["Format"] = $Format;
//		printarraycontents($this->Mode);
		return ($NameID);
	}

	function is_mysql_date($Date)
	{
		$Parts = explode("-", $Date);
		$D = $Parts[2] * 1;
		$M = $Parts[1] * 1;
		$Y = $Parts[0] * 1;
		if ($Y < 100)
			$Y = $Y + 2000;
		if (checkdate($M, $D, $Y) && ($D * $M * $Y))
			return(1);
		else
			return(0);
	}

	function is_nice_date($Date)
	{
		$Parts = explode("/", $Date);
		$D = $Parts[0] * 1;
		$M = $Parts[1] * 1;
		$Y = $Parts[2] * 1;
		if ($Y < 100)
			$Y = $Y + 2000;
		if (checkdate($M, $D, $Y) && ($D * $M * $Y))
			return(date("Y-m-d", mktime(0, 0, 0, $M, $D, $Y)));
		else
			return("");
	}

	function get_data($NameID, $Date)
	{
		if (in_array($Date, array("", "0000-00-00")))
			$Value = "0000-00-00";
		else
			$Value = $Date;
		switch($this->Mode[$NameID])
		{
			case "text":
				$Mode = "text\" size=\"10\" maxlength=\"10";
				break;
			case "edit":
				$Mode = "text\" size=\"10\" maxlength=\"10";
				if ($Value == "0000-00-00")
					$Value = "";
				else
				{
					if ($this->is_mysql_date($Date))
						$Value = date("d/m/Y", CreateTimeStamp($Date));
					else
					{
						$Parts = explode("-", $Value);
						$D = $Parts[2] * 1;
						$M = $Parts[1] * 1;
						$Y = $Parts[0] * 1;
						if ($Y < 100)
							$Y = $Y + 2000;
						if (checkdate($M, $D, $Y) && ($D * $M * $Y))
							$Value = date("d/m/Y", CreateTimeStamp($Value));
					}
				}
				break;
			case "hidden":
				$Mode = "hidden";
				break;
		}
		$Name = $this->Set[$NameID]["Name"];
		$String = $NameID."_Data|<input type=\"".$Mode."\" name=\"".$Name."\" value=\"".$Value."\"/>";
		return ($String);
	}

	function get_view($NameID, $Date)
	{
		if (in_array($Date, array("", "0000-00-00")))
			return ($NameID."_View| No Date Set");
		else
		{
			$Nice = $this->is_nice_date($Date);
			if ($this->is_mysql_date($Date))
				return ($NameID."_View| ".adodb_date($this->Set[$NameID]["Format"], CreateTimeStamp($Date)));
			elseif ($this->is_mysql_date($Nice))
				return ($NameID."_View| ".adodb_date($this->Set[$NameID]["Format"], CreateTimeStamp($Nice)));
			else
				return ($NameID."_View| No Date Set");
		}
	}

	function get_link($NameID, $Date)
	{
		$Label = $this->Set[$NameID]["Label"];
		if (in_array($Date, array("", "0000-00-00")))
			return ($NameID."_Link| [<a href=\"javascript:GetAjaxDate('".$NameID."','open','0000-00-00')\">".$Label."</a>]");
		else
		{
			$Nice = $this->is_nice_date($Date);
			if ($this->is_mysql_date($Date))
				return ($NameID."_Link| [<a href=\"javascript:GetAjaxDate('".$NameID."','open','".$Date."')\">".$Label."</a>]");
			elseif ($this->is_mysql_date($Nice))
				return ($NameID."_Link| [<a href=\"javascript:GetAjaxDate('".$NameID."','open','".$Nice."')\">".$Label."</a>]");
			else
				return ($NameID."_Link| [<a href=\"javascript:GetAjaxDate('".$NameID."','open','0000-00-00')\">".$Label."</a>]");
		}
	}

	function get_work($NameID, $Date)
	{
		if (in_array($Date, array("", "0000-00-00")))
			$Date = adodb_date("Y-m-d");

		$String   = "";
		$String  .= $NameID."_Work|";

		$String .= "<div class=\"jx_border\">";

		$UseStamp = CreateTimeStamp($Date);
		$UseDate = adodb_date("Y-m-d", $UseStamp);

		$String .= "<h2>";
		$String .= "<a href=\"javascript:GetAjaxDate('".$NameID."','wipe','')\">";
		$String .= " Clear";
		$String .= "</a>";
		$String .= "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
		$String .= "<a href=\"javascript:GetAjaxDate('".$NameID."','exit','".$this->Set[$NameID]["Date"] = $Date."')\">";
		$String .= " Close";
		$String .= "</a>";
		$String .= "</h2>";

		$T = new Table;

		$T->AddData($this->arrow_link($NameID, $UseDate, "-10 yr", "LD"), 1, 1, "", "white");
		$T->AddData($this->arrow_link($NameID, $UseDate, "-1 yr", "L"), 1, 1, "", "white");
		$T->AddData(adodb_date("Y", $UseStamp), 3, 1, "", "white");
		$T->AddData($this->arrow_link($NameID, $UseDate, "1 yr", "R"), 1, 1, "", "white");
		$T->AddData($this->arrow_link($NameID, $UseDate, "10 yr", "RD"), 1, 1, "", "white");
		$T->EndRow();

		$T->AddData($this->arrow_link($NameID, $UseDate, "-1 mo", "L"), 1, 1, "", "white");
		$T->AddData(adodb_date("F", $UseStamp), 5, 1, "", "white");
		$T->AddData($this->arrow_link($NameID, $UseDate, "1 mo", "R"), 1, 1, "", "white");
		$T->EndRow();

		$T->AddHead("S");
		$T->AddHead("M");
		$T->AddHead("T");
		$T->AddHead("W");
		$T->AddHead("T");
		$T->AddHead("F");
		$T->AddHead("S");
		$T->EndRow();

		$Year = adodb_date("Y", $UseStamp);
		$Month = adodb_date("n", $UseStamp);
		$DaysInMonth = adodb_date("t", $UseStamp);
		$Start = adodb_date("w", CreateTimeStamp(adodb_date("Y-m-01", $UseStamp)));
		for ($i=0;$i<$Start;$i++)
			$T->AddData("-", 1, 1, "", "blank");

		for($Day=1;$Day<=$DaysInMonth;$Day++)
		{
			$Use = adodb_mktime(0, 0, 0, $Month, $Day, $Year);
			$URL  = "";
			$URL .= "javascript:GetAjaxDate('".$NameID."','date','";
			$URL .= adodb_date("Y-m-d", $Use);
			$URL .= "')";
			if (in_array(adodb_date("w", $Use), array(0,6)))
				$T->AddData("<a href=\"".$URL."\">".$Day."</a>", 1, 1, "", "weekend");
			else
				$T->AddData("<a href=\"".$URL."\">".$Day."</a>");
			if (adodb_date("w", $Use) == 6)
				$T->EndRow();
		}

		$Ends = adodb_date("w", $Use);
		if ($Ends < 6)
		{
			for ($i=0;$i<6-$Ends;$i++)
				$T->AddData("-", 1, 1, "", "blank");
			$T->EndRow();
		}
		$String .= $T->Get();

		$String .= "</div>";
		return($String);
	}

	function response()
	{
		$this->StripMakeL("JXNameID", "JXMode", "JXDate");

		switch($this->JXMode)
		{
			case "open":
				echo $this->get_work($this->JXNameID, $this->JXDate);
				exit;
				break;
			case "move":
				echo $this->get_work($this->JXNameID, $this->JXDate);
				exit;
				break;
			case "date":
				echo $this->get_data($this->JXNameID, $this->JXDate);
				echo "|";
				echo $this->get_view($this->JXNameID, $this->JXDate);
				echo "|";
				echo $this->get_link($this->JXNameID, $this->JXDate);
				echo "|";
				echo $this->JXNameID."_Work|";
				exit;
				break;
			case "wipe":
				echo $this->get_data($this->JXNameID, "0000-00-00");
				echo "|";
				echo $this->get_view($this->JXNameID, "0000-00-00");
				echo "|";
				echo $this->get_link($this->JXNameID, "0000-00-00");
				echo "|";
				echo $this->JXNameID."_Work|";
				exit;
				break;
			case "exit":
				echo $this->JXNameID."_Work|";
				exit;
				break;
		}
	}

	function content($NameID, $End = 0)
	{
		$String  = "";
		if (!$End)
			$String .= "<div class=\"jx_date\" id=\"".$NameID."_Work\"></div> &nbsp; ";
		$Parts = explode("|", $this->get_data($NameID, $this->Set[$NameID]["Date"]));
		$String .= "<span id=\"".$NameID."_Data\">".$Parts[1]."</span>";
		$Parts = explode("|", $this->get_view($NameID, $this->Set[$NameID]["Date"]));
		$String .= "<span id=\"".$NameID."_View\">".$Parts[1]."</span>";
		$Parts = explode("|", $this->get_link($NameID, $this->Set[$NameID]["Date"]));
		$String .= "<span id=\"".$NameID."_Link\">".$Parts[1]."</span>";
		if ($End)
			$String .= "<div class=\"jx_date\" id=\"".$NameID."_Work\"></div>";
		return ($String);
	}

	function head($Limited = 0)
	{
		if (!$GLOBALS["AJAX_DATE_SCRIPTS"])
		{
			$GLOBALS["AJAX_DATE_SCRIPTS"] = 1;

			echo "<style>\n\n";

			echo "div.jx_date\n";
			echo "{\n";
			echo "	border: none;\n";
			echo "	display: block;\n";
			echo "	position: absolute;\n";
			echo "	padding: 0px;\n";
			echo "	z-index: 99;\n";
			echo "}\n\n";

			echo "div.jx_border\n";
			echo "{\n";
			echo "	border: 2px solid #000000;\n";
			echo "	padding: 5px;\n";
			echo "	background-color: #FFFFFF;\n";
			echo "	z-index: 999;\n";
			echo "}\n\n";

			echo "div.jx_date img\n";
			echo "{\n";
			echo "	border: none;\n";
			echo "}\n\n";

			echo "div.jx_date h1, div.jx_date h2\n";
			echo "{\n";
			echo "	font-family: sans-serif;\n";
			echo "	font-style: normal;\n";
			echo "	font-weight: bold;\n";
			echo "	padding: 2px;\n";
			echo "	margin: 2px;\n";
			echo "}\n\n";

			echo "div.jx_date h1\n";
			echo "{\n";
			echo "	font-size: 1.4em;\n";
			echo "}\n\n";

			echo "div.jx_date h2\n";
			echo "{\n";
			echo "	font-size: 1.3em;\n";
			echo "}\n\n";

			echo "div.jx_date table\n";
			echo "{\n";
			echo "	position: relative;\n";
			echo "	margin: 0;\n";
			echo "	padding: 0;\n";
			echo "	border-collapse: collapse;\n";
			echo "	border: none;\n";
			echo "	font-family: sans-serif;\n";
			echo "	font-size: 1.2em;\n";
			echo "}\n\n";

			echo "div.jx_date td, div.jx_date th\n";
			echo "{\n";
			echo "	padding: 2px;\n";
			echo "	border: 1px solid #999999;\n";
			echo "	text-align: center;\n";
			echo "	vertical-align: top;\n";
			echo "}\n\n";

			echo "div.jx_date th\n";
			echo "{\n";
			echo "	background-color: #666666;\n";
			echo "	color: #FFFFFF;\n";
			echo "}\n\n";

			echo "div.jx_date td\n";
			echo "{\n";
			echo "	background-color: #CCCCCC;\n";
			echo "	color: #333333;\n";
			echo "}\n\n";

			echo "div.jx_date td.weekend\n";
			echo "{\n";
			echo "	background-color: #999999;\n";
			echo "	color: #333333;\n";
			echo "}\n\n";

			echo "div.jx_date td.blank\n";
			echo "{\n";
			echo "	background-color: #FFFFFF;\n";
			echo "	color: #FFFFFF;\n";
			echo "}\n\n";

			echo "div.jx_date td.white\n";
			echo "{\n";
			echo "	background-color: #FFFFFF;\n";
			echo "	font-size: 1.1em;\n";
			echo "	font-family: sans-serif;\n";
			echo "	font-style: normal;\n";
			echo "	font-weight: bold;\n";
			echo "}\n\n";

			echo "</style>\n\n";

			echo "<script>\n\n";

			if (!$Limited)
			{
				echo "function createRequestObject()\n";
				echo "{\n";
				echo "	var ro;\n";
				echo "	var browser = navigator.appName;\n";
				echo "	if(browser == \"Microsoft Internet Explorer\")\n";
				echo "	{\n";
				echo "		ro = new ActiveXObject(\"Microsoft.XMLHTTP\");\n";
				echo "	}\n";
				echo "	else\n";
				echo "	{\n";
				echo "		ro = new XMLHttpRequest();\n";
				echo "	}\n";
				echo "	return ro;\n";
				echo "}\n\n";

				echo "var http = createRequestObject();\n\n";

				echo "function handleResponse()\n";
				echo "{\n";
				echo "	if(http.readyState == 4)\n";
				echo "	{\n";
				echo "		var response = http.responseText;\n";
				echo "		var update = new Array();\n";
				echo "		if(response.indexOf('|' != -1))\n";
				echo "		{\n";
				echo "			update = response.split('|');\n";
				echo "			done = 0;\n";
				echo "			i = 0;\n";
				echo "			while (done == 0)\n";
				echo "			{\n";
				echo "				j = i ;\n";
				echo "				k = i + 1;\n";
				echo "				if (update[j])\n";
				echo "				{\n";
				echo "					document.getElementById(update[j]).innerHTML = update[k];\n";
				echo "					i = i + 2;\n";
				echo "				}\n";
				echo "				else\n";
				echo "				{\n";
				echo "					done = 1;\n";
				echo "				}\n";
				echo "			}\n";
				echo "		}\n";
				echo "	}\n";
				echo "}\n\n";
			}

			echo "function GetAjaxDate(id,mode,date)\n";
			echo "{\n";
//			echo "	alert('".$this->URL."JXNameID='+id+'&JXMode='+mode+'&JXDate='+date);\n";
			echo "	http.open('get', '".$this->URL."JXNameID='+id+'&JXMode='+mode+'&JXDate='+date);\n";
			echo "	http.onreadystatechange = handleResponse;\n";
			echo "	http.send(null);\n";
			echo "}\n\n";

			echo "</script>\n\n";
		}
	}

	function arrow_link($NameID, $Date, $Modify, $Direction = "X")
	{
		$URL  = "";
		$URL .= "<a href=\"javascript:GetAjaxDate('".$NameID."','move','";

		$Stamp = CreateTimeStamp($Date);
		$Y = adodb_date("Y", $Stamp);
		$M = adodb_date("m", $Stamp);
		$D = adodb_date("d", $Stamp);
		$Parts = explode(" ", $Modify);
		if ($Parts[1] == "yr")
		{
			$Y = $Y + $Parts[0];
			if (($M == "02") && ($D == "29"))
				$D = "28";
		}
		else
		{
			$M = $M + $Parts[0];
			if ($M == 0)
			{
				$M = 12;
				$Y--;
			}
			if ($M == 13)
			{
				$M = 1;
				$Y++;
			}
			$SN = adodb_mktime(0, 0, 0, $M, 1, $Y);
			if ($D > adodb_date("t", $SN))
				$D = adodb_date("t", $SN);
		}

		$URL .= adodb_date("Y-m-d", adodb_mktime(0, 0, 0, $M, $D, $Y));

		$URL .= "')\">";
		$URL .= $this->arrow_url($Direction);
		$URL .= "</a>";
		return ($URL);
	}

	function arrow_url($Direction = "X")
	{
		return("<img src=\"".$this->URL."Go=".$Direction."\" />");
	}

	function arrow($Go)
	{
		$W = $this->ImageSize;
		$H = $this->ImageSize;
		$B = 1;

		$Points1 = array();
		$Points2 = array();

		$BGH = "#FFFFFF";
		$FGH = "#000000";

		$XY = array();
		$XY["XL"] = $B;
		$XY["XM"] = round($W / 2) -1;
		$XY["XR"] = $W - $B - 1;
		$XY["YT"] = $B;
		$XY["YM"] = round($H / 2) -1;
		$XY["YB"] = $H - $B - 1;
		switch(strtoupper($Go))
		{
			case "U":
				$Points1 = array($XY["XM"], $XY["YT"], $XY["XL"], $XY["YB"], $XY["XR"], $XY["YB"]);
				break;
			case "D":
				$Points1 = array($XY["XM"], $XY["YB"], $XY["XL"], $XY["YT"], $XY["XR"], $XY["YT"]);
				break;
			case "R":
				$Points1 = array($XY["XL"], $XY["YT"], $XY["XL"], $XY["YB"], $XY["XR"], $XY["YM"]);
				break;
			case "L":
				$Points1 = array($XY["XL"], $XY["YM"], $XY["XR"], $XY["YT"], $XY["XR"], $XY["YB"]);
				break;
			case "S":
				$Points1 = array($XY["XL"], $XY["YT"], $XY["XL"], $XY["YB"], $XY["XR"], $XY["YB"], $XY["XR"], $XY["YT"]);
				break;
			case "RD":
				$Points1 = array($XY["XL"], $XY["YT"], $XY["XL"], $XY["YB"], $XY["XM"], $XY["YM"]);
				$Points2 = array($XY["XM"], $XY["YT"], $XY["XM"], $XY["YB"], $XY["XR"], $XY["YM"]);
				break;
			case "LD":
				$Points1 = array($XY["XL"], $XY["YM"], $XY["XM"], $XY["YT"], $XY["XM"], $XY["YB"]);
				$Points2 = array($XY["XM"], $XY["YM"], $XY["XR"], $XY["YT"], $XY["XR"], $XY["YB"]);
				break;
			default:
				$Points1 = array($XY["XM"], $XY["YT"], $XY["XR"], $XY["YM"], $XY["XM"], $XY["YB"], $XY["XL"], $XY["YM"]);
				break;
		}

		$Image = @imagecreate($W, $H);

		$COL = ajax_date::GetCol($BGH);
		$BG = imagecolorallocate($Image, $COL[0], $COL[1], $COL[2]);
		$COL = ajax_date::GetCol($FGH);
		$FG = imagecolorallocate($Image, $COL[0], $COL[1], $COL[2]);

		imagefill($Image, 1, 1, $BG);
		if (sizeof($Points1))
			imagefilledpolygon($Image, $Points1, (sizeof($Points1) / 2), $FG);
		if (sizeof($Points2))
			imagefilledpolygon($Image, $Points2, (sizeof($Points2) / 2), $FG);

		Header("Content-type: image/png");
		imagepng($Image);
		imagedestroy($Image);
		exit();
	}

	function GetCol($HEX)
	{
		$HEX = str_replace(array(" ", "#"), "", $HEX);
		$COL = array();
		$COL[] = hexdec(substr($HEX, 0, 2));
		$COL[] = hexdec(substr($HEX, 2, 2));
		$COL[] = hexdec(substr($HEX, 4, 2));
		return($COL);
	}


}

?>
