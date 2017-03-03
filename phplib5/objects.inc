<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

require_once(PHPLIB_DIR."field.inc");
require_once(PHPLIB_DIR."browser.inc");
require_once(PHPLIB_DIR."database.inc");
require_once(PHPLIB_DIR."variable.inc");
require_once(PHPLIB_DIR."sql.inc");

if (!defined("COOKIE_NAME"))
	define("COOKIE_NAME", "OS_COOKIE");

class Objects
{
	function __construct($DB = "", $Table1 = "", $Table2 = "", $Table3 = "")
	{
		if (strstr($_SERVER["SCRIPT_NAME"], "/dev/"))
			$this->AllowAdmin = 1;
		else
			$this->AllowAdmin = 0;

		$this->Security = array();
		if ($Table1 == "")
			$Table1 = array("security_access", "object_code", "status");
		if ($Table2 == "")
			$Table2 = array("security_permissions", "object_code", "status", "identity");
		if ($Table3 == "")
			$Table3 = array("security_purpose", "object_code", "status");
		if ($DB)
		{
			$this->DB = $DB;
			$this->T1 = $Table1;
			$this->T2 = $Table2;
			$this->T3 = $Table3;
			$Q = new DBQuery;
			$Q->Run($this->DB, "select * from ".$this->T1[0]);
			while ($Q->Next())
				$this->Security[$Q->GetField($this->T1[1])] = $Q->GetField($this->T1[2]);
			$Q->EndQuery();
			$this->Purpose = array();
			$Q = new DBQuery;
			$Q->Run($this->DB, "select * from ".$this->T3[0]);
			while ($Q->Next())
				$this->Purpose[$Q->GetField($this->T3[1])] = $Q->GetField($this->T3[2]);
			$Q->EndQuery();
		}
		$this->Modes = array();
		$this->Modes["none"] = "No Security on Object";
		$this->Modes["wipe"] = "Permission is required to Delete";
		$this->Modes["edit"] = "Permission is required to Edit/Save or Delete";
		$this->Modes["view"] = "Permission is required to View, Edit/Save and Delete";
		$this->ModesFull = $this->Modes;
		$this->ModesFull["none"] .= " (all users can perform all actions)";
		$this->ModesFull["wipe"] .= " (View and Edit/Save granted)";
		$this->ModesFull["edit"] .= " (View access granted)";
		$this->Purposes = array();
		$this->Purposes["view"] = "Object is used for Viewing Only";
		$this->Purposes["edit"] = "Object is used for Editing/Creating";
		$this->Purposes["wipe"] = "Object is used for Deleting";
		$this->PurposesFull = array();
		$this->PurposesFull[""] = "V";
		$this->PurposesFull["edit"] = "E";
		$this->PurposesFull["wipe"] = "D";
		$this->Cell = array();
		$this->Cell["t"][1][1] = "tdv_";
		$this->Cell["t"][1][2] = "tdh_";
		$this->Cell["t"][2][1] = "tdv_";
		$this->Cell["t"][2][2] = "td_";
		$this->Cell["l"][1][1] = "tdv_";
		$this->Cell["l"][1][2] = "tdh_";
		$this->Cell["l"][2][1] = "td_";
		$this->Cell["l"][2][2] = "td_";
		$this->Cell["i"][1][1] = "tdv_";
		$this->Cell["i"][1][2] = "td_";
		$this->Cell["i"][2][1] = "tdv_";
		$this->Cell["i"][2][2] = "td_";
		$this->Cell["w"][1][1] = "td_";
		$this->Cell["w"][1][2] = "td_";
		$this->Cell["w"][2][1] = "td_";
		$this->Cell["w"][2][2] = "td_";
		$this->Logs = array();
		$this->Logs[""] = "Clear Status";
		$this->Logs["assigned"] = "Mark as Assigned";
		$this->Logs["complete"] = "Mark as Complete";

		$this->Max = 20;

		$this->DIR = $GLOBALS["DIR"];
		$this->FUNCTIONS = $GLOBALS["FUNCTIONS"];
		$this->CONTENTS = $GLOBALS["CONTENTS"];
		if ($this->AllowAdmin)
		{
			$Action = $_REQUEST["Action"];
			$this->OLD = $_REQUEST["OLD"];
			$this->NEW = $_REQUEST["NEW"];
			$this->KEY = $_REQUEST["KEY"];
			$this->NOTES = $_REQUEST["NOTES"];
			$this->SELECT = $_REQUEST["SELECT"];
			$this->COMMENTS = $_REQUEST["COMMENTS"];
			$this->CONSTRUCTOR = $_REQUEST["CONSTRUCTOR"];
			$this->VIEW = $_REQUEST["VIEW"];
			$this->HIDE = $_REQUEST["HIDE"];
			$this->MODE = $_REQUEST["MODE"];
			$this->ROOT = $_REQUEST["ROOT"];
		}
		else
			$Action = "";
		$this->URL = $GLOBALS["PHP_SELF"]."?ROOT=".$this->ROOT;
		if (is_file($this->DIR."object.log"))
			$this->LOG = unserialize(file_get_contents($this->DIR."object.log"));

		switch($Action)
		{
			case "Log":
				$this->ObjectLogScreen();
				break;
			case "LogSave":
				$this->ObjectLogSave();
				break;
			case "Pur":
				$this->ObjectPurposeScreen();
				break;
			case "PurSave":
				$this->ObjectPurposeSave();
				break;
			case "Sec":
				$this->ObjectSecurityScreen();
				break;
			case "SecSave":
				$this->ObjectSecuritySave();
				break;
			case "Add":
				$this->ObjectCreateScreen();
				break;
			case "Save":
				$this->CreateObjectFile();
				break;
			default:
				$this->Labels = array();
				$this->AnalyseDirectory($this->DIR);
				break;
		}
	}

	function ReadObjects($Dir)
	{
		$Command = "grep -rs extends ".$Dir."*";
		$Output = `$Command`;
		$Output = explode("\n", trim($Output));
		$ObjectList = array();
		foreach($Output as $Line)
		{
			$Data = explode(":class ", trim($Line));
			if ($Data[1])
			{
				$Path = substr($Data[0], strlen($Dir));
				$Path = explode("/", $Path);
				array_pop($Path);
				$Path = $Path[0];
				$Item = array();
				$Item["path"] = trim($Path);
				$Parts = explode(" // ", $Data[1]);
				$Item["label"] = trim($Parts[1]);
				$Parts = explode(" ", trim($Parts[0]));
				if ($Path)
				{
					if (defined("OBJECTS_CHR"))
					{
						$Item["class"] = $Path.OBJECTS_CHR.strtolower($Parts[0]);
						$Item["extends"] = $Path.OBJECTS_CHR.strtolower($Parts[2]);
					}
					else
					{
						$Item["class"] = $Path."|".strtolower($Parts[0]);
						$Item["extends"] = $Path."|".strtolower($Parts[2]);
					}
				}
				else
				{
					$Item["class"] = strtolower($Parts[0]);
					$Item["extends"] = strtolower($Parts[2]);
				}
				$ObjectList[] = $Item;
			}
		}
		return ($ObjectList);
	}

	function AnalyseDirectory($Dir)
	{
		$this->MakeHeader();

		$Root = array();
		foreach($this->ReadObjects($Dir) as $Item)
		{
			if ($Item["label"])
				$this->Labels[$Item["class"]] = $Item["label"];
			else
				$this->Labels[$Item["class"]] = $Item["class"];

			$Extended = $Item["extends"];
			if (!$Root[$Extended])
				$Root[$Extended] = "yes";
			if (!is_array($$Extended))
				$$Extended = array();

			$Extender = $Item["class"];
			$Root[$Extender] = "no";
			if (!is_array($$Extender))
				$$Extender = array();
			${$Extended}[$Extender] = &$$Extender;
		}

		echo "<table>\n";
		echo "<tr>\n";
		if (!$this->HIDE)
			echo "<td valign=\"top\">\n";

		if (!$this->HIDE)
		{
			if ($this->ROOT)
			{
				if ($defined("OBJECTS_CHR"))
					$Parts = explode(OBJECTS_CHR, $this->ROOT);
				else
					$Parts = explode("|", $this->ROOT);
				$Key = $Parts[0];
				echo "<a href=\"".$GLOBALS["PHP_SELF"]."\"><b>/</b></a>";
				echo "<table cellspacing=0 border=1>\n";
				$Level = "none";
				$Base = array($Key => $$Key);
				$this->Stack = array();
				$this->ShowAnalysisTree($Base, $Parts[1]);
				echo "</table>\n";
			}
			else
			{
				foreach ($Root as $Key => $Val)
				{
					if ($Val == "yes")
					{
						echo "<table cellspacing=0 border=1>\n";
						$Level = "none";
						$Base = array($Key => $$Key);
						$this->Stack = array();
						$this->ShowAnalysisTree($Base, $Level);
						echo "</table>\n";
						echo "<hr />\n";
					}
				}
			}
		}

		if (!$this->HIDE)
			echo "</td>\n";
		if ($this->HIDE)
			echo "<td valign=\"top\">\n";
		else
			echo "<th valign=\"top\">\n";

		if ($this->VIEW)
		{
			echo "<h1>";
			echo "<a href=\"".$this->URL."\">&lt;</a> ";
			echo $this->VIEW;
			if ($this->HIDE)
				echo " <a href=\"".$this->URL."&VIEW=".$this->VIEW."\">&gt;&lt;</a>";
			else
				echo " <a href=\"".$this->URL."&HIDE=1&VIEW=".$this->VIEW."\">&lt;&gt;</a>";
			echo "</h1>\n";
			if (is_file($this->DIR.$this->VIEW.".inc"))
			{
				$Lines = file($this->DIR.$this->VIEW.".inc");
				echo "<pre>\n";
				foreach ($Lines as $Line)
					echo htmlspecialchars($Line);
				echo "</pre>\n";
			}
			else
				echo "<p>Object file is missing</p>";
		}

		if ($this->HIDE)
			echo "</td>\n";
		else
			echo "</th>\n";
		echo "</tr>\n";
		echo "</table>\n";

		$this->MakeFooter();
	}

	function AnalysisStructure($Label, $Level)
	{
		$Space = "&nbsp;&nbsp;&nbsp;&nbsp;";
		echo "<tr>";
		foreach ($this->Stack as $Data)
		{
			echo "<td class=\"".$this->Cell[$Data[0]][1][1].$Data[1]."\">".$Space."</td>";
			echo "<td class=\"".$this->Cell[$Data[0]][1][2].$Data[1]."\">".$Space."</td>";
		}

		echo "<td colspan=\"".($this->Max - sizeof($this->Stack) - 1)."\" rowspan=\"2\">\n";
		echo $Label;
		echo "</td>";

		echo "</tr>\n";

		echo "<tr>";
		foreach ($this->Stack as $Data)
		{
			echo "<td class=\"".$this->Cell[$Data[0]][2][1].$Data[1]."\">".$Space."</td>";
			echo "<td class=\"".$this->Cell[$Data[0]][2][2].$Data[1]."\">".$Space."</td>";
		}
		echo "</tr>\n";
	}

	function ShowAnalysisTree($ArrayName, $Level, $Remain = 0)
	{
		$c = 0;
		foreach ($ArrayName as $Key => $Val)
		{
			$c++;

			$UseLevel = $this->SecurityLevel($Key, $Level);
			if ($this->AllowAdmin)
				$HREF = " href=\"javascript:Popup('".$this->URL."&Action=Log&KEY=".$Key."')\"";
			else
				$HREF = "";
			switch($this->LOG[$Key]["Status"])
			{
				case "complete":
					$Label = "<a name=\"".$Key."\" ".$HREF."class=\"stat_done\">&nbsp;</a>";
					break;
				case "assigned":
					$Label = "<a name=\"".$Key."\" ".$HREF."class=\"stat_work\">&nbsp;</a>";
					break;
				default:
					$Label = "<a name=\"".$Key."\" ".$HREF."class=\"stat_none\">&nbsp;</a>";
					break;
			}

			if ($this->PurposesFull[$this->Purpose[$Key]])
			{
				$Purpose = $this->Purpose[$Key];
				if ($this->DB && $this->AllowAdmin)
					$Label .= "<a href=\"javascript:Popup('".$this->URL."&Action=Pur&KEY=".$Key."')\">";
				$Label .= "<span class=\"purpose_".$Purpose."\">";
				$Label .= $this->PurposesFull[$Purpose]."&nbsp;";
				$Label .= "</span>";
				if ($this->DB && $this->AllowAdmin)
					$Label .= "</a>";
				$Label .= "";
			}

			if ($this->DB && $this->AllowAdmin)
				$Label .= "<a href=\"javascript:Popup('".$this->URL."&Action=Sec&KEY=".$Key."')\">";
			$Label .= $this->ShowObject($Key);
			if ($this->DB && $this->AllowAdmin)
				$Label .= "</a>";

			if ($this->AllowAdmin)
			{
				$Label .= "&nbsp;";
				$Label .= "&nbsp;";
				$Label .= "&nbsp;";
				$Label .= "<span class=\"noprint\">";
				$Label .= "<b>[</b>";
				if (defined("OBJECTS_CHR"))
					$Label .= "&nbsp;<a href=\"".$GLOBALS["PHP_SELF"]."?ROOT=".$Key.OBJECTS_CHR.$Level."\">/</a>";
				else
					$Label .= "&nbsp;<a href=\"".$GLOBALS["PHP_SELF"]."?ROOT=".$Key."|".$Level."\">/</a>";
				$Label .= "&nbsp;<a href=\"".$this->URL."&Action=Add&OLD=".$Key."\">+</a>";
				if ($Key == $this->VIEW)
					$Label .= "&nbsp;<a href=\"".$this->URL."\">&lt;</a>";
				else
					$Label .= "&nbsp;<a href=\"".$this->URL."&VIEW=".$Key."\">&gt;</a>";
				$Label .= "&nbsp;<b>]</b>";
				$Label .= "</span>";
				if ($this->Labels[$Key] != $Key)
					$Label .= "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class=\"object_note\">".$this->Labels[$Key]."</span>";
			}

			if ($Remain)
				array_push($this->Stack, array("t", $Level));
			else
				array_push($this->Stack, array("l", $Level));
			$this->AnalysisStructure($Label, $UseLevel);
			array_pop($this->Stack);
			if ($Remain)
				array_push($this->Stack, array("i", $Level));
			else
				array_push($this->Stack, array("w", $Level));

			$Keys = array_keys($Val);
			if (sizeof($Keys) == 1)
				$More = 0;
			else
				$More = sizeof($Val);

			$this->ShowAnalysisTree($Val, $UseLevel, $More);
			array_pop($this->Stack);
			if ($c == (sizeof($ArrayName) - 1))
				$Remain = 0;
		}
	}

	function ObjectLogSave()
	{
		$this->LOG[$this->KEY]["Status"] = $this->MODE;
		$fp = fopen($this->DIR."object.log", "w");
		fwrite($fp, serialize($this->LOG));
		fclose($fp);

		echo "<script>\n";
		echo "opener.document.location.reload();\n";
		echo "self.close();\n";
		echo "</script>\n";
	}

	function ObjectLogScreen()
	{
		$this->MakeHeader();
		echo "<h1>Object Name:</h1>";
		echo "<h2>".$this->KEY."</h2>";
		$Level = $this->Security[$this->KEY];
		echo "<div class=\"\">".$this->Logs[$this->LOGS[$this->KEY]["Status"]]."</div>\n";
		echo "<div>&nbsp;</div>\n";
		$URL = "<a href=\"".$this->URL."&Action=LogSave&KEY=".$this->KEY."&MODE=[XXXX]\">Set</A>";
		foreach ($this->Logs as $Key => $Val)
			echo "<div class=\"security_".$Key."\">[ ".str_replace("[XXXX]", $Key, $URL)." ] ".$Val."</div>\n";
		$this->MakeFooter(1);
	}

	function ObjectSecuritySave()
	{
		$Level = $this->Security[$this->KEY];
		$Query = "delete from ".$this->T1[0]." where ".$this->T1[1]." = '".$this->KEY."'";
		DB_DoQuery($this->DB, $Query);
		if (in_array($Level, array("wipe","edit","view")))
		{
			$Query = "delete from ".$this->T2[0]." where ".$this->T2[1]." = '".$this->KEY."'";
			DB_DoQuery($this->DB, $Query);
		}
		if ($this->MODE != "none")
		{
			$INQ = new DBInsert;
			$INQ->Save($this->T1[1], $this->KEY);
			$INQ->Save($this->T1[2], $this->MODE);
			$INQ->Run($this->DB, $this->T1[0], "INSERT");
			$INQ->EndQuery();
		}
		echo "<script>\n";
		echo "opener.document.location.reload();\n";
		echo "self.close();\n";
		echo "</script>\n";
	}

	function ObjectPurposeSave()
	{
		$Level = $this->Security[$this->KEY];
		$Query = "delete from ".$this->T3[0]." where ".$this->T3[1]." = '".$this->KEY."'";
		DB_DoQuery($this->DB, $Query);
		if (in_array($Level, array("wipe","edit","view")))
		{
			$Query = "delete from ".$this->T2[0]." where ".$this->T2[1]." = '".$this->KEY."'";
			DB_DoQuery($this->DB, $Query);
		}
		if (in_array($this->MODE, array("wipe", "edit")))
		{
			$INQ = new DBInsert;
			$INQ->Save($this->T3[1], $this->KEY);
			$INQ->Save($this->T3[2], $this->MODE);
			$INQ->Run($this->DB, $this->T3[0], "INSERT");
			$INQ->EndQuery();
		}
		echo "<script>\n";
		echo "opener.document.location.reload();\n";
		echo "self.close();\n";
		echo "</script>\n";
	}

	function ObjectPurposeScreen()
	{
		$this->MakeHeader();
		echo "<h1>Object Purpose:</h1>";
		echo "<h2>".$this->KEY."</h2>";
		$Level = $this->Purpose[$this->KEY];
		if ($Level == "")
			$Level = "view";
		echo "<div class=\"purpose_".$Level."\">".$this->Purposes[$Level]."</div>\n";
		echo "<div>&nbsp;</div>\n";
		$URL = "<a href=\"".$this->URL."&Action=PurSave&KEY=".$this->KEY."&MODE=[XXXX]\">Set</A>";
		foreach ($this->Purposes as $Key => $Val)
			if ($Key != $Level)
				echo "<div class=\"purpose_".$Key."\">[ ".str_replace("[XXXX]", $Key, $URL)." ] ".$Val."</div>\n";
		echo "<p>";
		echo "<b>Note:</b> ";
		echo "Pages based on the Edit classes will need to be left as a &quot;View&quot; object, with full security implemented using an Edit or Delete security check to determine which Edit class flags should be set.";
		echo "</p>";
		$this->MakeFooter(1);
	}

	function ObjectSecurityScreen()
	{
		$this->MakeHeader();
		echo "<h1>Object Security:</h1>";
		echo "<h2>".$this->KEY."</h2>";
		$Level = $this->Security[$this->KEY];
		echo "<div class=\"security_".$Level."\">".$this->Modes[$Level]."</div>\n";
		echo "<div>&nbsp;</div>\n";
		$URL = "<a href=\"".$this->URL."&Action=SecSave&KEY=".$this->KEY."&MODE=[XXXX]\">Set</A>";
		foreach ($this->Modes as $Key => $Val)
			if ($Key != $Level)
				echo "<div class=\"security_".$Key."\">[ ".str_replace("[XXXX]", $Key, $URL)." ] ".$Val."</div>\n";
		$this->MakeFooter();
	}

	function ObjectCreateScreen()
	{
		$this->MakeHeader();
		echo "<form action=\"".$this->URL."&Action=Save&OLD=".$this->OLD."\" method=\"post\">\n";
		echo "<h1>Object Name</h1>";
		echo Field::GetText("NEW", "", 50);
		echo Field::GetFormSubmit("Create");
		echo "<h2>Notes</h2>";
		echo Field::GetArea("NOTES", "", 35, 5);
		echo "<h2>Constructor</h2>";
		echo Field::GetArea("CONSTRUCTOR", "", 35, 5);
		foreach ($this->FUNCTIONS as $Function)
		{
			echo "<h3>";
			echo $Function;
			echo Field::GetCheck("SELECT[".$Function."]", "", 1);
			echo "</h3>";
			echo Field::GetArea("COMMENTS[".$Function."]", "", 35, 5);
		}
		echo "</form>\n";
		$this->MakeFooter();
	}

	function CreateObjectFile()
	{
		$Object = "";

		$Object .= "<?php\n\n";
		if ($this->NOTES)
			$Object .= "/*\n".$this->NOTES."\n*/\n";

		$Object .= "require_once(OBJECTS_DIR.\"".$this->OLD.".inc\");\n\n";

		$Object .= "class ".$this->NEW." extends ".$this->OLD."\n";
		$Object .= "{\n";
		if (phpversion() >= 5)
			$Object .= "	function __construct()\n";
		else
			$Object .= "	function ".$this->NEW."()\n";
		$Object .= "	{\n";
		if (phpversion() >= 5)
			$Object .= "		".$this->OLD."::__construct();\n";
		else
			$Object .= "		".$this->OLD."::".$this->OLD."();\n";
		if ($this->CONSTRUCTOR)
			$Object .= "/*\n".$this->CONSTRUCTOR."\n*/\n";
		$Object .= "	}\n\n";

		foreach($this->COMMENTS as $Function => $Comment)
		{
			if ($Comment || $this->SELECT[$Function])
			{
				$Object .= "	function ".$Function."()\n";
				$Object .= "	{\n";
				if (is_array($this->CONTENTS[$Function]))
					foreach ($this->CONTENTS[$Function] as $Code)
						$Object .= "		echo \"".$Code."\";\n";
				if (trim($Comment))
					$Object .= "/*\n".$Comment."\n*/\n";
				$Object .= "	}\n\n";
			}
		}

		$Object .= "}\n\n";

		$Object .= "?>";

		$File = $this->DIR.$this->NEW.".inc";
		$fp = fopen($File, "w");
		fwrite($fp, $Object);
		fclose($fp);
		chmod($File, 0755);
		Header("Location: ".$this->URL."#".$this->OLD);
	}

	function SecurityLevel($Key, $Current = "none")
	{
		$Level = $this->Security[$Key];
		if (!in_array($Level, array("wipe", "edit", "view")))
			$Level = "none";

		if ($Current == "view")
			$Level = "view";
		elseif ($Current == "edit" && $Level != "view")
			$Level = "edit";
		elseif ($Current == "wipe" && $Level == "none")
			$Level = "wipe";

		return ($Level);
	}

	function ShowObject($Key)
	{
		$String  = "";
		$String .= "<span class=\"security_".$this->SecurityLevel($Key)."\">";
		switch($this->LOG[$Key]["Status"])
		{
			case "complete":
				$String .= $Key;
				break;
			case "assigned":
				$String .= "<i>".$Key."</i>";
				break;
			default:
				$String .= "<b>".$Key."</b>";
				break;
		}
		$String .= "</span>";
		return ($String);
	}

	function MakeHeader()
	{
		$NONE = "#222222";
		$WIPE = "#009900";
		$EDIT = "#FF9900";
		$VIEW = "#990000";

		echo "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" >\n";
		echo "<head>\n";
		echo "<title>Object Tree</title>\n";
		echo "<style type='text/css'>\n";
		echo "BODY\n";
		echo "{\n";
		echo "	margin-top: 0px;\n";
		echo "	margin-left: 0px;\n";
		echo "	margin-right: 0px;\n";
		echo "	font-size: 0.8em;\n";
		echo "}\n";

		echo "*\n";
		echo "{\n";
		echo "	font-family: sans-serif;\n";
		echo "}\n";
		echo "PRE\n";
		echo "{\n";
		echo "	font-family: monospace;\n";
		echo "}\n";

		echo "TABLE, TH, TD\n";
		echo "{\n";
		echo "	border: none;\n";
		echo "	padding: 0px;\n";
		echo "	margin: 0px;\n";
		echo "}\n";

		echo "TH, TD\n";
		echo "{\n";
		echo "	text-align: left;\n";
		echo "	vertical-align: top;\n";
		echo "	background-color: #FFFFFF;\n";
		echo "}\n";

		echo "TH\n";
		echo "{\n";
		echo "	font-size: 0.9em;\n";
		echo "	font-weight: normal;\n";
		echo "	background-color: #DDDDDD;\n";
		echo "}\n";

		echo "TD\n";
		echo "{\n";
		echo "	font-size: 0.8em;\n";
		echo "}\n";

		echo "TD A:hover, TH A:hover \n";
		echo "{\n";
		echo "	color: #FF0000;\n";
		echo "}\n";

		echo ".tdv_none, .tdh_none, .td_none, ";
		echo ".tdv_wipe, .tdh_wipe, .td_wipe, ";
		echo ".tdv_view, .tdh_view, .td_view, ";
		echo ".tdv_edit, .tdh_edit, .td_edit\n";
		echo "{\n";
		echo "	font-size: 0.35em;\n";
		echo "}\n";

		echo ".tdv_none\n";
		echo "{\n";
		echo "	border-right: solid ".$NONE." 2px;\n";
		echo "}\n";
		echo ".tdv_wipe\n";
		echo "{\n";
		echo "	border-right: solid ".$WIPE." 2px;\n";
		echo "}\n";
		echo ".tdv_view\n";
		echo "{\n";
		echo "	border-right: solid ".$VIEW." 2px;\n";
		echo "}\n";
		echo ".tdv_edit\n";
		echo "{\n";
		echo "	border-right: solid ".$EDIT." 2px;\n";
		echo "}\n";
		echo ".tdh_none\n";
		echo "{\n";
		echo "	border-bottom: solid ".$NONE." 2px;\n";
		echo "}\n";
		echo ".tdh_wipe\n";
		echo "{\n";
		echo "	border-bottom: solid ".$WIPE." 2px;\n";
		echo "}\n";
		echo ".tdh_view\n";
		echo "{\n";
		echo "	border-bottom: solid ".$VIEW." 2px;\n";
		echo "}\n";
		echo ".tdh_edit\n";
		echo "{\n";
		echo "	border-bottom: solid ".$EDIT." 2px;\n";
		echo "}\n";
		echo ".td_edit, .td_save, .td_wipe, .td_none\n";
		echo "{\n";
		echo "}\n";

		echo ".stat_none, .stat_work, .stat_done\n";
		echo "{\n";
		echo "	font-size: 0.5em;\n";
		echo "	vertical-align: middle;\n";
		echo "}\n";
		echo ".stat_none\n";
		echo "{\n";
		echo "	border-left: solid ".$VIEW." 8px;\n";
		echo "}\n";
		echo ".stat_work\n";
		echo "{\n";
		echo "	border-left: solid ".$EDIT." 8px;\n";
		echo "}\n";
		echo ".stat_done\n";
		echo "{\n";
		echo "	border-left: solid ".$WIPE." 8px;\n";
		echo "}\n";

		echo "INPUT, SELECT, TEXTAREA\n";
		echo "{\n";
		echo "	color: #000099;\n";
		echo "	border: 1px solid #000099;\n";
		echo "	background-color: #FFFFAA;\n";
		echo "}\n";
		echo "INPUT:focus\n";
		echo "{\n";
		echo "	background-color: #DDDDDD;\n";
		echo "}\n";
		echo "INPUT[type|=\"submit\"]\n";
		echo "{\n";
		echo "	color: #FFFFAA;\n";
		echo "	background-color: #000099;\n";
		echo "	border: 1px solid #000099;\n";
		echo "	font-weight: bold;\n";
		echo "}\n";

		echo "A\n";
		echo "{\n";
		echo "	text-decoration: none;\n";
		echo "}\n";

		echo "A:link, A:visited \n";
		echo "{\n";
		echo "	color: #000099;\n";
		echo "}\n";

		echo ".security_none \n";
		echo "{\n";
		echo "	color: ".$NONE.";\n";
		echo "}\n";

		echo ".security_wipe, .purpose_, .purpose_view \n";
		echo "{\n";
		echo "	color: ".$WIPE.";\n";
		echo "}\n";

		echo ".security_view, .purpose_wipe \n";
		echo "{\n";
		echo "	color: ".$VIEW.";\n";
		echo "}\n";

		echo ".security_edit, .purpose_edit \n";
		echo "{\n";
		echo "	color: ".$EDIT.";\n";
		echo "}\n";

		echo ".object_note \n";
		echo "{\n";
		echo "	color: #FF0000;\n";
		echo "}\n";

		echo "@media print\n";
		echo "{\n";
		echo "	.noprint\n";
		echo "	{\n";
		echo "		display: none;\n";
		echo "	}\n";
		echo "	.stat_none, .stat_work, .stat_done\n";
		echo "	{\n";
		echo "		border: none;\n";
		echo "		margin-right: 25px;\n";
		echo "		font-size: 0.75em;\n";
		echo "		vertical-align: middle;\n";
		echo "	}\n";
		echo "	.stat_none:after\n";
		echo "	{\n";
		echo "		content: \"Unallocated\";\n";
		echo "	}\n";
		echo "	.stat_work:after\n";
		echo "	{\n";
		echo "		content: \"Development\";\n";
		echo "	}\n";
		echo "	.stat_done:after\n";
		echo "	{\n";
		echo "		content: \"Completed\";\n";
		echo "	}\n";
		echo "}\n";

		echo "</style>\n";

		echo "<script>\n";
		echo "function Popup(EditProg)\n";
		echo "{\n";
		echo "	Rand = Math.round(Math.random()*1000);\n";
		echo "	EditProg = EditProg + '&R=' + Rand;\n";
		echo "	window.open(EditProg, 'Popup', 'toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=550,height=350');\n";
		echo "}\n";
		echo "</script>\n";

		echo "</head>\n";
		echo "<body id=\"page_body\">\n";
	}

	function MakeFooter($Hide = 0)
	{
		if ($this->DB && !$this->HIDE && !$Hide)
		{
			echo "<h3>Legend:</h3>";
			echo "<p>";
			echo "<span class=\"security_none\">Black:</span>\n";
			echo $this->ModesFull["none"];
			echo "<br />";
			echo "<span class=\"security_wipe\">Green:</span>\n";
			echo $this->ModesFull["wipe"];
			echo "<br />";
			echo "<span class=\"security_edit\">Amber:</span>\n";
			echo $this->ModesFull["edit"];
			echo "<br />";
			echo "<span class=\"security_view\">Red:</span>\n";
			echo $this->ModesFull["view"];
			echo "</p>";
		}
		echo "</body>\n";
		echo "</html>\n";
	}
}

class ObjectSecurityOff extends Variable
{
	function __construct()
	{
		parent::__construct();
	}
}

class SecurePublicObject extends Variable
{
	function __construct($DB, $Action, $RootObject = "protected", $Timeout = "", $Type = "", $Mail = "", $Table = "", $AlternatePage = "")
	{
		if (defined("COOKIE_ENCRYPT"))
			$this->Encrypt = COOKIE_ENCRYPT;
		else
			$this->Encrypt = 0;

		$this->CookiePath = dirname($_SERVER["REQUEST_URI"]);
		if ($this->CookiePath != "/")
			$this->CookiePath .= "/"; 
		$Parts = explode(":", $_SERVER["HTTP_HOST"]);
		$this->CookieHost = array_shift($Parts);

		$Type = strtoupper($Type);
		if (in_array($Type, array("SOFT", "HARD")))
			$this->Type = $Type;
		else
			$this->Type = "";
		$this->Expires = time() + 60 * 60 * 24 * 30;
		if ($Timeout)
			$this->Expires = strtotime($Timeout);
		if ($Timeout === 0 || $Timeout === "0")
			$this->Expires = 0;

		parent::__construct();
		$this->MakeL("OS_KEY", "OS_DEST", "OS_FORM", "OS_LOGIN", "OS_PASSWORD", "OS_EMAIL", "OS_COO");
		$this->Action = $Action;
		$this->OS["DB"] = $DB;
		$this->OS["Root"] = $RootObject;
		$this->OS["ChangeMessage"] = array();
		if ($Table == "")
			$Table = array("users", "user_id", "active", "os_key", "name", "login", "password", "email");
			//              0                  1          2         3         4       5        6           7
		$this->OS["TB"] = $Table;

		if (($this->OS_KEY == "") && $_REQUEST[COOKIE_NAME])
		{
			$this->OS_KEY = $_REQUEST[COOKIE_NAME];
			// resetting a cookie
			if ($Type != "HARD")
			{
				$this->SetCookies($this->OS_KEY, $this->Expires, $this->CookiePath, $this->CookieHost);
			}
		}

		switch ($this->OS_FORM)
		{
			case "Lock":
				$Query  = "select ".$this->OS["TB"][1];
				$Query .= " from ".$this->OS["TB"][0];
				$Query .= " where ".$this->OS["TB"][3]." = '".sql::text($this->OS_KEY)."'";
				$UID = DBGet($this->OS["DB"], $Query);
				if ($UID)
				{
					$KEY = $this->CreateOSKey($this->OS["DB"],
								$this->OS["TB"][0],
								$this->OS["TB"][1],
								$this->OS["TB"][3]);
					$INQ = new DBInsert;
					$INQ->SaveWhere($this->OS["TB"][1], $UID);
					$INQ->Save($this->OS["TB"][3], $KEY);
					$INQ->Run($this->OS["DB"], $this->OS["TB"][0], "UPDATE");
					$INQ->EndQuery();
				}
				$this->UnsetCookies();
				break;
			case "Transfer":
				if ($this->OS_KEY && $this->OS_KEY == $this->OS_COO)
				{
					$Query  = "select ".$this->OS["TB"][1];
					$Query .= " from ".$this->OS["TB"][0];
					$Query .= " where ".$this->OS["TB"][2]." ";
					$Query .= " and ".$this->OS["TB"][3]." = '".sql::text($this->OS_KEY)."'";
					$UID = DBGet($this->OS["DB"], $Query);
					if ($UID)
					{
						$this->SetCookies($this->OS_KEY, $this->Expires, $this->CookiePath, $this->CookieHost);
						$this->GoToDest($this->OS_KEY);
						exit;
					}
				}
				break;
			case "Login":
				$UID = $this->Password2ID($this->OS_LOGIN, $this->OS_PASSWORD);
				if ($UID)
				{
					// initial set of a cookie
					if (defined("COOKIE_SIMULTANEOUS"))
						if (COOKIE_SIMULTANEOUS)
						{
							$Query  = "";
							$Query .= "SELECT ".$this->OS["TB"][3]." ";
							$Query .= "FROM ".$this->OS["TB"][0]." ";
							$Query .= "WHERE ".$this->OS["TB"][1]." = ".$UID." ";
							$KEY = DBGet($this->OS["DB"], $Query);
						}

					if (!trim($KEY))
					{
						$KEY = $this->CreateOSKey($this->OS["DB"], $this->OS["TB"][0], $this->OS["TB"][1], $this->OS["TB"][3]);
						$INQ = new DBInsert;
						$INQ->SaveWhere($this->OS["TB"][1], $UID);
						$INQ->Save($this->OS["TB"][3], $KEY);
						$INQ->Run($this->OS["DB"], $this->OS["TB"][0], "UPDATE");
						$INQ->EndQuery();
					}

					$this->SetCookies($KEY, $this->Expires, $this->CookiePath, $this->CookieHost);
					$this->GoToDest($KEY);
					exit;
				}
				break;
			case "Retrieve":
				$Query  = "select ".$this->OS["TB"][4].", ".$this->OS["TB"][5].", ".$this->OS["TB"][6];
				$Query .= " from ".$this->OS["TB"][0];
				$Query .= " where ".$this->OS["TB"][7]." = '".sql::text($this->OS_EMAIL)."'";
				$Query .= " and ".$this->OS["TB"][7]." <> ''";
				$Data = DBRow($this->OS["DB"], $Query);
				$Name = array_shift($Data);
				$Login = array_shift($Data);
				$Password = array_shift($Data);
				if ($Login)
				{
					$Message  = "";
					$Message .= "http://".$_SERVER["HTTP_HOST"].$_SERVER["REQUEST_URI"];
					$Message .= "\n\n";
					$Message .= "Login: ";
					$Message .= $Login;
					$Message .= "\n\n";
					$Message .= "Password: ";
					$Message .= $Password;
					if (trim($Mail))
						mail($this->OS_EMAIL, "Site Password", $Message, "From: ".$Mail."\r\n" ."Reply-To: ".$Mail."\r\n");
					else
						mail($this->OS_EMAIL, "Site Password", $Message);
					$this->Mailed = 1;
				}
				if ($this->OS_KEY)
				{
					$this->GoToDest($this->OS_KEY);
					exit;
				}
				break;
			case "Change":
				if ($this->OS_PASSWORD["new"] != $this->OS_PASSWORD["confirm"])
					$this->OS["ChangeMessage"][] = "New passwords do not match.";
				if (strlen($this->OS_PASSWORD["new"]) < 6  || strlen($this->OS_PASSWORD["new"]) > 16)
					$this->OS["ChangeMessage"][] = "Your new password must contain between 6  and 16 characters.";

				$NumberCount = $CharacterCount = 0;
				for($i=0; $i<strlen($this->OS_PASSWORD["new"]); $i++)
				{
					if(is_numeric($this->OS_PASSWORD["new"][$i]))
						$NumberCount++;
					else if(ereg("[^a-zA-Z]", $this->OS_PASSWORD["new"][$i]))
						$BadCharacters = 1;
					else
						$CharacterCount++;
				}
				if ($NumberCount < 2)
					$this->OS["ChangeMessage"][] = "You must use at least 2 numerical characters";
				if ($CharacterCount < 2)
					$this->OS["ChangeMessage"][] = "You must use at least 2 alphabetical characters";
				if ($BadCharacters)
					$this->OS["ChangeMessage"][] = "Your new password has illegal characters";

				$Query  = "select ".$this->OS["TB"][5];
				$Query .= " from ".$this->OS["TB"][0];
				$Query .= " where ".$this->OS["TB"][1]." = ".$this->GetID();
				$Login = DBGet($this->OS["DB"]);
				$UID = $this->Password2ID($Login, $this->OS_PASSWORD["old"]);

				if (!$UID)
					$this->OS["ChangeMessage"][] = "Your old password is incorrect.";

				if (sizeof($this->OS["ChangeMessage"]) == 0)
				{
					$INQ = new DBInsert;
					$INQ->SaveWhere($this->OS["TB"][1], $User);
					if ($this->Encrypt)
						$INQ->Add("FUNCTION", $this->OS["TB"][6], "encrypt('".sql::text($this->OS_PASSWORD["new"])."')");
					else
						$INQ->Save($this->OS["TB"][6], $this->OS_PASSWORD["new"]);
					$INQ->Run($this->OS["DB"], $this->OS["TB"][0], "UPDATE");
					$INQ->EndQuery();
					$this->OS["ChangeMessage"][] = "Password changed successfully.";
				}
				break;
			case "Logout":
				// deleting a cookie
				$this->UnsetCookies();
				Header("Location: ".$_SERVER["PHP_SELF"]."?Action=".$this->OS["Root"]);
				exit;
				break;
		}

		if ($this->OS_KEY)
		{
			$Query  = "select ".$this->OS["TB"][1].", ".$this->OS["TB"][2].", ".$this->OS["TB"][4];
			$Query .= " from ".$this->OS["TB"][0];
			$Query .= " where ".$this->OS["TB"][3]." = '".sql::text($this->OS_KEY)."'";
			$Data = DBRow($this->OS["DB"], $Query);
			if (is_array($Data))
			{
				$this->OS["ID"] = array_shift($Data);
				$this->OS["Active"] = array_shift($Data);
				$this->OS["Name"] = array_shift($Data);
			}
			if (!$this->OS["Active"])
			{
				$this->OS_KEY = "";
				$this->OS["ID"] = "";
				$this->OS["Name"] = "";
			}
		}

		if (!$this->OS["Active"] && trim($AlternatePage))
		{
			if (trim($AlternatePage))
				exit($AlternatePage);
		}
		elseif (!$this->OS["Active"] && ($this->Action != $this->OS["Root"]))
		{
			$URL  = "";
			$URL .= "".$_SERVER["PHP_SELF"]."?Action=".$this->OS["Root"];
			if ($this->OS_DEST)
	        	$URL .= "&OS_DEST=".$this->OS_DEST;
			else
			{
	        	$URL .= "&OS_DEST=";
				$URL .= base64_encode($this->GetHere());
			}
			Header("Location: ".$URL);
			exit;
		}
		return ($this->OS_KEY);
	}

	function GetHere()
	{
		$URL  = "";
		if ($_SERVER["SERVER_PORT"] == 443)
			$URL .= "https://";
		else
			$URL .= "http://";
		$URL .= $_SERVER["HTTP_HOST"];
		$URL .= $_SERVER["REQUEST_URI"];
		if (defined("OBJECTS_CHR"))
			$URL = str_replace(OBJECTS_CHR, urlencode(OBJECTS_CHR), $URL);
		else
			$URL = str_replace("|", urlencode("|"), $URL);
		return($URL);
	}

	function GoToDest($KEY)
	{
			$URL = base64_decode($this->OS_DEST);
			$FROM = str_replace(array("https://", "http://"), "", $URL);
			$HOST = $_SERVER["HTTP_HOST"];
			if (substr($FROM, 0, strlen($HOST)) != $HOST)
				$URL = $this->GetHere();

			if (strstr($URL, "?"))
				$URL .= "&";
			else
				$URL .= "?";
			$URL .= "OS_KEY=".$KEY;
			Header("Location: ".$URL);
	}

	function Password2ID($Login, $Password)
	{
		if ($this->Encrypt)
		{
			$Query  = "SELECT ".$this->OS["TB"][6]." ";
			$Query .= "FROM ".$this->OS["TB"][0]." ";
			$Query .= "WHERE ".$this->OS["TB"][2]." ";
			$Query .= "AND ".$this->OS["TB"][5]." = '".sql::text($Login)."' ";

			$UPASS = DBGet($this->OS["DB"], $Query);

			$Key = substr($UPASS, 0, 2);
			$Query  = "SELECT ".$this->OS["TB"][1]." ";
			$Query .= "FROM ".$this->OS["TB"][0]." ";
			$Query .= "WHERE ".$this->OS["TB"][2]." ";
			$Query .= "AND ".$this->OS["TB"][5]." = '".sql::text($Login)."' ";
			$Query .= "AND ".$this->OS["TB"][6]." = encrypt('".addslashes($Password)."', '".$Key."') ";
			$UID = DBGet($this->OS["DB"], $Query);
		}
		else
		{
			$Query  = "select ".$this->OS["TB"][1];
			$Query .= " from ".$this->OS["TB"][0];
			$Query .= " where ".$this->OS["TB"][2]." ";
			$Query .= " and ".$this->OS["TB"][5]." = '".sql::text($Login)."'";
			$Query .= " and ".$this->OS["TB"][6]." = '".sql::text($Password)."'";
			$UID = DBGet($this->OS["DB"], $Query);
		}
		return($UID);
	}

	function SetCookies($Key, $Expires, $Path, $Host)
	{
		setcookie(COOKIE_NAME, $Key, 00000000, $Path, $Host, ($_SERVER["HTTPS"] == "on"), true);
	}

	function UnsetCookies()
	{
		$Path = explode("/", substr($this->CookiePath, 1, strlen($this->CookiePath) - 2));
		while (sizeof($Path))
		{
			$Delete = "/".implode("/", $Path)."/";
			if ($Delete == "//")
				$Delete = "/";
			$this->SetCookies("", (time() - 3600), $Delete, $this->CookieHost);
			array_pop($Path);
		}
	}

	function CreateOSKey($DB, $Table, $UserID, $Key)
	{
			$KEY = CreateUniqueKey(32);
			$Query = "select count(*) from ".$Table." where ".$Key." = '".sql::text($KEY)."'";
			$Count = DBGet($DB, $Query);
			if ($Count)
				$KEY = $this->CreateOSKey($DB, $Table, $UserID, $Key);
			return ($KEY);
	}

	function ContentArea()
	{
		switch ($this->OS_FORM)
		{
			case "Login":
				echo "<h3>Incorrect login: please try again</h3>";
				break;
			case "Retrieve":
				if ($this->Mailed)
					echo "<h3>Your login and password have been emailed to ".$this->OS_EMAIL."</h3>";
				else
					echo "<h3>'".$this->OS_EMAIL."' not found in our subscriber database: please try again.</h3>";
				break;
			case "Change":
				echo "<ul><li>".implode("</li><li>", $this->OS["ChangeMessage"])."</li></ul>";
				break;
		}
		if ($this->OS_KEY)
		{
			echo "<h2>".$this->OS["Name"].", you are currently logged in.</h2>";
			Field::PutFormStart("".$_SERVER["PHP_SELF"]."?Action=".$this->Action."&OS_DEST=".$this->OS_DEST, 1, "id=\"PasswordObjectForm\"");
			Field::PutHidden("OS_KEY", $this->OS_KEY);
			Field::PutFieldsetStart("Change Password");
			echo "<h5>Current Password</h5>";
			Field::PutPassword("OS_PASSWORD[old]", "", 16);
			echo "<h5>New Password</h5>";
			Field::PutPassword("OS_PASSWORD[new]", "", 16);
			echo "<h5>Confirm Password</h5>";
			Field::PutPassword("OS_PASSWORD[confirm]", "", 16);
			Field::PutFormSubmit("Change", "OS_FORM");
			Field::PutFieldsetEnd();
			Field::PutFormEnd();
			if ($this->Expires)
				echo "<h5><a href=\"".$_SERVER["PHP_SELF"]."?Action=".$this->OS["Root"]."&OS_FORM=Logout\">Logout</a></h5>";
		}
		else
		{
			if ($this->OS_DEST == "")
				$this->OS_DEST = base64_encode($this->GetHere());

			Field::PutFormStart("".$_SERVER["PHP_SELF"]."?Action=".$this->Action."&OS_DEST=".$this->OS_DEST, 1, "id=\"PasswordObjectForm\"");
			Field::PutFieldsetStart("Site Login");
			echo "<h3>Login</h3>";
			Field::PutText("OS_LOGIN", "", 16);
			echo "<h3>Password</h3>";
			Field::PutPassword("OS_PASSWORD", "", 16);
			Field::PutFormSubmit("Login", "OS_FORM");
			Field::PutFieldsetEnd();

			if (!$this->Encrypt)
			{
				Field::PutFieldsetStart("Forgotten Password?");
				echo "<p>Please provide your email address.</p>";
				Field::PutText("OS_EMAIL", "", 50);
				Field::PutFormSubmit("Retrieve", "OS_FORM");
				Field::PutFieldsetEnd();
			}
			Field::PutFormEnd();
		}
	}

	function GetKey()
	{
		if ($_REQUEST[COOKIE_NAME])
			return ("");
		else
			return ($this->OS_KEY);
	}

	function GetObjectKey()
	{
		if ($_REQUEST[COOKIE_NAME])
			return ($_REQUEST[COOKIE_NAME]);
		else
			return ($this->OS_KEY);
	}

	function GetID()
	{
		return($this->OS["ID"]);
	}

	function ObjectLevelOn()
	{
		if ($this->GetID())
			$this->OSO = new ObjectSecurityOn($this->OS["DB"], $this->GetID());
	}

	function IsAdmin()
	{
		if (strtolower(get_class($this->OSO)) == "objectsecurityon")
			$this->OSO->IsAdmin();
	}

	function AllowView($Object)
	{
		if (strtolower(get_class($this->OSO)) == "objectsecurityon")
			return($this->OSO->AllowView($Object));
		else
			return 1;
	}

	function AllowEdit($Object)
	{
		if (strtolower(get_class($this->OSO)) == "objectsecurityon")
			return($this->OSO->AllowEdit($Object));
		else
			return 1;
	}

	function AllowDelete($Object)
	{
		if (strtolower(get_class($this->OSO)) == "objectsecurityon")
			return($this->OSO->AllowDelete($Object));
		else
			return 1;
	}

	function DenyView($Object)
	{
		if (strtolower(get_class($this->OSO)) == "objectsecurityon")
			return($this->OSO->DenyView($Object));
		else
			return 1;
	}

	function DenyEdit($Object)
	{
		if (strtolower(get_class($this->OSO)) == "objectsecurityon")
			return($this->OSO->DenyEdit($Object));
		else
			return 1;
	}

	function DenyDelete($Object)
	{
		if (strtolower(get_class($this->OSO)) == "objectsecurityon")
			return($this->OSO->DenyDelete($Object));
		else
			return 1;
	}
}

class ObjectSecurityOn extends Variable
{
	function __construct($DB, $Identity, $Table1 = "", $Table2 = "", $Table3 = "")
	{
		parent::__construct();
		$this->DB = $DB;
		$this->ID = $Identity;
		if ($Table1 == "")
			$Table1 = array("security_access", "object_code", "status");
		if ($Table2 == "")
			$Table2 = array("security_permissions", "object_code", "status", "identity");
		if ($Table3 == "")
			$Table3 = array("security_purpose", "object_code", "status");

		$this->T1 = $Table1;
		$this->T2 = $Table2;
		$this->T3 = $Table3;
		$this->Security = array();
		$Q = new DBQuery;
		$Q->Run($this->DB, "select * from ".$this->T1[0]);
		while ($Q->Next())
			$this->Security[$Q->GetField($this->T1[1])] = $Q->GetField($this->T1[2]);
		$Q->EndQuery();
		$this->Purpose = array();
		$Q = new DBQuery;
		$Q->Run($this->DB, "select * from ".$this->T3[0]);
		while ($Q->Next())
			$this->Purpose[$Q->GetField($this->T3[1])] = $Q->GetField($this->T3[2]);
		$Q->EndQuery();

		$this->LevelCheck = array();
		$this->LevelCheck["list"]        = array("edit", "view", "wipe", "none");
		$this->LevelCheck["wipe"]["obj"] = array("none");
		$this->LevelCheck["wipe"]["usr"] = array("wipe");
		$this->LevelCheck["edit"]["obj"] = array("none", "wipe");
		$this->LevelCheck["edit"]["usr"] = array("wipe", "edit");
		$this->LevelCheck["view"]["obj"] = array("none", "wipe", "edit");
		$this->LevelCheck["view"]["usr"] = array("wipe", "edit", "view");

		$All = array();
		foreach(Objects::ReadObjects(OBJECTS_DIR) as $Item)
		{
			$Extended = $Item["extends"];
			if (!$Root[$Extended])
				$Root[$Extended] = "yes";
			if (!is_array($$Extended))
				$$Extended = array();

			$Extender = $Item["class"];
			$Root[$Extender] = "no";
			if (!is_array($$Extender))
				$$Extender = array();
			${$Extended}[$Extender] = &$$Extender;
			$All[] = $Extended;
			$All[] = $Extender;
		}

		$Sequence = main::path();
		$Start = "ABCXYZ";
		while (!in_array($Start, $All))
			$Start = strtolower(array_shift($Sequence));

		$this->Root = $$Start;
		foreach (main::path() as $Object)
		{
			$ObjectLevel = $this->Purpose[$Object];
			if (!$ObjectLevel)
				$ObjectLevel = "view";
			if (!$this->SecurityCheck($Object, $ObjectLevel))
			{
				Header("Location: ".$_SERVER["PHP_SELF"]);
				exit;
			}
		}
	}

	function IsAdmin()
	{
		return (0);
	}

	function SecurityCheck($Object, $Level = "")
	{
		$Q = new DBQuery;

		// everyone has access to an uprotected object
		if (!in_array($Level, $this->LevelCheck["list"]))
			return (1);
		// grant administrators unrestricted access
		if ($this->IsAdmin)
			return (1);

		// check user privelages
		$Query = "select ".$this->T2[2]." from ".$this->T2[0]." where ".$this->T2[3]." = '".$this->ID."' and ".$this->T2[1]." = '".sql::text($Object)."'";
		$Q->RunOne($this->DB, $Query);
		$UserLevel = $Q->GetField($this->T2[2]);
		if (!in_array($UserLevel, $this->LevelCheck["list"]))
			$UserLevel = "none";
		$Q->EndQuery();
		if (in_array($UserLevel, $this->LevelCheck[$Level]["usr"]))
			return (1);

		// check object privelages
		$ObjectLevel = $this->Security[$Object];

		if (!in_array($ObjectLevel, $this->LevelCheck["list"]))
			$ObjectLevel = "none";
		if (in_array($ObjectLevel, $this->LevelCheck[$Level]["obj"]))
			return (1);

		return (0);
	}

	function SecurityCheckLoop($Object, $Level)
	{
		if ($this->SecurityCheck($Object, $Level))
		{
			$Objects = $this->FindObject($Object, $this->Root);
			if (is_array($Objects))
				foreach ($Objects as $Parent)
				{
					if ($this->SecurityCheck($Parent, $Level) == 0)
						return (0);
				}
			return (1);
		}

		return (0);
	}

	function AllowView($Object)
	{
		return ($this->SecurityCheckLoop($Object, "view"));
	}

	function AllowEdit($Object)
	{
		return ($this->SecurityCheckLoop($Object, "edit"));
	}

	function AllowDelete($Object)
	{
		return ($this->SecurityCheckLoop($Object, "wipe"));
	}

	function DenyView($Object)
	{
		return (!$this->SecurityCheckLoop($Object, "view"));
	}

	function DenyEdit($Object)
	{
		return (!$this->SecurityCheckLoop($Object, "edit"));
	}

	function DenyDelete($Object)
	{
		return (!$this->SecurityCheckLoop($Object, "wipe"));
	}

	function GetID()
	{
		return($this->ID);
	}

	function FindObject($Find, $Root, $Keys = 0)
	{
		$Data = "";
		if (!is_array($Keys))
			$Keys = array();
		if (array_key_exists($Find, $Root))
			return ($Keys);
		foreach($Root as $Key => $Branch)
		{
			array_unshift($Keys, $Key); 
			$Data = $this->FindObject($Find, $Branch, $Keys);
			array_shift($Keys); 
			if (is_array($Data))
				return ($Data);
		}
	}
}

class ObjectSecurityShow
{
	function __construct($DB, $Users, $Table1 = "", $Table2 = "", $Table3 = "")
	{
		$Labels = array();
		foreach(Objects::ReadObjects(OBJECTS_DIR) as $Item)
		{
			if ($Item["label"])
				$Labels[$Item["class"]] = $Item["label"];
			else
				$Labels[$Item["class"]] = $Item["class"];
		}
		$this->DB = $DB;
		if ($Table1 == "")
			$Table1 = array("security_access", "object_code", "status");
		if ($Table2 == "")
			$Table2 = array("security_permissions", "object_code", "status", "identity");
		if ($Table3 == "")
			$Table3 = array("security_purpose", "object_code", "status");
		$this->T1 = $Table1;
		$this->T2 = $Table2;
		$this->T3 = $Table3;
		$Q = new DBQuery;

		$Modes = array();
		$Modes["none"] = "No Security on Object";
		$Modes["wipe"] = "Permission is given to to View, Edit/Save or Delete";
		$Modes["edit"] = "Permission is given to Edit/Save or Delete";
		$Modes["view"] = "Permission is given to View";

		$Objects = array();
		$Q->Run($this->DB, "select * from ".$this->T1[0]);
		while ($Q->Next())
			$Objects[$Q->GetField($this->T1[1])] = $Q->GetField($this->T1[2]);
		$Q->EndQuery();
		ksort($Objects);

		echo "<table>\n";
		foreach($Objects as $Object => $Mode)
		{
			echo "<tr>\n";
			echo "<th colspan=2>".$Labels[$Object]."</th>\n";
			echo "</tr>\n";

			$Q->Run($this->DB, "select * from ".$this->T2[0]." where ".$this->T2[1]." = '".sql::text($Object)."'");
			while ($Q->Next())
			{
				echo "<tr>\n";
				echo "<td>".$Users[$Q->GetField($this->T2[3])]."</td>\n";
				echo "<td>".$Modes[$Q->GetField($this->T2[2])]."</td>\n";
				echo "</tr>\n";
				$Objects[$Q->GetField($this->T1[1])] = $Q->GetField($this->T1[2]);
			}
			$Q->EndQuery();
			ksort($Objects);
		}	
		echo "</table>\n";
	}
}

class ObjectSecurityAdmin
{
	function __construct($DB, $Identity, $Basic = 0, $Table1 = "", $Table2 = "", $Table3 = "")
	{
		$Labels = array();
		foreach(Objects::ReadObjects(OBJECTS_DIR) as $Item)
		{
			if ($Item["label"])
				$Labels[$Item["class"]] = $Item["label"];
			else
				$Labels[$Item["class"]] = $Item["class"];
		}
		$this->URL = $_SERVER["REQUEST_URI"];
		$this->DB = $DB;
		$this->ID = $Identity;
		if ($Table1 == "")
			$Table1 = array("security_access", "object_code", "status");
		if ($Table2 == "")
			$Table2 = array("security_permissions", "object_code", "status", "identity");
		if ($Table3 == "")
			$Table3 = array("security_purpose", "object_code", "status");
		$this->T1 = $Table1;
		$this->T2 = $Table2;
		$this->T3 = $Table3;
		$Q = new DBQuery;

		$this->Objects = array();
		$Q->Run($this->DB, "select * from ".$this->T1[0]);
		while ($Q->Next())
			$this->Objects[$Q->GetField($this->T1[1])] = $Q->GetField($this->T1[2]);
		$Q->EndQuery();
		ksort($this->Objects);

		$this->User = array();
		$Q->Run($this->DB, "select * from ".$this->T2[0]." where ".$this->T2[3]." = '".$this->ID."'");
		while ($Q->Next())
			$this->User[$Q->GetField($this->T2[1])] = $Q->GetField($this->T2[2]);
		$Q->EndQuery();

		$this->Modes = array();
		$this->Modes["none"] = "No Security on Object";
		$this->Modes["wipe"] = "Permission is required to Delete";
		$this->Modes["edit"] = "Permission is required to Edit/Save or Delete";
		$this->Modes["view"] = "Permission is required to View, Edit/Save and Delete";
		$this->ModesFull = $this->Modes;
		$this->ModesFull["none"] .= " (all users can perform all actions)";
		$this->ModesFull["wipe"] .= " (View and Edit/Save granted)";
		$this->ModesFull["edit"] .= " (View access granted)";

		$this->Purposes = array();
		$this->Purposes["view"] = "Object is used for Viewing Only";
		$this->Purposes["edit"] = "Object is used for Editing/Creating";
		$this->Purposes["wipe"] = "Object is used for Deleting";

		$this->LevelCheck = array();
		$this->LevelCheck["list"] = array("edit", "view", "wipe");
		$this->LevelCheck[""] = array();
		$this->LevelCheck["none"] = array();
		$this->LevelCheck["wipe"] = array("view", "edit");
		$this->LevelCheck["edit"] = array("view");
		$this->LevelCheck["view"] = array();

		$this->LevelChange = array();
		$this->LevelChange["view"] = "none";
		$this->LevelChange["edit"] = "view";
		$this->LevelChange["wipe"] = "edit";

		$SecurityObject = $_REQUEST["SECURITY_OBJECT"];
		$SecurityAction = $_REQUEST["SECURITY_ACTION"];
		switch ($SecurityAction)
		{
			case "hide":
				exit;
				break;
			case "none":
			case "wipe":
			case "edit":
			case "view":
				$Query = "delete from ".$this->T2[0]." where ".$this->T2[1]." = '".$SecurityObject."' and ".$this->T2[3]." = '".$this->ID."'";
				DB_DoQuery($this->DB, $Query);
				if (($SecurityAction != "none") && !in_array($SecurityAction, $this->LevelCheck[$this->Objects[$SecurityObject]]))
				{
					$INQ = new DBInsert;
					$INQ->Save($this->T2[1], $SecurityObject);
					$INQ->Save($this->T2[2], $SecurityAction);
					$INQ->Save($this->T2[3], $this->ID);
					$INQ->Run($this->DB, $this->T2[0], "INSERT");
				}
				echo "<script>\n";
				echo "top.document.location.reload();\n";
				echo "</script>\n";
				break;
			default:
				echo "<style>\n";
				echo "TD.no A:link, TD.no A:visited\n";
				echo "{\n";
				echo "	font-weight: bold;\n";
				echo "	color: #990000\n";
				echo "}\n";
				echo "TD.yes A:link, TD.yes A:visited\n";
				echo "{\n";
				echo "	font-weight: bold;\n";
				echo "	color: #009900\n";
				echo "}\n";

				echo "#TakeAction\n";
				echo "{\n";
				echo "	position: absolute;\n";
				echo "	left: 0;\n";
				echo "	top: 0;\n";
				echo "	width: 0;\n";
				echo "	height: 0;\n";
				echo "	clip: rect(0,0,0,0);\n";
				echo "}\n";

				echo "</style>\n";

				echo "<div id=\"TakeAction\">\n";
				echo "<iframe src=\"".$this->URL."&SECURITY_ACTION=hide\" ";
				echo "name=\"textFrame\" scrolling=\"no\" ";
				echo "width=\"0\" height=\"0\" ";
				echo "marginwidth=\"0\" marginheight=\"0\" ";
				echo "frameborder=\"0\">\n";
				echo "</iframe>\n";
				echo "</div>\n";

				echo "<table>\n";

				echo "<tr>\n";
				echo "<th>Object</th>\n";
				if ($Basic)
					echo "<th>Access</th>\n";
				else
				{
					echo "<th>View</th>\n";
					echo "<th>Edit</th>\n";
					echo "<th>Delete</th>\n";
				}
				echo "</tr>\n";

				foreach ($this->Objects as $Object => $Status)
				{
					echo "<tr>\n";
					echo "<td>".$Labels[$Object]."</td>\n";
					if ($Basic)
						$this->MakeSecurityLink($Object, $Status, "auto");
					else
					{
						$this->MakeSecurityLink($Object, $Status, "view");
						$this->MakeSecurityLink($Object, $Status, "edit");
						$this->MakeSecurityLink($Object, $Status, "wipe");
					}
					echo "</tr>\n";
				}

				echo "</table>\n";
			break;
		}
	}

	function MakeSecurityLink($Object, $Status, $Column)
	{
		if ($Column == "auto")
		{
			if (in_array("wipe", $this->LevelCheck[$this->User[$Object]]) || ("wipe" == $this->User[$Object]))
				echo "<td class=\"yes\"><a target=\"textFrame\" href=\"".$this->URL."&SECURITY_OBJECT=".$Object."&SECURITY_ACTION=".$this->LevelChange["view"]."\">Yes</a></td>\n";
			else
				echo "<td class=\"no\"><a target=\"textFrame\" href=\"".$this->URL."&SECURITY_OBJECT=".$Object."&SECURITY_ACTION=wipe\">No</a></td>\n";
		}
		elseif (in_array($Column, $this->LevelCheck[$Status]))
			echo "<td>Yes</td>\n";
		else
		{
			if (in_array($Column, $this->LevelCheck[$this->User[$Object]]) || ($Column == $this->User[$Object]))
				echo "<td class=\"yes\"><a target=\"textFrame\" href=\"".$this->URL."&SECURITY_OBJECT=".$Object."&SECURITY_ACTION=".$this->LevelChange[$Column]."\">Yes</a></td>\n";
			else
				echo "<td class=\"no\"><a target=\"textFrame\" href=\"".$this->URL."&SECURITY_OBJECT=".$Object."&SECURITY_ACTION=".$Column."\">No</a></td>\n";
		}
	}
}

?>
