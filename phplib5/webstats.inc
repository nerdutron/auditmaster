<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

require_once(PHPLIB_DIR."table.inc");
require_once(PHPLIB_DIR."browser.inc");
require_once(PHPLIB_DIR."database.inc");

// TO DO
// purchasing
// top screen res

class ip
{
	function lookup($IP)
	{
		$IP = explode(".", $IP);
		$Query  = "";
		$Query .= "SELECT country_code, country_name ";
		$Query .= "FROM ip_list il, ip_countries ic ";
		$Query .= "WHERE ip <= (16777216*".$IP[0]." + 65536*".$IP[1]." + 256*".$IP[2]." + ".$IP[3].") ";
		$Query .= "AND il.country_id = ic.country_id ";
		$Query .= "ORDER BY ip DESC LIMIT 1";
		$Data = DBRow(WEB_DB, $Query);
		return(array($Data["country_code"]=>$Data["country_name"]));
	}
}

// search_engine - class to determine the components of a URL and return the search terms
class search_engine
{
	// engines - function that returns a compiled list of search engines.
	function engines()
	{
		$Engines = array();
		$Engines[] = array("host"=>".google.", 			"qryvar"=>"q", 			"label"=>"Google");
		$Engines[] = array("host"=>"search.msn.", 		"qryvar"=>"q", 			"label"=>"MSN");
		$Engines[] = array("host"=>"search.ninemsn.", 	"qryvar"=>"q", 			"label"=>"NineMSN");
		$Engines[] = array("host"=>".yahoo.", 			"qryvar"=>"p", 			"label"=>"Yahoo");
		$Engines[] = array("host"=>".yahoo.", 			"qryvar"=>"va", 		"label"=>"Yahoo");
		$Engines[] = array("host"=>"search.aol.", 		"qryvar"=>"q", 			"label"=>"AOL");
		$Engines[] = array("host"=>".altavista.", 		"qryvar"=>"q", 			"label"=>"Altavista");
		$Engines[] = array("host"=>".hotbot.", 			"qryvar"=>"query", 		"label"=>"HotBot");
		$Engines[] = array("host"=>".lycos.", 			"qryvar"=>"query", 		"label"=>"Lycos");
		$Engines[] = array("host"=>".sensis.", 			"qryvar"=>"find", 		"label"=>"Sensis");
		$Engines[] = array("host"=>".alltheweb.",		"qryvar"=>"q",			"label"=>"All The Web");
		$Engines[] = array("host"=>".looksmart.",		"qryvar"=>"key",		"label"=>"LookSmart");
		$Engines[] = array("host"=>".ask.",				"qryvar"=>"q",			"label"=>"AskJeeves");
		$Engines[] = array("host"=>".search.com",		"qryvar"=>"q",			"label"=>"search.com");
		$Engines[] = array("host"=>".go.com",			"qryvar"=>"search",		"label"=>"go.com");
		$Engines[] = array("host"=>".websearch.",		"qryvar"=>"qkw",		"label"=>"Web Search");
		$Engines[] = array("host"=>".overture.",		"qryvar"=>"Keywords",	"label"=>"Overture");
//'Excite|excite|/web/',
//'Webcrawler|webcrawler|/web/',
		return ($Engines);
	}

	function terms($URL)
	{
		// get list of engines..
		$Engines = search_engine::engines();
		
		// parse the url into its components
		$URLComponents = parse_url($URL);

		// iterate through each of the engines and determine if the host given matches..
		foreach ($Engines as $Engine)
		{
			// if the host name belongs to a search engine
			if (strstr($URLComponents["host"], $Engine["host"]))
			{
				// determine the parameters..
				$QueryParts = explode("&", $URLComponents["query"]);
				$SearchTerm = "";
				
				// get the search terms 
				foreach ($QueryParts as $Part)
				{
					// explode on the = sign..
					$QP = explode("=", $Part);

					if ($QP[0] == $Engine["qryvar"])
					{
						// set the search term..
						$SearchTerm = urldecode($QP[1]);
						
						// break from two levels..
						break;
					}
				}

				// set the search details..
				$SearchDetails = array("SearchURL"=>$URL, "SearchType"=>$Engine["label"], "SearchTerm"=>$SearchTerm);
				return ($SearchDetails);
			}
		}

		return (0);
	}
}

class webstats
{
	function __construct($DB, $Excludes = "")
	{
		$this->DB = $DB;
		$this->StatsStart = array_sum(explode(" ", microtime()));
		if (!is_array($Excludes))
			$Excludes = array();
		$this->StatsExcludes = $Excludes;
	}

	function record($Action = "")
	{
		if (!$Action && defined("CMS_LOGIN"))
			$Action = CURRENT_ACTION;

		$Classes = get_declared_classes();
		foreach ($this->StatsExcludes as $Exclude)
			if (in_array($Exclude, $Classes))
				return;

		// remote country by ip
		$Country = ip::lookup($_SERVER["REMOTE_ADDR"]);

		// referer domain
		$Tmp = explode("//", $_SERVER["HTTP_REFERER"]);
		array_shift($Tmp);
		$Tmp = explode("/", array_shift($Tmp));
		$Referer = array_shift($Tmp);
		if ($Referer == $_SERVER["HTTP_HOST"])
			$Entry = 0;
		else
			$Entry = 1;

		$INQ = new DBInsert;
		$INQ->Save("log_id", 0);
		$INQ->Save("action", $Action);

		$INQ->Save("remote", $_SERVER["REMOTE_ADDR"]);
		$INQ->Save("country", array_pop($Country));
		$INQ->Save("page", $_SERVER["REQUEST_URI"]);
		if ($Entry)
			if (trim($_SERVER["HTTP_REFERER"]))
				$INQ->Save("referer", $_SERVER["HTTP_REFERER"]);
			else
				$INQ->Save("referer", "");

		$B = new Browser;
		$INQ->Save("browser", $B->Browser["Real"]);
		$INQ->Save("version", $B->Version["Real"]);
		$INQ->Save("os", $B->OperatingSystem);

		$INQ->Save("date_month", date("Ym"));
		$INQ->Save("date_day", date("d"));
		$INQ->Save("date_hour", date("H"));
		$INQ->Save("week_day", date("w"));

		$INQ->Save("duration", (array_sum(explode(" ", microtime())) - $this->StatsStart));
		$a = explode(" ", trim(`cat /proc/loadavg`));
		$CurrentLoad = array_shift($a) * 1;
		$INQ->Save("load_avg", $CurrentLoad);

		if ($_SERVER["HTTP_REFERER"])
		{
			$SearchEngine = search_engine::terms($_SERVER["HTTP_REFERER"]);
			if (is_array($SearchEngine))
			{
				$INQ->Save("search_engine", $SearchEngine["SearchType"]);
				$INQ->Save("search_term", $SearchEngine["SearchTerm"]);
			}
		}

		$INQ->Run($this->DB, "access_log", "INSERT");
		$INQ->EndQuery();
	}
}

class showstats
{
	function header()
	{
		if ($_REQUEST["WSG"])
			showstats::graph($_REQUEST["WSG"]);
		if ($_REQUEST["BAR"])
			showstats::bar();
	}

	function stats($MonthDate)
	{
		$Stats = array();

		$Select = "SELECT ";
		$Count = " COUNT(*) as val ";
		$From = " FROM access_log ";
		$Group = " GROUP BY ";
		$Month = " WHERE date_month = ";
		$Visit = " AND referer <> '' ";
		$HTTP = " AND (referer like 'http://%' or referer like 'https://%') ";
		$Limit = " ORDER BY val DESC LIMIT 10 ";
		$SearchOn = "AND search_engine <> '' ";
		$SearchOff = "AND search_engine = '' ";
		$Domain = " SUBSTRING(referer, 1, LOCATE('/', referer, 9)) ";
		$Bookmark = " AND referer <> '' ";
		$Month .= "'".$MonthDate."' ";

		$Stat = array();

		$Stat["MO"]["Title"] = "By Day";
		$Stat["MO"]["Query"] = $Select."date_day as item,".$Count.$From.$Month.$Visit.$Group."date_day";
		$Stat["MO"]["Min"] = 1;
		$Stat["MO"]["Max"] = date("t", CreateTimeStamp($MonthDate."01000000"));

		$Stat["HO"]["Title"] = "By Hour";
		$Stat["HO"]["Query"] = $Select."date_hour as item,".$Count.$From.$Month.$Visit.$Group."date_hour";
		$Stat["HO"]["Min"] = 0;
		$Stat["HO"]["Max"] = 23;

		$Stat["WD"]["Title"] = "By Week Day";
		$Stat["WD"]["Query"] = $Select."week_day as item,".$Count.$From.$Month.$Visit.$Group."week_day";
		$Stat["WD"]["Min"] = 0;
		$Stat["WD"]["Max"] = 6;
		$Stat["WD"]["Map"] = array("Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday");

		$Stat["EN"]["Title"] = "Top Entry Pages";
		$Stat["EN"]["Query"] = $Select."action as item,".$Count.$From.$Month.$Visit.$Group."action";
		$Stat["EN"]["Split"] = "1";

		$Stat["PG"]["Title"] = "Top Pages";
		$Stat["PG"]["Query"] = $Select."action as item,".$Count.$From.$Month.$Group."action";
		$Stat["PG"]["Split"] = "1";

		$Stat["RE"]["Title"] = "Top Referers";
		$Stat["RE"]["Query"] = $Select.$Domain." as item,".$Count.$From.$Month.$SearchOff.$HTTP.$Group.$Domain;
		$Stat["RE"]["Split"] = "1";

		$Stat["SE"]["Title"] = "Top Search Engines";
		$Stat["SE"]["Query"] = $Select."search_engine as item,".$Count.$From.$Month.$SearchOn.$Visit.$Group."search_engine";
		$Stat["SE"]["Split"] = "1";

		$Stat["KW"]["Title"] = "Top Keywords";
		$Stat["KW"]["Query"] = $Select."search_term as item,".$Count.$From.$Month.$SearchOn.$Visit.$Group."search_term";
		$Stat["KW"]["Split"] = "1";

		$Stat["VI"]["Title"] = "Top Visitors";
		$Stat["VI"]["Query"] = $Select."remote as item,".$Count.$From.$Month.$Group."remote";
		$Stat["VI"]["Split"] = "1";

		$Stat["BG"]["Title"] = "Top Browser Groups";
		$Stat["BG"]["Query"] = $Select."browser as item,".$Count.$From.$Month.$Group."browser";
		$Stat["BG"]["Split"] = "1";

		$Stat["BV"]["Title"] = "Top Browser Versions";
		$Stat["BV"]["Query"] = $Select."concat(browser,' ',version) as item,".$Count.$From.$Month.$Group."browser,version";
		$Stat["BV"]["Split"] = "1";

		$Stat["OS"]["Title"] = "Top Operating Systems";
		$Stat["OS"]["Query"] = $Select."os as item,".$Count.$From.$Month.$Group."os";
		$Stat["OS"]["Split"] = "1";

		$Stat["CV"]["Title"] = "By Country (Visits)";
		$Stat["CV"]["Query"] = $Select."country as item,".$Count.$From.$Month.$Visit.$Group."country";
		$Stat["CV"]["Split"] = "1";

		$Stat["CP"]["Title"] = "By Country (Pages)";
		$Stat["CP"]["Query"] = $Select."country as item,".$Count.$From.$Month.$Group."country";
		$Stat["CP"]["Split"] = "1";

		return ($Stat);
	}

	function menu()
	{
		$SS = $_REQUEST["SS"];

		if ($SS["tgt"])
		{
			$Stat = showstats::stats($SS["tgt"]);

			if (!is_array($Stat[$SS["typ"]]))
				$SS["typ"] = "MO";

			$Links = array();
			foreach ($Stat as $Type => $Data)
			{
				$Words = explode(" ", $Data["Title"]);
				$Prefix = array_shift($Words);
				$Links[$Prefix][] = showstats::page_url($SS["tgt"], $Type).implode("<br />", $Words)."</a>";
			}

			$Table = new Table;
			$Table->AddData(showstats::page_url()."Home</a>");
			foreach ($Links as $Prefix => $Reports)
			{
				$Table->AddHead($Prefix.":");
				foreach ($Reports as $Report)
					$Table->AddData($Report);
			}
			$Table->Put();
		}
	}

	function body($DB)
	{
		$SS = $_REQUEST["SS"];

		if ($SS["tgt"])
		{
			$Stat = showstats::stats($SS["tgt"]);

			if (!is_array($Stat[$SS["typ"]]))
				$SS["typ"] = "MO";

			echo "<h2>".date("F Y", CreateTimeStamp($SS["tgt"]."01000000"))."</h2>";
			showstats::results($DB, $Stat[$SS["typ"]]);
		}
		else
		{
			$Select = "SELECT ";
			$Count = " COUNT(*) as val ";
			$Sum = " COUNT(*) as val ";
			$From = " FROM access_log ";
			$Group = " GROUP BY ";
			$Month = " WHERE date_month = ";
			$Visit = " AND referer <> '' ";
			$HTTP = " AND (referer like 'http://%' or referer like 'https://%') ";
			$Limit = " ORDER BY val DESC LIMIT 10 ";
			$SearchOn = "AND search_engine <> '' ";
			$SearchOff = "AND search_engine = '' ";
			$Domain = " SUBSTRING(referer, 1, LOCATE('/', referer, 9)) ";
			$Bookmark = " AND referer <> '' ";

			echo "<h2>".date("F Y", CreateTimeStamp($ThisMonth."01000000"))."</h2>";
			$Table = new Table;
			$Table->AddData("&nbsp;");
			$Table->AddHead("Daily Averages", 2);
			$Table->AddHead("Monthly Totals", 2);
			$Table->EndRow();
			$Table->AddHead("Month");
			$Table->AddHead("Visits");
			$Table->AddHead("Pages");
			$Table->AddHead("Visits");
			$Table->AddHead("Pages");
			$Table->EndRow();
			$Query = "select distinct(date_month) from access_log order by date_month desc";
			$Q = new DBQuery;
			$Q->Run($DB, $Query);
			$i = 0;
			$Max = 0;
			$Data = array();
			while ($Q->Next())
			{
				$i++;
				$ThisMonth = $Q->GetField("date_month");
				$URL = showstats::page_url($ThisMonth);
				$Data[$i]["title"] = $URL.date("F Y", CreateTimeStamp($ThisMonth."01000000"))."</a>";

				// daily avg visits
				$Value = showstats::average($DB, $Select.$Count.$From.$Month.$ThisMonth.$Visit, $ThisMonth);
				$Data[$i]["DV"] = $Value;

				// daily avg hits
				$Value = showstats::average($DB, $Select.$Count.$From.$Month.$ThisMonth, $ThisMonth);
				$Data[$i]["DP"] = $Value;

				// monthly visits
				$Value = DBGet($DB, $Select.$Count.$From.$Month.$ThisMonth.$Visit);
				$Data[$i]["MV"] = $Value;
				if ($Value > $Max)
					$Max = $Value;

				// monthly hits
				$Value = DBGet($DB, $Select.$Count.$From.$Month.$ThisMonth);
				$Data[$i]["MP"] = $Value;
				if ($Value > $Max)
					$Max = $Value;
			}

			foreach($Data as $Row)
			{
				$Table->AddData($Row["title"]);
				$Table->AddData(number_format($Row["DV"], 2));
				$Table->AddData(number_format($Row["DP"], 2));
				$Table->AddData($Row["MV"]);
				$Table->AddData($Row["MP"]);

				$Visits = 100 / $Max * $Row["MV"];
				$Pages = 100 / $Max * $Row["MP"];
				$Table->AddData(showstats::image_url($Visits, $Pages));

				$Table->EndRow();
			}

			$Table->Put();
		}
		echo "<br style=\"clear: both;\" />\n";
	}

	function url($Extra = "")
	{
		$Parts = explode("SS[", $_SERVER['REQUEST_URI']);
		if (sizeof($Parts) == 1)
			$URL = $_SERVER['REQUEST_URI'];
		else
			$URL = $Parts[0];

		if (strstr($URL, "?"))
		{
			switch(substr($URL, strlen($URL) - 1))
			{
				case "?":
				case "&":
					$URL .= $Extra;
					break;
				default:
					$URL .= "&".$Extra;
					break;
			}
		}
		else
			$URL = $URL."?".$Extra;
		return($URL);
	}

	function page_url($Target = "", $Type = "")
	{
		$Extra  = "";
		$Extra .= "SS[tgt]=".$Target;
		if ($Type)
			$Extra .= "&SS[typ]=".$Type;
		return("<a href=\"".showstats::url($Extra)."\">");
	}

	function image_url($V1, $V2 = 0, $Split = 0)
	{
		$Extra  = "";
		$Extra .= "BAR[V1]=".$V1;
		if ($V2)
			$Extra .= "&BAR[V2]=".$V2;
		if ($Split)
			$Extra .= "&BAR[Split]=".$Split;
		return("<img src=\"".showstats::url($Extra)."\" />");
	}

	function average($DB, $Query, $ThisMonth)
	{
		$Days = date("t", CreateTimeStamp($ThisMonth."01000000"));
		if ($ThisMonth == date("Ym"))
			$Days = date("j");
		$Total = DBGet($DB, $Query);
		if ($Days)
			return ($Total / $Days);
		else
			return (0);
	}
	
	function results($DB, $Stat)
	{
		$Title = $Stat["Title"];
		$Query = $Stat["Query"];
		$Min = $Stat["Min"];
		$Max = $Stat["Max"];
		$Map = $Stat["Map"];
		$Split = $Stat["Split"] * 1;
		$Range = (isset($Min) && isset($Max));
		echo "<h3 style=\"clear: both;\">".$Title."</h3>\n";

		$Table = new Table;
		if ($Range)
		{
			if (strstr($Query, "AND referer <> ''"))
				$Double = 1;
			$Table->AddHead("&nbsp;");
			if ($Double)
			{
				$Table->AddHead("Visits", 1 + $Split);
				$Table->AddHead("Pages", 1 + $Split);
			}
			else
				$Table->AddHead("Count", 1 + $Split);
			$Table->EndRow();

			$GraphMax = 0;

			$Ret = DBResult($DB, $Query);
			$Visits = array();
			foreach($Ret as $Data)
			{
				if($Data["val"])
				{
					$Visits[$Data["item"]] = $Data["val"];
					if ($Data["val"] > $GraphMax)
						$GraphMax = $Data["val"];
				}
			}
			$VisitsTotal = array_sum($Visits);

			if ($Double)
			{
				$Ret = DBResult($DB, str_replace("AND referer <> ''", "", $Query));
				$Pages = array();
				foreach($Ret as $Data)
					if($Data["val"])
					{
						$Pages[$Data["item"]] = $Data["val"];
						if ($Data["val"] > $GraphMax)
							$GraphMax = $Data["val"];
					}
				$PagesTotal = array_sum($Visits);
			}

			for ($i=$Min;$i<=$Max;$i++)
			{
				if (is_array($Map))
					$Label = $Map[$i];
				else
					$Label = "".$i;
				$Table->AddHead($Label);
				$Table->AddData($Visits[$i] * 1);
				if ($Split)
					$VisitsPercent = 100 / $VisitsTotal * $Visits[$i];
				else
					$VisitsPercent = 100 / $GraphMax * $Visits[$i];
				if ($Split)
					$Table->AddData(number_format($VisitsPercent, 2)."%");
				if ($Double)
				{
					$Table->AddData($Pages[$i] * 1);
					if ($Split)
						$PagesPercent = 100 / $PagesTotal * $Pages[$i];
					else
						$PagesPercent = 100 / $GraphMax * $Pages[$i];
					if ($Split)
						$Table->AddData(number_format($PagesPercent, 2)."%");
				}
				$Table->AddData(showstats::image_url($VisitsPercent, $PagesPercent, $Split));
				$Table->EndRow();
			}
			$Table->EndRow();
		}
		else
		{
			$Ret = DBResult($DB, $Query);
			$Total = 0;
			foreach ($Ret as $Data)
				$Total = $Total + $Data["val"];
			$Other = $Total;
			$Ret = DBResult($DB, $Query." ORDER BY val DESC LIMIT 50");
			$i = 0;
			foreach ($Ret as $Data)
			{
				if ($Data["item"])
					$Table->AddHead($Data["item"]);
				else
					$Table->AddHead("Unidentified");
				$Table->AddData(number_format($Data["val"])."&nbsp;");
				$Percent = 100 / $Total * $Data["val"];
				$Table->AddData(number_format($Percent, 2)."%");
				$Table->AddData(showstats::image_url($Percent));
				$Table->EndRow();
				$Other = $Other - $Data["val"];
			}
			$Table->AddHead("Other (Combined)");
			$Table->AddData(number_format($Other)."&nbsp;");
			if ($Total)
			{
				$Percent = 100 / $Total * $Other;
				$Table->AddData(number_format($Percent, 2)."%");
				$Table->AddData(showstats::image_url($Percent));
			}
			else
			{
				$Table->AddData(number_format(0, 2)."%");
				$Table->AddData(showstats::image_url(0));
			}
			$Table->EndRow();
			
		}
		echo "<br style=\"clear: both;\" />";
		$Table->Put();
	}

	function bar()
	{
		$BAR = $_REQUEST["BAR"];

		$W = 400;
		$H = 13;

		Header("Content-type: image/png");
		$IMG = @imagecreate($W, $H);

		$ColR = imagecolorallocate($IMG, 225, 075, 075);
		$ColG = imagecolorallocate($IMG, 075, 225, 075);
		$ColB = imagecolorallocate($IMG, 080, 080, 200);
		$ColF = imagecolorallocate($IMG, 200, 200, 255);

		imagefill($IMG, 1, 1, $ColF);
		imagerectangle($IMG, 0, 0, $W - 1, $H - 1, $ColB);
		if ($BAR["Split"])
		{
			$Divider = floor($H / 2);
			if ($BAR["V1"])
			{
				$V1 = round($BAR["V1"] * ($W / 100));
				imageline($IMG, 0, $Divider, $V1, $Divider, $ColB);
				imageline($IMG, $V1, 0, $V1, $Divider, $ColB);
				if ($V1 == $W)
					$V1--;
				if ($V1 - 1 > 0)
					imagefill($IMG, $V1 - 1, 1, $ColG);
			}
			if ($BAR["V2"])
			{
				$V2 = round($BAR["V2"] * ($W / 100));
				imageline($IMG, 0, $Divider, $V2, $Divider, $ColB);
				imageline($IMG, $V2, $Divider, $V2, $H - 1, $ColB);
				if ($V2 == $W)
					$V2--;
				if ($V2 - 1 > 0)
					imagefill($IMG, $V2 - 1, $Divider + 1, $ColR);
			}
		}
		else
		{
			if ($BAR["V2"])
			{
				$V2 = round($BAR["V2"] * ($W / 100));
				imageline($IMG, $V2, 0, $V2, $H - 1, $ColB);
				if ($V2 == $W)
					$V2--;
				if ($V2 - 1 > 0)
					imagefill($IMG, $V2 - 1, 1, $ColR);
			}
			if ($BAR["V1"])
			{
				$V1 = round($BAR["V1"] * ($W / 100));
				imageline($IMG, $V1, 0, $V1, $H - 1, $ColB);
				if ($V1 == $W)
					$V1--;
				if ($V1 - 1 > 0)
					imagefill($IMG, $V1 - 1, $H - 2, $ColG);
			}
		}
		imagepng($IMG);
		imagedestroy($IMG);
		exit;
	}
}

/*
CREATE TABLE access_log (
  log_id int(11) NOT NULL auto_increment,
  action varchar(64) NOT NULL default '',
  remote varchar(255) NOT NULL default '',
  country varchar(16) NOT NULL default '',
  page varchar(255) NOT NULL default '',
  referer varchar(255) NOT NULL default '',
  browser varchar(32) default '',
  version float(10,2) default '0.00',
  os varchar(16) default '',
  date_month int(4) NOT NULL default '0',
  date_day int(4) NOT NULL default '0',
  date_hour int(4) NOT NULL default '0',
  week_day int(4) NOT NULL default '0',
  duration float(10,4) NOT NULL default '0.0000',
  load_avg float(10,4) NOT NULL default '0.0000',
  PRIMARY KEY  (log_id)
) TYPE=MyISAM;
*/

?>
