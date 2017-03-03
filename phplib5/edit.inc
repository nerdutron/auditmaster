<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

require_once(PHPLIB_DIR."tools.inc");
require_once(PHPLIB_DIR."table.inc");
require_once(PHPLIB_DIR."database.inc");
require_once(PHPLIB_DIR."datefield.inc");

// initalise bitflags
define("EDIT_LIST",   1);
define("EDIT_NOEDIT", 2);
define("EDIT_NOSHOW", 4);
define("EDIT_NOSAVE", 8);
define("EDIT_FORM",  16);
define("EDIT_GROUP", 32);
define("EDIT_AUTO_INC", 64);

class Edit
{
	function __construct($Config) // define
	{
		$this->EditErrors = array();

		$this->EditDB = $Config["EditDB"];
		$this->EditURL = $Config["EditURL"];
		$this->BackURL = $Config["BackURL"];
		$this->EditList = $Config["EditList"];
		$this->EditSeq = $Config["EditSeq"];
		$this->EditOrder = $Config["EditOrder"];
		$this->EditTable = $Config["EditTable"];
		$this->EditNoBack = $Config["EditNoBack"];
		if ($Config["NoEdit"])
			$this->EditNoEdit = 1;
		if ($Config["NoDelete"])
			$this->EditNoDelete = 1;
		if ($Config["NoNew"])
			$this->EditNoNew = 1;
		$this->EditPrimaryKey = $Config["EditPrimaryKey"];
		$this->EditFormName = $Config["EditFormName"];

//		$this->InitEdit();
//	}
//
//	function InitEdit()
//	{
		$this->Dates = new DateField;
		// internalise all necessary data
		$this->EditAction = $_REQUEST["EditAction"];
		$this->DataGroups = $_REQUEST["DataGroups"];
		if (!is_array($this->DataGroups))
			$this->DataGroups = array();
		$this->DataFields = $_REQUEST["DataFields"];
		if (!is_array($this->DataFields))
			$this->DataFields = array();
		$this->DataPrimary = $_REQUEST["DataPrimary"];
		if (!is_array($this->DataPrimary))
			$this->DataPrimary = array();

		// work out which fields are used for grouping
		$this->EditGroups = array();
		foreach ($this->EditList as $Key => $Data)
			if ($Data["flag"]&EDIT_GROUP)
				$this->EditGroups[] = $Key;

		// preform any necessary pre-actions
		$this->EditPreAction();
		switch ($this->EditAction)
		{
			case "save":
			case "savenew":
				$Valid = $this->EditValidate();
				if ($Valid)
					$this->EditSave();
				else
				{
					$this->EditAction = ($this->EditAction == "savenew" ? "editnew" : "edit");
					foreach ($this->EditPrimaryKey as $Key)
						$this->DataEdit[$Key] = $this->DataPrimary[$Key];
					$this->EditFetch();
				}
				break;
			case "move":
				$this->MoveID = $_REQUEST["MoveID"];
				$this->EditMove();
				break;
			case "delete":
				$this->EditFetch();
				$this->DataEdit = $_REQUEST["DataEdit"];
				$this->EditDelete();
				break;
			case "edit":
			case "editnew":
				$this->DataEdit = $_REQUEST["DataEdit"];
				$this->EditFetch();
				break;
			default:
				$this->EditFetch();
				break;
		}
	}

	function EditScreen()
	{
		// display any errors
		$this->EditErrorList();

		if ($this->EditAction == "edit" || $this->EditAction == "editnew")
			$this->EditEdit();
		elseif (sizeof($this->EditGroups) && sizeof($this->DataGroups) == 0)
			$this->EditList();
		else
			$this->EditData();
	}

	// headless functions that can be overloaded
	function EditPreAction() { }
	function EditPreDelete() { }
	function EditPostDelete() { }
	function EditPreList() { }
	function EditPostList() { }
	function EditPreData() { }
	function EditPostData() { }
	function EditPreEdit() { }
	function EditPostEdit() { }
	function EditPreValidate() { }
	function EditPostValidate() { }
	function EditPreErrorList() { }
	function EditPostErrorList() { }
	function EditPreSave() { }
	function EditPostSave() { }

	function EditDataView($Key, $Data)
	{
		switch($Key)
		{
			default:
				return ($Data);
				break;
		}
	}

	function EditDataEdit($Key, $Data)
	{
		switch($Key)
		{
			default:
				return ($Data);
				break;
		}
	}


	function EditMove()
	{
		// if no EditSeq
		if (!$this->EditSeq)
			$this->EditSeq = "sequence_nr";
		
		$Where  = "";
		foreach ($this->DataGroups as $Key => $Val)
			$Where .= " and ".$Key." = '".addslashes($Val)."'";

		$Query  = " from ".$this->EditTable." where ".$this->EditSeq." = ".($this->MoveID*1)." ".$Where;

		// make sure the row can be found!!!
		$Q = new DBQuery;
		$Q->RunOne($this->EditDB, "SELECT COUNT(*) AS counter".$Query);
		$Count = $Q->GetField("counter");
		$Q->EndQuery();

		// row can only be moved if it is the ONLY record with this number
		if($Count == 1)
		{
			// what would be the next sequence number?
			$EndID = $this->EditMaxSequence() + 1;

			// move the specified row to the end
			$Query = "UPDATE ".$this->EditTable." SET ".$this->EditSeq." = ".$EndID
				." WHERE ".$this->EditSeq." = ".$this->MoveID.$Where;
			DB_DoQuery($this->EditDB, $Query);

			// move the next row back to here (if it exists)
			$Query = "UPDATE ".$this->EditTable." SET ".$this->EditSeq." = ".$this->MoveID
				." WHERE ".$this->EditSeq." = ".($this->MoveID + 1).$Where;
			DB_DoQuery($this->EditDB, $Query);

			// move the original row (now at the end) to it's new position
			// at $this->MoveID + 1
			$Query = "UPDATE ".$this->EditTable." SET ".$this->EditSeq." = ".($this->MoveID + 1)
				." WHERE ".$this->EditSeq." = ".$EndID.$Where;
			DB_DoQuery($this->EditDB, $Query);

//			$Q->EndQuery();
//
//			$Q = new DBQuery;
//			$Q->RunOne($this->EditDB, "select *".$Query);
//			$Store = $Q->GetArray();
//			$Store[$this->EditSeq]++;
//			$Q->EndQuery();
//
//			DB_DoQuery($this->EditDB, "delete".$Query);
//
//			$Query  = "update ".$this->EditTable." set ".$this->EditSeq." = ".$this->MoveID;
//			$Query .= " where ".$this->EditSeq." = ".($this->MoveID + 1)." ".$Where;
//			DB_DoQuery($this->EditDB, $Query);
//
//			$INQ = new DBInsert;
//			foreach ($Store as $Key => $Val)
//				$INQ->Add("", $Key, $Val);
//			$INQ->Run($this->EditDB, $this->EditTable, "INSERT");
//			$INQ->EndQuery();
		}

		$this->EditFetch();
		$this->EditResetSequence();

		$Row = array();
		foreach ($this->EditGroups as $Key)
			$Row["DataGroups[".$Key."]"] = $this->DataGroups[$Key];
		Header("Location: ".$this->EditURL($Row));
		exit;
	}

	function EditResetSequence()
	{
//		$Query = "SELECT ".implode(", ", $this->EditPrimaryKey).", ".$this->EditSeq." FROM ".$this->EditTable;
//		$Join = " WHERE ";
//		foreach ($this->EditGroups as $Key)
//		{
//			$Query .= $Join.$Key." = '".$this->DataGroups[$Key]."'";
//			$Join = " and ";
//		}
//		$Query .= " ORDER BY ".$this->EditSeq;

		$Q = new DBQuery;
//		$Q->Run($this->EditDB, $Query);
		$Q->Run($this->EditDB, $this->EditQuery);
		$i = 0;
		while ($Q->Next())
		{
			$i++;
			if ($i != $Q->GetField($this->EditSeq))
			{
				$INQ = new DBInsert;
				$INQ->Add("", $this->EditSeq, $i);
				foreach ($this->EditPrimaryKey as $Key)
					$INQ->AddWhere("", $Key, $Q->GetField($Key));
				$INQ->Run($this->EditDB, $this->EditTable, "UPDATE");
				$INQ->EndQuery();
			}
		}
		$Q->EndQuery();
	}

	function EditDelete()
	{
		$this->EditPreDelete();
		if (is_array($this->DataEdit))
			$this->MakeDelete($this->DataEdit);
		else
			$this->MakeDelete($this->DataGroups);

		if ($this->EditSeq && (sizeof($this->EditGroups) == sizeof($this->DataGroups)))
//		{
			$this->EditResetSequence();
//			$Q = new DBQuery;
//			$Q->Run($this->EditDB, $this->EditQuery);
//			$i = 0;
//			while ($Q->Next())
//			{
//				$i++;
//				if ($i != $Q->GetField($this->EditSeq))
//				{
//					$INQ = new DBInsert;
//					$INQ->Add("", $this->EditSeq, $i);
//					foreach ($this->EditPrimaryKey as $Key)
//						$INQ->AddWhere("", $Key, $Q->GetField($Key));
//					$INQ->Run($this->EditDB, $this->EditTable, "UPDATE");
//					$INQ->EndQuery();
//				}
//			}
//			$Q->EndQuery();
//
//		}

		$this->EditPostDelete();
		if (is_array($this->DataEdit))
			foreach ($this->EditGroups as $Key)
				$Row["DataGroups[".$Key."]"] = $this->DataGroups[$Key];
		Header("Location: ".$this->EditURL($Row));
		exit;
	}

	function EditFetch()
	{
		$this->EditQuery = "";

		if ($this->EditAction == "editnew")
			return(0);

		if ($this->EditAction == "edit")
		{
			$this->EditQuery = "select * from ".$this->EditTable;
			$Join = " where ";
			foreach ($this->EditPrimaryKey as $Key)
			{
				$this->EditQuery .= $Join;
				$this->EditQuery .= $Key;
				$this->EditQuery .= " = ";
				$this->EditQuery .= "'".$this->DataEdit[$Key]."'";
				$Join = " and ";
			}
		}
		elseif (sizeof($this->EditGroups) == 0)
		{
			$this->EditQuery = "select * from ".$this->EditTable;
		}
		elseif (sizeof($this->DataGroups) == 0)
		{
			if (sizeof($this->EditGroups) == 1)
			{
				$this->EditQuery  = "select distinct(".implode(",", $this->EditGroups).")";
				$this->EditQuery .= " from ".$this->EditTable;
			}
			else
			{
				$this->EditQuery  = "select distinct ".implode(",", $this->EditGroups);
				$this->EditQuery .= " from ".$this->EditTable;
			//	$this->EditQuery .= " group by ".implode(",", $this->EditGroups);
			}
		}
		else
		{
			$this->EditQuery = "select * from ".$this->EditTable;
			$Join = " where ";
			foreach ($this->EditGroups as $Key)
			{
				$this->EditQuery .= $Join;
				$this->EditQuery .= $Key;
				$this->EditQuery .= " = ";
				$this->EditQuery .= "'".$this->DataGroups[$Key]."'";
				$Join = " and ";
			}
		}

		if (!strstr($this->EditQuery, "select distinct"))
		{
			if($this->EditSeq)
				$this->EditQuery .= " order by ".$this->EditSeq;
			else if($this->EditOrder)
				$this->EditQuery .= " order by ".$this->EditOrder;
		}
	}

	function EditList()
	{
		$this->EditPreList();

		$Fields = array();
		if (sizeof($this->EditGroups))
			$Fields = $this->EditGroups;
		else
			foreach ($this->EditList as $Key => $Data)
				if ($Data["flag"]&EDIT_LIST)
					$Fields[] = $Key;

		$Q = new DBQuery;
		$Q->Run($this->EditDB, $this->EditQuery);
		Field::PutFormStart($this->EditURL(), 1);
		echo "<h3>Data:</h3>\n";
		$this->T = new Table;
		foreach ($Fields as $Key)
			if ($this->EditList[$Key]["name"])
				$this->T->AddHead($this->EditList[$Key]["name"]);
			else
				$this->T->AddHead($Key);
		$this->T->AddHead("Actions", 2);
		$this->T->EndRow();
		while ($Q->Next())
		{
			$Row = array();
			foreach ($Fields as $Key)
			{
				$Row["DataGroups[".$Key."]"] = $Q->GetField($Key);
				$this->EditDataShow($Key, $Q->GetField($Key));
			}
			if ($this->EditNoEdit)
				$this->T->AddData("&nbsp;");
			else
				$this->T->AddData("<a href=\"".$this->EditURL($Row)."\">Edit</a>");
			$Row["EditAction"] = "delete";
			if ($this->EditNoDelete)
				$this->T->AddData("&nbsp;");
			else
				$this->T->AddData("[<a href=\"".$this->EditURL($Row)."\" onclick=\"return confirm('Do you wish to delete this group?')\">X</a>]");
			$this->T->EndRow();
		}
		if (sizeof($this->EditGroups))
			foreach($Fields as $Key)
			{
				$this->T->AddData($this->EditFieldCreate($Key, "DataGroups"));
			}
		if (!$this->EditNoNew)
			$this->T->AddData(Field::GetFormSubmit("Add"), 2);
		$this->T->EndRow();
		$this->T->Put();
		Field::PutFormEnd();
		$Q->EndQuery();

		$this->EditPostList();
	}

	function EditDataShow($Key, $Val)
	{
		$Cell = "";
		$Val = $this->EditDataView($Key, $Val);
		switch ($this->EditList[$Key]["type"])
		{
			case "file":
				$Parts = explode("|", $Val);
				if (sizeof($Parts) == 2)
				{
					$Cell .= "Current file: ";
					$Cell .= number_format(strlen(gzuncompress(base64_decode($Parts[1]))));
					$Cell .= " byte ";
					$Cell .= "<b>".$Parts[0]."</b>";
					$Cell .= " file.";
				}
				else
					$Cell .= "No current file attached.";
				break;
			case "select":
				$Cell = $this->EditList[$Key]["select"][$Val];
				break;
			case "date":
				$Cell = date("d/m/Y", CreateTimeStamp($Val));
				break;
			case "time":
				$Cell = date("h:ia", CreateTimeStamp($Val));
				break;
			case "time24":
				$Cell = date("H:i", CreateTimeStamp($Val));
				break;
			case "datetime":
				$Cell = date("d/m/Y h:ia", CreateTimeStamp($Val));
				break;
			case "datetime24":
				$Cell = date("d/m/Y H:i", CreateTimeStamp($Val));
				break;
			case "check":
				if (in_array("$Val", array("1","t","true")))
					$Cell = "Yes";
				else
					$Cell = "No";
				break;
			case "text":
			default:
				$Cell = $Val."&nbsp;";
				break;
		}
		$this->T->AddData($Cell);
	}

	function EditGroupShow()
	{
		if (sizeof($this->DataGroups))
		{
			echo "<h2>Currently Editing:</h2>\n";
			$this->T = new Table;
			foreach ($this->DataGroups as $Key => $Val)
			{
				$this->T->AddHead(($this->EditList[$Key]["name"] ? $this->EditList[$Key]["name"] : $Key).":");
				$this->EditDataShow($Key, $Val);
			}
			$this->T->EndRow();
			$this->T->Put();
		}
	}

	function EditData()
	{
		$this->EditPreData();

		$this->EditGroupShow();
		echo "<h3>Data:</h3>\n";
		$Fields = array();
		foreach ($this->EditList as $Key => $Data)
			if ($Data["flag"]&EDIT_LIST)
				$Fields[] = $Key;

		$Q = new DBQuery;
		$Q->Run($this->EditDB, $this->EditQuery);

		$this->T = new Table;
		foreach ($Fields as $Key)
			$this->T->AddHead($this->EditList[$Key]["name"] ? $this->EditList[$Key]["name"] : $Key);

		if ($this->EditSeq)
			$this->T->AddHead("Order", 2);
		$this->T->AddHead("Actions", 2);
		$this->T->EndRow();
		$i = 0;
		while ($Q->Next())
		{
			$i++;
			foreach ($Fields as $Key)
				$this->EditDataShow($Key, $Q->GetField($Key));
			$Row = array();
			foreach($this->DataGroups as $Key => $Val)
				$Row["DataGroups[".$Key."]"] = $Q->GetField($Key);
			$Row["EditAction"] = "move";
			if ($this->EditSeq && !$this->EditNoEdit)
			{
				if ($i > 1)
					$this->T->AddData("<a href=\"".$MoveURL."\">/\\</a>");
				else
					$this->T->AddData("&nbsp;");

				// change $MoveURl to the URL for the new row
				$Move = $Row;
				$Move["MoveID"] = $Q->GetField($this->EditSeq);
				$MoveURL = $this->EditURL($Move);

				if ($i < $Q->NumRows())
					$this->T->AddData("<a href=\"".$MoveURL."\">\\/</a>");
				else
					$this->T->AddData("&nbsp;");
			}
			else if ($this->EditSeq && $this->EditNoEdit)
			{
					$this->T->AddData("&nbsp;");
					$this->T->AddData("&nbsp;");
			}
			foreach ($this->EditPrimaryKey as $Key)
				$Row["DataEdit[".$Key."]"] = $Q->GetField($Key);
			$Row["EditAction"] = "edit";
			if ($this->EditNoEdit)
				$this->T->AddData("&nbsp;");
			else
				$this->T->AddData("<a href=\"".$this->EditURL($Row)."\">Edit</a>");
			$Row["EditAction"] = "delete";
			if ($this->EditNoDelete)
				$this->T->AddData("&nbsp;");
			else
				$this->T->AddData("[<a href=\"".$this->EditURL($Row)."\" onclick=\"return confirm('Do you wish to delete this record?')\">X</a>]");
			$this->T->EndRow();
		}
		echo "<tr>\n";
		$this->T->AddData("&nbsp;", sizeof($Fields));
		if ($this->EditSeq)
			$this->T->AddData("&nbsp;", 2);
		$Row = array();
		foreach ($this->DataGroups as $Key => $Data)
			$Row["DataGroups[".$Key."]"] = $Data;
		$Row["EditAction"] = "editnew";
		if ($this->EditNoNew)
			$this->T->AddData("&nbsp;", 2);
		else
			$this->T->AddData("<a href=\"".$this->EditURL($Row)."\">Add</a>", 2);
		$this->T->EndRow();
		$this->T->Put();

		if (sizeof($this->DataGroups) && !$this->EditNoBack)
			echo "<p><a href=\"".$this->BackURL()."\">&lt;&lt; back</a></p>\n";
		$Q->EndQuery();

		$this->EditPostData();
	}

	function EditEdit()
	{
		$this->EditPreEdit();

		$this->EditGroupShow();
		echo "<h3>Record:</h3>\n";
		if ($this->EditAction == "editnew")
		{
			$Type = "DataGroups";
			Field::PutFormStart($this->EditURL(array("EditAction"=>"savenew")), 1, ($this->EditFormName)?("name=\"".$this->EditFormName."\""):(""));
		}
		else
		{
			$Type = "DataFields";
			$Q = new DBQuery;
			$Q->RunOne($this->EditDB, $this->EditQuery);
			$this->DataFields = array_merge($Q->GetArray(), $this->DataFields);
			$Q->EndQuery();
			Field::PutFormStart($this->EditURL(array("EditAction"=>"save")), 1, ($this->EditFormName)?("name=\"".$this->EditFormName."\""):(""));
		}
		$DataUse = $this->$Type;

		$this->T = new Table;
		foreach ($this->EditList as $Key => $Data)
		{
			if ($Key == $this->EditSeq)
				Field::PutHidden("DataPrimary[".$Key."]", $DataUse[$Key]);
			if ($Data["flag"]&EDIT_GROUP)
				Field::PutHidden("DataFields[".$Key."]", $DataUse[$Key]);
			elseif ($Data["flag"]&EDIT_NOSHOW)
			{
				if (in_array($Key, $this->EditPrimaryKey))
					Field::PutHidden("DataFields[".$Key."]", $DataUse[$Key]);
			}
			else
			{
				$this->T->AddHead(($this->EditList[$Key]["name"] ? $this->EditList[$Key]["name"] : $Key), 1, 1, "style=\"text-align: right;\"");
				if ($Data["flag"]&EDIT_NOEDIT)
				{
					// use the defalt value if we are doing a create (editnew)
					$Value = $this->EditAction == "editnew" ? $this->EditList[$Key]["default"] : $this->DataFields[$Key];
					$this->T->AddData(Field::GetHidden("DataFields[".$Key."]", $Value)
						.$Value."&nbsp;");
				}
				else
					$this->T->AddData($this->EditFieldCreate($Key));
				$this->T->EndRow();
			}
		}
		foreach ($this->EditPrimaryKey as $Key)
			Field::PutHidden("DataPrimary[".$Key."]", $DataUse[$Key]);
		if ($this->EditSeq && !in_array($this->EditSeq, array_keys($this->EditList)))
			Field::PutHidden("DataFields[".$this->EditSeq."]", $DataUse[$this->EditSeq]);
		$this->T->AddData("&nbsp;");
		$this->T->AddData(Field::GetFormSubmit("Save"));
		$this->T->EndRow();
		$this->T->Put();

		if (!$this->EditNoBack)
		{
			$Back = array();
			foreach ($this->DataGroups as $Key => $Val)
				$Back["DataGroups[".$Key."]"] = $Val;
			echo "<p><a href=\"".$this->BackURL($Back)."\">&lt;&lt; back</a></p>\n";
		}
		Field::PutFormEnd();

		$this->EditPostEdit();
	}

	function EditValidate()
	{
		// pre-validation
		$this->EditPreValidate();

		// automatically validate data
		$this->EditAutoValidate();

		// post-validation
		$this->EditPostValidate();

		if(sizeof($this->EditErrors))
			return 0;

		return 1;
	}

	function EditAutoValidate()
	{
		// generic validation here
		// integer, float, alpha, alphanum, (in)valid char list
		// date, time, boolean, required, context required

		foreach($this->EditList as $Key => $Data)
		{
			// $Min and $Max must be set or unset to cater for a value of 0
			if(isset($Data["validate_min"]))
				$Min = $Data["validate_min"];
			else
				unset($Min);
			if(isset($Data["validate_max"]))
				$Max = $Data["validate_max"];
			else
				unset($Max);
			$MinSize = $Data["validate_min_size"];
			$MaxSize = $Data["validate_max_size"];
			$Precision = $Data["validate_precision"];

			$Allow = $Data["validate_allow"];
			if(!is_array($Allow))
			{
				// validate_allow can contain the following patterns to indicate
				// allowable characters: [int][float][alpha][alphanum]
				$CharSets = array(
					"int" => "0123456789",
					"float" => "0123456789.",
					"alpha" => "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
					"lower" => "abcdefghijklmnopqrstuvwxyz",
					"upper" => "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
					"alphanum" => "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
					"special" => " !@\$%^&*()-=_+\\|[]{};:'\",./<>?",
					);

				foreach($CharSets as $Set => $Chars)
				{
					$Pos = strpos($Allow, "[$Set]");
					if($Pos !== false)
					{
						$Allow = substr($Allow, 0, $Pos)
							.$Chars
							.substr($Allow, $Pos+strlen($Set)+2);
					}
				}
			}

			// if this is a date field, build a date string as the value
			if(in_array($Data["type"], array("date", "datetime", "datetime24", "time", "time24")))
			{
				$Value = "";
				$s = $this->DataFields[$Key];
				if (!$s["Opt"])
				{
					$this->Dates->AddDateField($s["Y"], $s["M"], $s["D"], $s["h"], $s["m"], $s["a"]);
					if ($this->Dates->ValidDate())
						$Value = $this->Dates->GetDate();
				}

				$FParts = array();
				if(substr($Data["type"], 0, 4) == "date")
					$FParts[] = "j M Y";
				if(substr($Data["type"], -4) == "time")
					$FParts[] = "h:iA";
				else if(substr($Data["type"], -6) == "time24")
					$FParts[] = "H:i";
				$F = implode(" ", $FParts);

				$MinFormatted = date(implode(" ", $FParts), CreateTimeStamp($Min));
				$MaxFormatted = date(implode(" ", $FParts), CreateTimeStamp($Max));
			}
			else
			{
				$Value = trim($this->DataFields[$Key]);
				$MinFormatted = $Min;
				$MaxFormatted = $Max;
			}

			// which tests do we run?
			if(isset($Min))
			{
				if($Value < $Min)
					$this->EditErrorAdd($Data["name"]." must be at least ".$MinFormatted);
			}
			if(isset($Max))
			{
				if($Value > $Max)
					$this->EditErrorAdd($Data["name"]." must not be more than ".$MaxFormatted);
			}
			if($MinSize)
			{
				if($Data["type"] == "file")
				{
					// test filesize
					if(filesize($_FILES[$Key]["tmp_name"]) < $MinSize)
					{
						// if the size required is 1 exactly, we're really only
						// checking that the file is existent period.
						if($MinSize === 1)
							$this->EditErrorAdd($Data["name"]." must be uploaded");
						else
							$this->EditErrorAdd($Data["name"]." must be at least ".$this->GetFormattedSize($MinSize));
					}
				}
				// otherwise, just test the length of the value
				else if(strlen($Value) < $MinSize)
					$this->EditErrorAdd($Data["name"]." must be at least ".$MinSize
						." character".($MinSize > 1 ? "s" : ""));
			}
			if($MaxSize)
			{
				if($Data["type"] == "file")
				{
					// test filesize
					if(filesize($_FILES[$Key]["tmp_name"]) > $MaxSize)
						$this->EditErrorAdd($Data["name"]." must not be bigger than ".$this->GetFormattedSize($MaxSize));
				}
				// otherwise, just test the length of the value
				else if(strlen($Value) > $MaxSize)
					$this->EditErrorAdd($Data["name"]." must not be more than ".$MaxSize
						." character".($MaxSize > 1 ? "s" : ""));
			}
			if($Precision)
			{
				// test number of decimal places as well as number of decimal characters
				$Parts = explode(".", $Value);
				if(sizeof($Parts) > 2)
					$this->EditErrorAdd($Data["name"]." has more than 1 decimal character");
				else if(strlen($Parts[1]) > $Precision)
					$this->EditErrorAdd($Data["name"]." cannot have more than ".$Precision
						." decimal place".($Precision > 1 ? "s" : ""));
			}
			if($Allow)
			{
				if($Data["type"] == "file")
				{
					$Type = $_FILES[$Key]["type"];
					if($Type && !in_array($Type, $Allow))
						$this->EditErrorAdd($Data["name"]." has an invalid mime type: ".$Type);
				}
				else
				{
					$Illegal = "";
					for($i=0; $i<strlen($Value); $i++)
						if(!strstr($Allow, $Value[$i]) && !strstr($Illegal, $Value[$i]))
							$Illegal .= $Value[$i];
					if($Illegal)
						$this->EditErrorAdd($Data["name"]." contains ".strlen($Illegal)." illegal"
							." character".(strlen($Illegal) > 1 ? "s" : "")
							.": '".htmlspecialchars($Illegal)."'");
				}
			}
			if($Data["validate_numeric"])
			{
				if(!is_numeric($Value))
					$this->EditErrorAdd($Data["name"]." must be numeric");
			}

			// if this is a file upload, make sure it is an uploaded file
			if($Data["type"] == "file" && filesize($_FILES[$Key]["tmp_name"])
				&& !is_uploaded_file($_FILES[$Key]["tmp_name"]))
				$this->EditErrorAdd($Data["name"]." was not uploaded correctly");
		}
	}

	function GetFormattedSize($Size)
	{
		if($Size > 1024*1024)
			return number_format($Size / (1024*1024), 1)."MB";

		if($Size > 1024)
			return number_format($Size / (1024), 1)."KB";

		return number_format($Size, 1)."B";
	}

	function EditErrorAdd($Message)
	{
		$this->EditErrors[] = $Message;
	}

	function EditErrorList()
	{
		// if there are no errors, nothing to do
		if(!sizeof($this->EditErrors))
			return;

		// pre/post/and item functions allow flexibility in displaying
		// as a list, inside a div tag, etc
		$this->EditPreErrorList();
		foreach($this->EditErrors as $Error)
			$this->EditErrorItem($Error);
		$this->EditPostErrorList();
	}

	function EditErrorItem($Error)
	{
		echo "<b>- ".$Error."</b><br />\n";
	}

	function EditSave()
	{
		$this->EditPreSave();

		$INQ = new DBInsert;
	//	$INQ->NoRun = 1;
	//	$INQ->Debug = 1;

		if ($this->EditSeq)
		{
			if ($this->EditAction == "savenew")
				$INQ->Add("", $this->EditSeq, $this->EditMaxSequence() + 1);
			else
				$INQ->Add("", $this->EditSeq, $this->DataFields[$this->EditSeq]);
		}

		foreach ($this->EditList as $Key => $Data)
		{
			if (!($Data["flag"]&EDIT_FORM) || !$this->EditSeq)
			{
				// is this field an auto-increment column?
				if($Data["flag"]&EDIT_AUTO_INC)
				{
					if($AutoInc) // if column already specified
						trigger_error("EditSave(): multiple auto-increment columns ($AutoInc, $Key)", E_USER_ERROR);

					if(!in_array($Key, $this->EditPrimaryKey))
						trigger_error("EditSave(): auto-increment $Key is not the primary key", E_USER_ERROR);

					$AutoInc = $Key;
				}

				$AddType = "";
				switch ($this->EditList[$Key]["type"])
				{
					case "date":
					case "datetime":
					case "datetime24":
					case "time":
					case "time24":
						$s = $this->DataFields[$Key];

						// determine that the date is optional or not
						if (!$s["Opt"])
						{
							$this->Dates->AddDateField($s["Y"], $s["M"], $s["D"], $s["h"], $s["m"], $s["a"]);
							if ($this->Dates->ValidDate())
								$DataItem = $this->Dates->GetDate();
							else
								$DataItem = "";
						}
						else
							$DataItem = "";
						break;
					case "file":
						$DataItem = "";
						if (is_file($_FILES[$Key]["tmp_name"]))
						{
							$DataItem .= $_FILES[$Key]["type"]."|";
							$fp = fopen($_FILES[$Key]["tmp_name"], "r");
							$DataItem .= base64_encode(gzcompress(fread($fp, $_FILES[$Key]["size"])));
						}
						break;

					case "password":
						// if there is a new password value, we need to encrypt and save it
						if($this->DataFields[$Key])
						{
							$DataItem = "ENCRYPT('".$this->DataFields[$Key]."')";
							$AddType = "FUNCTION";
						}
						else
							continue 2; // if there is no value, don't save this field
						break;

					case "callback":
					case "select":
					case "check":
					case "text":
					default:
						$DataItem = $this->DataFields[$Key];
						break;
				}
				if ($Data["flag"]&EDIT_GROUP)
					$INQ->Add("", $Key, $DataItem);
				elseif(in_array($Key, $this->EditPrimaryKey))
					$INQ->Add("", $Key, $DataItem);
				elseif (!($Data["flag"]&EDIT_NOSAVE))
					if (!($this->EditList[$Key]["type"] == "file" && $DataItem == ""))
						$INQ->Add($AddType, $Key, $DataItem);
			}
		}

		if ($this->EditAction == "savenew")
		{
			$INQ->Run($this->EditDB, $this->EditTable, "INSERT");

			if($AutoInc)
				$this->DataFields[$AutoInc] = $INQ->GetInsertId();
		}
		else
		{
			foreach ($this->DataPrimary as $Key => $Val)
				$INQ->AddWhere("", $Key, $Val);
			$INQ->Run($this->EditDB, $this->EditTable, "UPDATE");
		}
		$INQ->EndQuery();
		$this->EditPostSave();

		if($this->EditSeq)
		{
			// we must populate the DataGroups array in order to perform sequencing
			foreach($this->EditGroups as $Key)
				$this->DataGroups[$Key] = $this->DataFields[$Key];
			$this->EditFetch();
			$this->EditResetSequence();
		}

		$Row = array();
		foreach ($this->EditGroups as $Key)
			$Row["DataGroups[".$Key."]"] = $this->DataFields[$Key];
		Header("Location: ".$this->EditURL($Row));
		exit;
	}

	function MakeDelete($Data)
	{
		$Query = "delete from ".$this->EditTable;
		$Join = " where ";
		foreach ($Data as $Key => $Val)
		{
			$Query .= $Join;
			$Query .= $Key." = '".addslashes($Val)."'";
			$Join = " and ";
		}
//echo "<br />".$Query;
		DB_DoQuery($this->EditDB, $Query);
	}

	function EditMaxSequence()
	{
		$Query  = "";
		$Query .= "select max(".$this->EditSeq.") as max from ";
		$Query .= $this->EditTable;
		$Join = " where ";
		foreach ($this->EditGroups as $Key)
		{
			$Query .= $Join;
			$Query .= $Key;
			$Query .= " = ";
//			$Query .= "'".$this->DataFields[$Key]."'";
			$Query .= "'".$this->DataGroups[$Key]."'";
			$Join = " and ";
		}
		$Q = new DBQuery;
		$Q->RunOne($this->EditDB, $Query);
		$Max = $Q->GetField("max");
		$Q->EndQuery;
		return ($Max * 1);
	}

	function GetURL($URL, $Vars = "")
	{
		if (strstr($URL, "?"))
			$Join = "&";
		else
			$Join = "?";
		if (is_array($Vars))
			foreach ($Vars as $Key => $Val)
			{
				$URL .= $Join;
				$URL .= $Key;
				$URL .= "=";
				$URL .= addslashes($Val);
				$Join = "&";
			}
		return ($URL);
	}

	function EditURL($Vars = "")
	{
		return ($this->GetURL($this->EditURL, $Vars));
	}

	function BackURL($Vars = "")
	{
		if (trim($this->BackURL))
			return ($this->GetURL($this->BackURL, $Vars));
		else
			return($this->EditURL($Vars));
	}

	function EditFieldCreate($Key, $Type = "DataFields")
	{
		$Name = $Type."[".$Key."]";
		$DataUse = $this->$Type;

		if (is_array($this->EditList[$Key]["years"]))
			$this->Dates->GetDateSelect("year", $this->EditList[$Key]["years"][0], $this->EditList[$Key]["years"][1]);
		else
			$this->Dates->GetDateSelect("year", date("Y"), date("Y"));
		$Value = isset($DataUse[$Key]) ? $DataUse[$Key] : $this->EditList[$Key]["default"];

		// modify the value 
		$Value = $this->EditDataEdit($Key, $Value);

		// Determine Optional flag
		$Optional = $this->EditList[$Key]["optional"];
		
		switch ($this->EditList[$Key]["type"])
		{
			case "callback":
				$Url = "index.htm?Action=".$this->EditList[$Key]["action"];
				if (is_array($this->EditList[$Key]["additional_url_args"]))
					foreach ($this->EditList[$Key]["additional_url_args"] as $N=>$V)
						$Url .= "&".$N."=".$V;
				$SpanName = "DataFields[".$Key."_span]";
				$SpanValue = $this->EditList[$Key]["span_default"];
				$Width = $this->EditList[$Key]["width"];
				$Height = $this->EditList[$Key]["height"];
				$Label = $this->EditList[$Key]["label"];
				$Field = $this->EditList[$Key]["cb_func"]->Activate($Url, array($Name=>$Value), array($SpanName=>$SpanValue), $Width, $Height, $Label);
				break;
			case "file":
				$Field = "";
				$Parts = explode("|", $Value);
				if (sizeof($Parts) == 2)
				{
					$Field .= "Current file: ";
					$Field .= number_format(strlen(gzuncompress(base64_decode($Parts[1]))));
					$Field .= " byte ";
					$Field .= "<b>".$Parts[0]."</b>";
					$Field .= " file.<br />";
				}
				else
					$Field .= "No current file attached.<br />";
				$Field .= Field::GetSmartFile($Key);
				break;
			case "select":
				$Field = Field::GetSmartDrop($Name, $Value,
						$this->EditList[$Key]["select"]);
				break;
			case "check":
				if (in_array("$Value", array("1","t","true")))
					$Value = 1;
				else
					$Value = 0;
				$Field = Field::GetSmartCheck($Name, $Value, 1);
				break;
			case "text":
				$Field = Field::GetSmartArea($Name, $Value, 32, 5);
				break;
			case "date":
				$Control = array(	$Name."[Y]", $Name."[M]", $Name."[D]");
				$Field = $this->Dates->CreateDateField($Value, "month_short", "", $Control);
				if ($Optional)
					$Field .= "&nbsp;&nbsp;".Field::GetSmartCheck($Name."[Opt]", 0, 1, "Ignore");
				break;
			case "time":
				$Control = array(	"", "", "",
							$Name."[h]", $Name."[m]", $Name."[a]");
				$Field = $this->Dates->CreateDateField($Value, "month_short", "hour_12", $Control);
				if ($Optional)
					$Field .= "&nbsp;&nbsp;".Field::GetSmartCheck($Name."[Opt]", 0, 1, "Ignore");
				break;
			case "time24":
				$Control = array(	"", "", "",
							$Name."[h]", $Name."[m]", $Name."[a]");
				$Field = $this->Dates->CreateDateField($Value, "month_short", "hour_24", $Control);
				if ($Optional)
					$Field .= "&nbsp;&nbsp;".Field::GetSmartCheck($Name."[Opt]", 0, 1, "Ignore");
				break;
			case "datetime":
				$Control = array(	$Name."[Y]", $Name."[M]", $Name."[D]",
							$Name."[h]", $Name."[m]", $Name."[a]");
				$Field = $this->Dates->CreateDateField($Value, "month_short", "hour_12", $Control);
				if ($Optional)
					$Field .= "&nbsp;&nbsp;".Field::GetSmartCheck($Name."[Opt]", 0, 1, "Ignore");
				break;
			case "datetime24":
				$Control = array(	$Name."[Y]", $Name."[M]", $Name."[D]",
							$Name."[h]", $Name."[m]", $Name."[a]");
				$Field = $this->Dates->CreateDateField($Value, "month_short", "hour_24", $Control);
				if ($Optional)
					$Field .= "&nbsp;&nbsp;".Field::GetSmartCheck($Name."[Opt]", 0, 1, "Ignore");
				break;
			case "password":
//				$Field = Field::GetPassword($Name, 16, 0, "value=\"".$Value."\"");
				$Field = Field::GetPassword($Name, 16, 0);
				if($this->EditAction == "edit")
					$Field .= "<br /><span style=\"font-size: 0.8em\">You may enter a new password here</span>";
				break;
			default: // char, num
				$Size = ($this->EditList[$Key]["size"] ? $this->EditList[$Key]["size"] : 16);
				$Field = Field::GetSmartText($Name, $Value, $Size);
				break;
		}
		return ($Field);
	}
}

function ShowEditDocument($DB, $Query, $Field, $Name = "")
{
	$Q = new DBQuery;
	$Q->RunOne($DB, $Query);
	$Parts = explode("|", $Q->GetField($Field));
	$Q->EndQuery();
	$Data = gzuncompress(base64_decode($Parts[1]));
	Header("Content-type: ".$Parts[0]);
	if ($Name)
	{
		Header("Content-Length: ".strlen($Data));
		Header("Content-Disposition: attachment; filename=".$Name);
	}
	echo $Data;
}

?>
