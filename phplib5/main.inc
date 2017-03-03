<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

require_once(PHPLIB_DIR."tools.inc");

class main
{
	function __construct()
	{
		$TempAction = explode(".", basename($_SERVER["SCRIPT_FILENAME"]));
		$FileAction = array_shift($TempAction);
		if ($FileAction == "index")
			$FileAction = "";
		if (defined("OBJECTS_CHR"))
			$RequestAction = str_replace(OBJECTS_CHR, "/", $_REQUEST["Action"]);
		else
			$RequestAction = str_replace("|", "/", $_REQUEST["Action"]);
		$Action = basename($RequestAction);
		if ($RequestAction != $Action)
		{
			$Module = basename(dirname($RequestAction))."/";
			if (is_dir(OBJECTS_DIR.$Module))
			{
				define("MODULES_DIR", $Module);
				if (defined("OBJECTS_CHR"))
					define("MODULES_NAME", str_replace("/", OBJECTS_CHR, $Module));
				else
					define("MODULES_NAME", str_replace("/", "|", $Module));
			}
		}
		if ($Action == "")
			$Action = $FileAction;
		if ($Action == "")
			$Action = ACTION_DEF;

		if ($Action == "page_cache")
			main::buffer_show($_REQUEST["Page"], $_REQUEST["Owner"]);

		if (defined("MODULES_DIR"))
		{
			$Include = OBJECTS_DIR.MODULES_DIR.$Action.".inc";
			$Error = "Fatal Error: specified object does not exist.";
		}
		else
		{
			$Include = OBJECTS_DIR.$Action.".inc";
			$Error = "Fatal Error: specified object does not exist.";
		}

		if (is_file($Include))
		{
			require_once($Include);
		}
		else
		{
			$Error = "File not found $Include";
			exit($Error);
			//exit($Error.' '.$Include);
		}

		define("CURRENT_ACTION", $Action);
		$Main = new $Action;
		$Main->Start();
	}

	function url($Action, $Parameters = "", $Persistent = "", $PHP = 0, $Lookup = 0)
	{
		if (!is_array($Parameters))
			$Parameters = array();
		if (!is_array($Persistent))
			$Persistent = array();
		$URL  = "";
		$Start = "";
		$Start .= "index.";
		if ($PHP)
			$EXT = "php";
		else
			$EXT = "htm";
		$Start .= $EXT;
		$Start .= "?Action=";
		$Start .= $Action;

		$Values = $Persistent;
		foreach ($Parameters as $Key => $Val)
			$Values[$Key] = $Val;

		if (is_file(dirname($_SERVER["SCRIPT_FILENAME"])."/".$Action.".".$EXT))
		{
			$URL = $Action.".".$EXT;
			$Join = "?";
			foreach ($Values as $Key => $Val)
			{
				$URL .= $Join.$Key."=".urlencode($Val);
				$Join = "&";
			}
		}
		else
		{
			$URL = $Start;
			foreach ($Values as $Key => $Val)
				$URL .= "&".$Key."=".urlencode($Val);
		}
		return ($URL);
	}

	function href($Action, $Content, $Extra = "", $Parameters = "", $Persistent = "", $PHP = 0, $Lookup = 0)
	{
		$HREF  = "";
		$HREF .= "<a href=\"";
		$HREF .= main::url($Action, $Parameters, $Persistent, $PHP, $Lookup);
		$HREF .= "\"";
		if (trim($Extra))
			$HREF .= " ".$Extra;
		$HREF .= ">";
		$HREF .= $Content;
		$HREF .= "</a>";
		return ($HREF);
	}

	function redirect($URL)
	{
		header("Location: ".$URL);
		exit(0);
	}

	function path($Display = 0)
	{
		$Action = ($this->Action ? $this->Action : ACTION_DEF);
		$Avoid = array( 
						"ajax_date",
						"browser",
						"callback",
						"cms_text",
						"cms_faq",
						"cms",
						"cms_html",
						"cms_text_formatted",
						"cms_list_base",
						"cms_news",
						"cms_faq_summary",
						"cms_html_advanced",
						"cms_item_base",
						"dbquery",
						"dberr",
						"dbinsert",
						"datefield",
						"edit",
						"egate",
						"egatetest",
						"excelgen",
						"field",
						"files",
						"form",
						"graph",
						"image",
						"lists",
						"mail",
						"mailq",
						"main",
						"mailmimedecode",
						"dbquery",
						"dbinsert",
						"objects",
						"objectsecurityoff",
						"securepublicobject",
						"objectsecurityon",
						"objectsecurityshow",
						"objectsecurityadmin",
						"cpdf",
						"cezpdf",
						"pdflabel",
						"basepdf",
						"process",
						"safepayinterface",
						"script",
						"stopWatch",
						"table",
						"usagemanage",
						"userdetails",
						"variable",
						"ip",
						"search_engine",
						"webstats",
						"showstats",
						"parsexml",
						"createxml",
						"xml",
						"datastore",
						"zip");

		$Show = 0;
		$Objects = array();
		foreach(get_declared_classes() as $ClassName)
		{
			if ($Show && !in_array(strtolower($ClassName), $Avoid))
				$Objects[] = $ClassName;
			if ($ClassName == "main")
				$Show = 1;
			if ($ClassName == $Action)
				$Show = 0;
		}
		if ($Display)
			echo implode(" &gt; ", $Objects);
		else
			return ($Objects);
	}

	function buffer_on($Mode = "")
	{
		if (defined("PAGE_CACHE_DB"))
		{
			$Mode = strtolower($Mode);
			if (!in_array($Mode, array("send","cache")))
				$Mode = "send";
			$GLOBALS["PAGE_CACHE_MODE"] = $Mode;

			$State = ob_get_status();

			if (ob_get_level() && $State["name"] != "buffer_".$Mode)
				$Buffer = ob_get_clean();
			ob_start("buffer_".$Mode);
			echo $Buffer;
		}
	}

	function buffer_send($Buffer)
	{
		Header("Content-Type: text/html");
		Header("Content-Length: ".strlen($Buffer));
		return($Buffer);
	}

	function buffer_cache($Buffer)
	{
		if ($GLOBALS["PAGE_CACHE_MODE"] != "cache")
			return($Buffer);

		$INQ = new DBInsert;
		if (defined("PAGE_CACHE_KEY"))
			$INQ->Save("secure_key", $_REQUEST[PAGE_CACHE_KEY]);
		$INQ->Save("page", $Buffer);
		$INQ->Save("expiry", date("Y-m-d H:i:s", strtotime("+ 2 hours")));
		$INQ->Run(PAGE_CACHE_DB, "page_cache", "INSERT");
		$ID = $INQ->GetInsertID();
		$INQ->EndQuery();
		$URL = "Location: index.php?Action=page_cache&Page=".base64_encode($ID);
		Header($URL);
		exit;
	}

	function buffer_off()
	{
		$GLOBALS["PAGE_CACHE_MODE"] = "";
		if (ob_get_level())
			echo ob_get_clean();
	}

	function buffer_show($Page)
	{
		if (defined("PAGE_CACHE_DB"))
		{
			$Query = "SELECT page FROM page_cache WHERE cache_id = ".base64_decode($Page);
			if (defined("PAGE_CACHE_KEY"))
				$Query .= " AND secure_key = '".addslashes($_REQUEST[PAGE_CACHE_KEY])."'";
			$Page = DBGet(PAGE_CACHE_DB, $Query);
			Header("Content-Type: text/html");
			Header("Content-Length: ".strlen($Page));
			echo $Page;
			exit;
		}
	}
}

function buffer_send($Buffer)
{
	return(main::buffer_send($Buffer));
}

function buffer_cache($Buffer)
{
	return(main::buffer_cache($Buffer));
}

?>
