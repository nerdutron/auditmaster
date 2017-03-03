<?php
/*
Copyright: MU Systems Pty Ltd, 2004

Modifications: 
26.11.2004 - Tim Cromwell
Created functions, Get/PutFormEnterTrap and GetFormIEEnterTrap so as to trap the enter key within a form
*/


require_once(PHPLIB_DIR."browser.inc");

class Field
{
	//////////////////////////////////////////////////
	// SUPPORTING FUNCTIONS				//
	//////////////////////////////////////////////////

	function DoubleClickTrap($Delay = 5)
	{
		define("FORM_CLICK_TRAP", 1);

		echo "<script language=\"JavaScript\">\n";
		echo "<!-- Please keep copyright lines in place without changes.\n";
		echo "//   Copyright 2000 William and Mari Bontrager\n";
		echo "//   Copyright 2003 Bontrager Connection, LLC\n";
		echo "//\n";
		// For more information and instructions, please
		//    see the 'Double Click Trapper' article at
		//    http://willmaster.com/possibilities/archives

		// When the form's submit button is clicked a second
		//    time, the click trap is set. Here, specify how
		//    many seconds the trap exist before it's released.

		echo "TrapClicksForHowManySeconds = ".$Delay.";\n\n";

		// If the user clicks the submit button and causes a
		//    trap, or clicks when a trip is set, you may spawn
		//    an alert box with a message. If you do want to
		//    spawn the alert box, specify the message between
		//    the quotation marks. Otherwise, leave it blank.
		//(Blank is two consecutive quotation mark characters,
		//    with nothing between them.)
		// Note: If you use any quotation marks in the message
		//       itself, those must be preceeded with a back-slash
		//       character. Example: 'My name is \'tall\', okay?'

		echo "AlertBoxMessage = \"Please use a single mouse click in all circumstances. Click 'OK' to continue.\";\n\n";

		// If the user clicks the submit button and causes a trap,
		//    or clicks when a trip is set, you may spawn a popup
		//    box. If you want to spawn the popup box, specify
		//    the URL of the web page to insert into the popup.
		//    Otherwise, leave it blank. You may also specify
		//    the height of the popup and the width of the popup.

		echo "PopUpBoxURL = \"\";\n";
		echo "PopUpBoxHeight = 300;\n";
		echo "PopUpBoxWidth = 500;\n\n";

		// If you want the submit button to change its text when
		//    it is clicked, specify the name you assigned to the
		//    form, the name you assigned to the submit button
		//    form field, and the new text for the submit button.
		//    Otherwise, leave at least one of these blank.

		echo "NameOfForm = \"\";\n";
		echo "NameOfSubmitButton = \"\";\n";
		echo "NewTextForSubmitButton = \"\";\n\n";

		// ////////////////////////////////////////// //
		// NO CUSTOMIZATION REQUIRED BELOW THIS POINT //
		// ////////////////////////////////////////// //

		echo "TrapTime = 0;\n";
		echo "DoubleClickTrapperCounter = 0;\n\n";

		echo "function DoubleClickTrapperAction() {\n";
		echo "	DoubleClickTrapperCounter++;\n";
		echo "	DoubleClickTrapperButton();\n";
		echo "	var trapTheClick = false;\n";
		echo "	if(DoubleClickTrapperCounter > 1 ) { trapTheClick = true; }\n";
		echo "	if(trapTheClick == true) {\n";
		echo "		if(TrapTime == 0) {\n";
		echo "			var tDate = new Date;\n";
		echo "			TrapTime = tDate.valueOf();\n";
		echo "		}\n";
		echo "		else {\n";
		echo "			var tDate = new Date;\n";
		echo "			var localTrapTime = tDate.valueOf();\n";
		echo "			if((localTrapTime - TrapTime) > (TrapClicksForHowManySeconds * 1000)) {\n";
		echo "				TrapTime = 0;\n";
		echo "				trapTheClick = false;\n";
		echo "				DoubleClickTrapperCounter = 0;\n";
		echo "			}\n";
		echo "		}\n";
		echo "	}\n";
		echo "	var valueToReturn = true;\n";
		echo "	if(trapTheClick == true) {\n";
		echo "		valueToReturn = false;\n";
		echo "		DoubleClickTrapperPopUp();\n";
		echo "		DoubleClickTrapperAlert();\n";
		echo "	}\n";
		echo "	return valueToReturn;\n";
		echo "} // end of function DoubleClickTrapperAction()\n\n";

		echo "function DoubleClickTrapperButton() {\n";
		echo "	var formname   = StripSpaces(NameOfForm);\n";
		echo "	var submitname = StripSpaces(NameOfSubmitButton);\n";
		echo "	var newtext    = StripSpaces(NewTextForSubmitButton);\n";
		echo "	if(formname.length > 0 && submitname.length > 0 && newtext.length > 0) {\n";
		echo "		var s = 'document.' + NameOfForm + '.' + NameOfSubmitButton + \".value = '\" + NewTextForSubmitButton + \"'\";\n";
		echo "		eval(s);\n";
		echo "	}\n";
		echo "} // end of function DoubleClickTrapperButton()\n\n";

		echo "function DoubleClickTrapperPopUp() {\n";
		echo "	var url = StripSpaces(PopUpBoxURL);\n";
		echo "	if(url.length > 0) {\n";
		echo "		if(PopUpBoxHeight < 1) { PopUpBoxHeight = 200; }\n";
		echo "		if(PopUpBoxWidth < 1) { PopUpBoxWidth = 300; }\n";
		echo "		window.open(url,'',('height=' + PopUpBoxHeight + ',width=' + PopUpBoxWidth + ',resizable=yes,scrollbars=yes'));\n";
		echo "	}\n";
		echo "} // end of function DoubleClickTrapperPopUp()\n\n";

		echo "function DoubleClickTrapperAlert() {\n";
		echo "	var message = StripSpaces(AlertBoxMessage);\n";
		echo "	if(message.length > 0) { alert(AlertBoxMessage); }\n";
		echo "} // end of function DoubleClickTrapperAlert()\n\n";

		echo "function StripSpaces(s) {\n";
		echo "	while(s.indexOf(' ') == 0) { s = s.substr(1); }\n";
		echo "	return s;\n";
		echo "} // end of function StripSpaces()\n\n";

		echo "// -->\n";
		echo "</script>\n\n";
	}

	//
	// resizing the navigator cells
	//
	function NavigatorCell($S)
	{
		if (BrowserCheck("mozilla", 5))
			return ($S);
		if (BrowserCheck("mozilla", 3))
		{
			$S = round ($S * 0.63);
			if ($S < 2)
				$S = 2;
		}
		return ($S);
	}

	//
	// when displaying hidden fields, do we show the label only, or label with field?
	//
	function FieldHide($Text, $Name, $Value, $Hide)
	{
		$Text = nl2br(str_replace(array("&", "<", ">"), array("&amp;", "&lt;", "&gt;"), $Text));
		if ($Hide == 2)
			return($Text);
		else
			return ($Text."&nbsp;".field::GetHidden($Name, $Default));
	}

	//
	// field tab order element
	//
	function Order($Order)
	{
		if ($Order === "")
			return "";
		return " tabindex=\"$Order\"";
	}

	//
	// calculate the actual value of a variable name passed as a string
	// variables in multi-dimensional arrays are supported (with no depth limit)
	// the following examples are supported:
	// "VarName"
	// "ArrayName[0]"
	// "ArrayName[FieldName]"
	// "ArrayName[0][FieldName]"
	// and so on...

	function ConvertVarString($String)
	{
		$Val = $_REQUEST;
		$Parts = explode("[", str_replace("]", "", $String));
		foreach ($Parts as $Key)
			$Val = $Val[$Key];
		return ($Val);
	}

	//
	// returns true or false if a field's global variable is set or not
	//
	function CheckSet($Name)
	{
		$NameParts = explode("[", str_replace("]", "", $Name));
		$GlobalSub = implode("\"][\"", $NameParts);
		$GlobalName = "\$_REQUEST[\"".$GlobalSub."\"]";
	
		$CodeString = "\$Return = isset(".$GlobalName.");";
		eval($CodeString);
	
		return $Return;
	}

	//
	// returns submitted value (inside the variable <$FieldName>) if variable <$FieldName> exists.
	// otherwise, it returns $DefaultVal
	//
	function ChooseValue($FieldName, $DefaultVal, $CheckboxValue = "")
	{
		if(substr($FieldName, -2) == "[]")
		{
			$BaseName = substr($FieldName, 0, -2);

			if(field::CheckSet($BaseName))
			{
				$BaseData = field::ConvertVarString($BaseName);
				if(is_array($BaseData) && in_array($CheckboxValue, $BaseData))
					return $CheckboxValue;
				else
					return "";
			}

			return $DefaultVal;
		}

		return field::CheckSet($FieldName) ? field::ConvertVarString($FieldName) : $DefaultVal;
	}

	function GetFormStart($Action, $File = 0, $Extra = "", $Trap = 0)
	{
		$DoTrap = 0;
		$Default = 0;
		if (defined("FORM_TRAP"))
			if (FORM_TRAP)
				$Default = 1;
		if ($Trap xor $Default)
			$DoTrap = 1;
		$Return  = "";
		$Return .= "<form";
		$Return .= " action=\"".$Action."\"";
		$Return .= " method=\"post\"";
		if ($File)
			$Return .= " enctype=\"multipart/form-data\"";
		$Return .= " ".$Extra;
		if (BrowserCheck("msie") && $DoTrap)
			$Return .= " onkeydown=\"if (event.keyCode == 13) { return false; }\"";
		$Return .= ">";
		if (!BrowserCheck("msie") && $DoTrap)
			$Return .= "<input type=\"submit\" name=\"submitfalse".rand(1,9999)."\" value=\"SubmitFalse\" onclick=\"return false;\" style=\"display:none;\" />";
		$Return .= "\n";
		return ($Return);
	}

	function GetFormEnd()
	{
		return ("</form>\n");
	}

	function GetFormSubmit($Label, $Name = "", $Extra = "")
	{
		$Return  = "";
		$Return .= "<input";
		$Return .= " type=\"submit\"";
		if ($Name)
			$Return .= " name=\"".$Name."\"";
		$Return .= " value=\"".$Label."\"";
		$Return .= " ".$Extra;

		if (defined("FORM_CLICK_TRAP"))
		{
			$Return .= " onClick=\"return DoubleClickTrapperAction();\"";
		}
		$Return .= " />\n";
		return ($Return);
	}

	function GetFormButton($Label, $Name = "", $Extra = "")
	{
		$Return  = "";
		$Return .= "<input";
		$Return .= " type=\"button\"";
		if ($Name)
			$Return .= " name=\"".$Name."\"";
		$Return .= " value=\"".$Label."\"";
		$Return .= " ".$Extra;
		$Return .= " />\n";
		return ($Return);
	}

	function GetFormReset($Label, $Extra = "")
	{
		$Return  = "";
		$Return .= "<input";
		$Return .= " type=\"reset\"";
		$Return .= " value=\"".$Label."\"";
		$Return .= " ".$Extra;
		$Return .= " />\n";
		return ($Return);
	}

	function GetFieldsetStart($Label = "", $Extra = "")
	{
		$Return  = "";
		$Return .= "<fieldset".(($Extra)?(" ".$Extra):("")).">\n";
		if (trim($Label))
			$Return .= "<legend>".$Label."</legend>\n";
		return ($Return);
	}

	function GetFieldsetEnd()
	{
		return ("</fieldset>\n");
	}
	//////////////////////////////////////////////////
	// HIDDEN FIELD					//
	//////////////////////////////////////////////////

	//
	// returns a hidden field given a name and a value
	//
	function GetHidden($Name, $Value, $Extra = "")
	{
		$Value = str_replace(array("&", "\""), array("&amp;", "&quot;"), $Value);
		if (trim($Extra))
			$Extra = " ".trim($Extra);
		else
			$Extra = "";
		return ("<input type=\"hidden\" name=\"".$Name."\" value=\"".$Value."\"".$Extra." />\n");
	}

	//////////////////////////////////////////////////
	// PASSWORD FIELD				//
	//////////////////////////////////////////////////

	function GetPassword($Name, $Size = 0, $Max = 0, $Extra = "")
	{
		$Return = "";
		$Return .= "<input type=\"password\"";
		$Return .= " name=\"".$Name."\"";
		if ($Size)
			$Return .= " size=\"".field::NavigatorCell($Size)."\"";
		if ($Max)
			$Return .= " maxlength=".$Max;
		$Return .= " ".$Extra;
		$Return .= " />\n";
		return ($Return);
	}

	//////////////////////////////////////////////////
	// TEXT FIELD					//
	//////////////////////////////////////////////////

	//
	// renders a text field form element
	//
	function GetText($Name, $Value, $Size, $Max = 0, $Extra = "")
	{
		$Return = "";
		$Return .= "<input type=\"text\"";
		$Return .= " name=\"".$Name."\"";
		$Return .= " value=\"".str_replace(array("&", "\""), array("&amp;", "&quot;"), $Value)."\"";
		$Return .= " size=\"".field::NavigatorCell($Size)."\"";
		$Return .= " ".$Extra;
		if ($Max)
			$Return .= " maxlength=".$Max;
		$Return .= " />\n";
		return ($Return);
	}

	function GetSmartText($Name, $Default, $Size, $Max = 0, $Extra = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		if ($Reset == 0)
			$Default = field::ChooseValue($Name, $Default);
		if ($Hide)
			return(field::FieldHide($Default, $Name, $Default, $Hide));
		$Extra .= field::Order($Order);
		return (field::GetText($Name, $Default, $Size, $Max, trim($Extra)));
	}

	//////////////////////////////////////////////////
	// AREA FIELD					//
	//////////////////////////////////////////////////

	//
	// renders a text area field form element
	//
	function GetArea($Name, $Value, $Cols, $Rows, $Extra = "")
	{
		$Return = "";
		$Return .= "<textarea";
		$Return .= " name=\"".$Name."\"";
		$Return .= " cols=\"".field::NavigatorCell($Cols)."\"";
		$Return .= " rows=\"".$Rows."\"";
		$Return .= " wrap=\"virtual\"";
		$Return .= " ".$Extra;
		$Return .= ">";
		$Return .= str_replace(array("&", "<", ">"), array("&amp;", "&lt;", "&gt;"), $Value);
		$Return .= "</textarea>\n";
		return ($Return);
	}
 
	function GetSmartArea($Name, $Default, $Cols, $Rows, $Extra = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		if ($Reset == 0)
			$Default = field::ChooseValue($Name, $Default);
		if ($Hide)
			return(field::FieldHide($Default, $Name, $Default, $Hide));
		$Extra .= field::Order($Order);
		return (field::GetArea($Name, $Default, $Cols, $Rows, trim($Extra)));
	}

	//////////////////////////////////////////////////
	// UPLOAD FIELD					//
	//////////////////////////////////////////////////

	//
	// renders a text field form element
	//
	function GetFile($Name, $Extra = "")
	{
		$Return = "";
		$Return .= "<input type=\"file\"";
		$Return .= " name=\"".$Name."\"";
		$Return .= " ".$Extra;
		$Return .= " />\n";
		return ($Return);
	}

	function GetSmartFile($Name, $Extra = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		if ($Reset == 0)
			$Default = $_FILES[$Name]["name"];
		if ($Hide)
			return ("");
		$Extra .= field::Order($Order);
		return (field::GetFile($Name, trim($Extra)));
	}

	//////////////////////////////////////////////////
	// RADIO FIELD					//
	//////////////////////////////////////////////////

	//
	// renders a radio select field form element
	//
	function GetRadio($Name, $Select, $Values, $Cols = 0, $Extra = "")
	{
		$Return = "";
	
		$i = 0;
		if ($Cols == 0)
			$Cols = sizeof($Values) + 1;
		if (is_array($Values))
		{
			foreach ($Values as $Data => $Label)
			{
				$i++;
				$Return .= "<input type=\"radio\"";
				$Return .= " name=\"".$Name."\"";
				$Return .= " value=\"".$Data."\"";
				if ($Data == $Select)
					$Return .= " checked";
				$Return .= " ".$Extra;
				$Return .= " />\n";
				$Return .= $Label."&nbsp; ";
				if ($i == $Cols)
				{
					$Return .= "<br />";
					$i = 0;
				}
			}
		}
	
		return ($Return);
	}

	function GetSmartRadio($Name, $Select, $Values, $Cols = 0, $Extra = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		if ($Reset == 0)
			$Select = field::ChooseValue($Name, $Select);
		if ($Hide)
			return(field::FieldHide($Values[$Select], $Name, $Select, $Hide));
		$Extra .= field::Order($Order);
		return (field::GetRadio($Name, $Select, $Values, $Cols, trim($Extra)));
	}

	function GetDatabaseRadio($DB, $ListID, $Name, $Select, $Cols = 0, $Extra = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		$Values = field::GetListArray($DB, $ListID);
		return field::GetSmartRadio($Name, $Select, $Values, $Cols, $Extra, $Reset, $Hide, $Order);
	}

	//////////////////////////////////////////////////
	// CHECK FIELD(s)				//
	//////////////////////////////////////////////////

	//
	// renders a check box field form element
	//
	function GetCheck($Name, $Status, $Value, $Label = "", $Extra = "")
	{
		$Return = "";
		if(!strstr($Name, "[]"))
			$Return .= field::GetHidden($Name, 0);
		$Return .= "<input type=\"checkbox\"";
		$Return .= " name=\"".$Name."\"";
		$Return .= " value=\"".$Value."\"";
		$Return .= " ".$Extra;
		if ("$Status" == "$Value")
			$Return .= " checked";
		$Return .= " />\n";
		$Return .= $Label;

		return ($Return);
	}

	function GetSmartCheck($Name, $DefaultStatus, $Value, $Label = "", $Extra = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		if ($Reset == 0)
			$DefaultStatus = field::ChooseValue($Name, $DefaultStatus, $Value);

		if ($Hide)
		{
			if ("$DefaultStatus" == "$Value")
				return(field::FieldHide($Label, $Name, ("$DefaultStatus" == "$Value" ? $Value : 0), $Hide));
			return ("&nbsp;");
		}
		$Extra .= field::Order($Order);
		return (field::GetCheck($Name, $DefaultStatus, $Value, $Label, trim($Extra)));
	}

	//
	// renders multiple check box field form elements
	//
	function GetSmartChecks($Name, $DefaultChecked, $Values, $Extra = "", $Format = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		// $Format:
		// [0] = echo before the checkbox
		// [1] = echo between the checkbox and the label
		// [2] = echo between the checkbox and the next label
		// [3] = number of checkboxes on each line
		if (!is_array($Format))
			$Format = array();
		$Return = "";

		// if the field name ends in [], we need to show a hidden field to ensure that if no checkboxes are ticked,
		// we will have a value of 0
		if(substr($Name, -2) == "[]")
			$Return .= field::GetHidden(substr($Name, 0, -2), 0);

		$i = 0;
		foreach ($Values as $Value => $Label)
		{
			$DefaultStatus = $DefaultChecked[$Value];

			if($Reset == 0)
				$DefaultStatus = field::ChooseValue($Name, $DefaultStatus, $Value);

			if(!$Hide || "$DefaultStatus" == "$Value")
				$Return .= $Format[0];

			$Return .= field::GetSmartCheck($Name, $DefaultStatus, $Value, "", $Extra, $Reset, $Hide, $Order);

			if(!$Hide || "$DefaultStatus" == "$Value")
			{
				$Return .= $Format[1];
				$Return .= $Label;
				$Return .= $Format[2];

				$i++;

				if ($Format[3] == $i)
				{
					$Return .= "<br />";
					$i = 0;
				}
			}
		}
		return ($Return);
	}

	function GetDatabaseChecks($DB, $ListID, $Name, $DefaultChecked, $Extra = "", $Format = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		$Values = field::GetListArray($DB, $ListID);
		return field::GetSmartChecks($Name, $DefaultChecked, $Values, $Extra, $Format, $Reset, $Hide, $Order);
	}

	function GetAJAXCheck($Form, $Name, $OnValue, $OffValue, $Current)
	{
		$Return  = "";
		$Return .= "<input type=\"hidden\" name=\"".$Name."\" value=\"";
		if ($Current == $OnValue)
			$Return .= $OnValue;
		else
			$Return .= $OffValue;
		$Return .= "\" />";
		$Return .= "<input type=\"checkbox\" onclick=\"this.checked ? ";
		$Return .= "document.forms['".$Form."'].elements['".$Name."'].value = '".addslashes($OnValue)."' ";
		$Return .= ": ";
		$Return .= "document.forms['".$Form."'].elements['".$Name."'].value = '".addslashes($OffValue)."' ";
		$Return .= "\" ";
		if ($Current == $OnValue)
			$Return .= "checked ";
		$Return .= "/>";
		return ($Return);
	}

	//////////////////////////////////////////////////
	// SELECT FIELDS				//
	//////////////////////////////////////////////////

	//
	// renders a drop down field form element
	//
	function GetDrop($Name, $Select, $Values, $Extra = "")
	{
		$Return = "";
		$Return .= "<select name=\"".$Name."\" ".$Extra.">\n";
	
		if (is_array($Values))
		{
			foreach ($Values as $Data => $Label)
			{
				$Return .= "<option value=\"".$Data."\"";
				if ("$Data" === "$Select")
					$Return .= " selected";
				$Return .= ">";
				$Return .= $Label;
				$Return .= "</option>\n";
			}
		}
	
		$Return .= "</select>\n";
		return ($Return);
	}
 
	function GetSmartDrop($Name, $Select, $Values, $Extra = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		if ($Reset == 0)
			$Select = field::ChooseValue($Name, $Select);
		if ($Hide)
			return(field::FieldHide($Values[$Select], $Name, $Select, $Hide));
		$Extra .= field::Order($Order);
		return (field::GetDrop($Name, $Select, $Values, trim($Extra)));
	}

	function GetDatabaseDrop($DB, $ListID, $Name, $Select, $Extra = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		$Values = field::GetListArray($DB, $ListID);
		return field::GetSmartDrop($Name, $Select, $Values, $Extra, $Reset, $Hide, $Order);
	}

	function GetListQuery($DB, $Query)
	{
		$ItemArray = array();
		$Q = new DBQuery;
		$Q->Run($DB, $Query);
		while($Q->Next())
		{
			$Data = array_values($Q->GetArray());
			$ItemArray[$Data[0]] = $Data[1];
		}
		$Q->EndQuery();
		return($ItemArray);
	}
 
	function GetListArray($DB, $ListID)
	{
		$ItemArray = array();
		$Query = "select * from value_list where list_id = '".addslashes($ListID)."' order by sequence_nr";
		$Q = new DBQuery;
		$Q->Run($DB, $Query);
		while($Q->Next())
			$ItemArray[$Q->GetField("real_value")] = $Q->GetField("label_value");
		$Q->EndQuery();
		return($ItemArray);
	}
 
	function GetListLabel($DB, $ListID, $VarValue)
	{
		$Query = "select * from value_list where list_id = '".addslashes($ListID)."'"
			." and real_value = '".addslashes($VarValue)."'";

		$Q = new DBQuery;
		$Q->RunOne($DB, $Query);
		$Data = $Q->GetField("label_value");
		$Q->EndQuery();
		return($Data);
	}

	function GetListReal($DB, $ListID, $VarValue)
	{
		$Query = "select * from value_list where list_id = '".addslashes($ListID)."'"
			." and label_value = '".addslashes($VarValue)."'";
		$Q = new DBQuery;
		$Q->RunOne($DB, $Query);
		$Data = $Q->GetField("real_value");
		$Q->EndQuery();
		return($Data);
	}

	//////////////////////////////////////////////////
	// MAPPED VERSIONS FOR ECHOING			//
	//////////////////////////////////////////////////

	function PutFormStart($Action, $File = 0, $Extra = "", $Trap = 0)
	{
		echo field::GetFormStart($Action, $File, $Extra, $Trap);
	}

	function PutFormEnd()
	{
		echo field::GetFormEnd();
	}

	function PutFormSubmit($Label, $Name = "", $Extra = "")
	{
		echo field::GetFormSubmit($Label, $Name, $Extra);
	}

	function PutFormButton($Label, $Name = "", $Extra = "")
	{
		echo field::GetFormButton($Label, $Name, $Extra);
	}

	function PutFormReset($Label, $Extra = "")
	{
		echo field::GetFormReset($Label, $Extra);
	}

	function PutHidden($Name, $Value)
	{
		echo field::GetHidden($Name, $Value);
	}

	function PutPassword($Name, $Size = 0, $Max = 0, $Extra = "")
	{
		echo field::GetPassword($Name, $Size, $Max, $Extra);
	}

	function PutText($Name, $Value, $Size, $Max = 0, $Extra = "")
	{
		echo field::GetText($Name, $Value, $Size, $Max, $Extra);
	}

	function PutSmartText($Name, $Default, $Size, $Max = 0, $Extra = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		echo field::GetSmartText($Name, $Default, $Size, $Max, $Extra, $Reset, $Hide, $Order);
	}

	function PutArea($Name, $Value, $Cols, $Rows, $Extra = "")
	{
		echo field::GetArea($Name, $Value, $Cols, $Rows, $Extra);
	}
 
	function PutSmartArea($Name, $Default, $Cols, $Rows, $Extra = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		echo field::GetSmartArea($Name, $Default, $Cols, $Rows, $Extra, $Reset, $Hide, $Order);
	}

	function PutFile($Name, $Extra = "")
	{
		echo field::GetFile($Name, $Extra);
	}

	function PutSmartFile($Name, $Size, $Extra = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		echo field::GetSmartFile($Name, $Size, $Extra, $Reset, $Hide, $Order);
	}

	function PutRadio($Name, $Select, $Values, $Cols = 0, $Extra = "")
	{
		echo field::GetRadio($Name, $Select, $Values, $Cols, $Extra);
	}

	function PutSmartRadio($Name, $Select, $Values, $Cols = 0, $Extra = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		echo field::GetSmartRadio($Name, $Select, $Values, $Cols, $Extra, $Reset, $Hide, $Order);
	}

	function PutDatabaseRadio($DB, $ListID, $Name, $Select, $Cols = 0, $Extra = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		echo field::GetDatabaseRadio($DB, $ListID, $Name, $Select, $Cols, $Extra, $Reset, $Hide, $Order);
	}

	function PutCheck($Name, $Status, $Value, $Label = "", $Extra = "")
	{
		echo field::GetCheck($Name, $Status, $Value, $Label, $Extra);
	}

	function PutSmartCheck($Name, $DefaultStatus, $Value, $Label = "", $Extra = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		echo field::GetSmartCheck($Name, $DefaultStatus, $Value, $Label, $Extra, $Reset, $Hide, $Order);
	}

	function PutSmartChecks($Name, $DefaultChecked, $Values, $Extra = "", $Format = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		echo field::GetSmartChecks($Name, $DefaultChecked, $Values, $Extra, $Format, $Reset, $Hide, $Order);
	}

	function PutDatabaseChecks($DB, $ListID, $Name, $DefaultChecked, $Extra = "", $Format = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		echo field::GetDatabaseChecks($DB, $ListID, $Name, $DefaultChecked, $Extra, $Format, $Reset, $Hide, $Order);
	}

	function PutAJAXCheck($Form, $Name, $OnValue, $OffValue, $Current)
	{
		echo field::GetAJAXCheck($Form, $Name, $OnValue, $OffValue, $Current);
	}

	function PutDrop($Name, $Select, $Values, $Extra = "")
	{
		echo field::GetDrop($Name, $Select, $Values, $Extra);
	}
 
	function PutSmartDrop($Name, $Select, $Values, $Extra = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		echo field::GetSmartDrop($Name, $Select, $Values, $Extra, $Reset, $Hide, $Order);
	}

	function PutDatabaseDrop($DB, $ListID, $Name, $Default, $Extra = "", $Reset = 0, $Hide = 0, $Order = "")
	{
		echo field::GetDatabaseDrop($DB, $ListID, $Name, $Default, $Extra, $Reset, $Hide, $Order);
	}

	function PutListLabel($DB, $ListID, $VarValue)
	{
		echo field::GetListLabel($DB, $ListID, $VarValue);
	}

	function PutListReal($DB, $ListID, $VarValue)
	{
		echo field::GetListReal($DB, $ListID, $VarValue);
	}

	function PutFieldsetStart($Label = "", $Extra = "")
	{
		echo field::GetFieldsetStart($Label, $Extra);
	}

	function PutFieldsetEnd()
	{
		echo field::GetFieldsetEnd();
	}
}

?>
