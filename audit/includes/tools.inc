<?php


class tools
{
	function GetStateDrop($AddBlank = false)
	{
		$Q = new DBQuery();

		$Q->Run(DB, "SELECT * FROM states");

		$States = array();

		if ($AddBlank)
			$States[''] = '';

		while ($Q->Next())
		{
			$States[$Q->GetField("state_code")] = $Q->GetField("state");	
		}

		$Q->EndQuery();

		return $States;
	}

	function EmailLink($Link, $Email)
	{
		return "<a href='mailto:$Email'>$Link</a>";
	}

	function GetAutocompleteOperatorSearchJS($Location)
	{
		$JS = '<script type="text/javascript" src="js/jquery-ui-1.8.2.custom.min.js"></script>'."\n";
		$JS .= '<script type="text/javascript">'."\n";
		$JS .= "$(function() {\n";

		$JS .= "function load(id) {\n";
		$JS .= "	window.location = '$Location&OperatorID=' + id;\n";
		$JS .= "}\n";
		$JS .= "\n";
		$JS .= "$(\"#legal_name_search\").autocomplete({\n";
//		$JS .= "	source: \"".main::URL("audit|operator_search")."\",\n";
		$JS .= "	source: \"op_search.php\",\n";
		$JS .= "	minLength: 2,\n";
		$JS .= "	select: function(event, ui) {\n";
		$JS .= "		load(ui.item.id);\n";
		$JS .= "		}\n";
		$JS .= "	});\n";
		$JS .= "});\n";

		$JS .= "</script>\n";
;

		return $JS;
	}

	function GetAutocompleteOperatorSearch()
	{
		$String = "<div class='ui-widget'><label for='legal_name_search'>Name: </label><input id='legal_name_search'></div>";

		return $String;
	}

	function GetAutocompleteContactSearchJS($Location)
	{
		$JS = '<script type="text/javascript" src="js/jquery-ui-1.8.2.custom.min.js"></script>'."\n";
		$JS .= '<script type="text/javascript">'."\n";
		$JS .= "$(function() {\n";

		$JS .= "function load(id) {\n";
		$JS .= "	window.location = '$Location&OperatorID=' + id;\n";
		$JS .= "}\n";
		$JS .= "\n";
		$JS .= "$(\"#contact_search\").autocomplete({\n";
//		$JS .= "	source: \"".main::URL("audit|operator_search")."\",\n";
		$JS .= "	source: \"contact_search.php\",\n";
		$JS .= "	minLength: 2,\n";
		$JS .= "	select: function(event, ui) {\n";
		$JS .= "		load(ui.item.id);\n";
		$JS .= "		}\n";
		$JS .= "	});\n";
		$JS .= "});\n";

		$JS .= "</script>\n";
;

		return $JS;
	}
	function GetAutocompleteContactSearch()
	{
		$String = "<div class='ui-widget'><label for='contact_search'>Contact: </label><input id='contact_search'></div>";

		return $String;
	}

	function GetAutocompleteSuburbSearchJS($SuburbField, $StateField, $PostcodeField)
	{
		$JS = '<script type="text/javascript" src="js/jquery-ui-1.8.2.custom.min.js"></script>'."\n";
		$JS .= '<script type="text/javascript">'."\n";
		$JS .= "$(function() {\n";

		$JS .= "function load(suburb, state, postcode) {\n";
		$JS .= "	document.getElementById('$SuburbField').value = suburb;\n";
		$JS .= "	document.getElementById('$StateField').value = state;\n";
		$JS .= "	document.getElementById('$PostcodeField').value = postcode;\n";
		$JS .= "}\n";
		$JS .= "\n";
		$JS .= "$(\"#$SuburbField\").autocomplete({\n";
//		$JS .= "	source: \"".main::URL("audit|operator_search")."\",\n";
		$JS .= "	source: \"suburb_search.php\",\n";
		$JS .= "	minLength: 2,\n";
		$JS .= "	select: function(event, ui) {\n";
		$JS .= "		load(ui.item.suburb, ui.item.state, ui.item.postcode);\n";
		$JS .= "		}\n";
		$JS .= "	});\n";
		$JS .= "});\n";

		$JS .= "</script>\n";
;

		return $JS;
	}

	function GetSuburb($Term)
	{
		$Q = new DBQuery();

		$Q->Run(DB, "SELECT suburb, state, postcode FROM suburbs WHERE suburb LIKE '%$Term%'");

		$List = array();

		while ($Q->Next())
		{
			$List[] = array("label" => $Q->GetField("suburb"),
							"suburb" => $Q->GetField("suburb"),
							"state" => $Q->GetField("state"),
							"postcode" => $Q->GetField("postcode"));
		}

		return $List;
	}

	function GetFirstStage($Schedule)
	{
		$CurrentStage = DBRow(DB, "SELECT * FROM stages WHERE schedule = '$Schedule' AND sequence = 1");

		return $CurrentStage;
	}

	function GetNextStage($StageID)
	{
		if (!$StageID)
			return 0;
		$CurrentStage = DBRow(DB, "SELECT * FROM stages WHERE stage_id = $StageID");

		if ($CurrentStage["alternate_next_stage"])
		{
			$NextStage = DBRow(DB, "SELECT * FROM stages WHERE schedule = '".$CurrentStage["schedule"]."' AND stage_id = ".$CurrentStage["alternate_next_stage"]);
		}
		else
		{
			$NextSequence = $CurrentStage["sequence"] + 1;

			$NextStage = DBRow(DB, "SELECT * FROM stages WHERE schedule = '".$CurrentStage["schedule"]."' AND sequence = $NextSequence");
		}

		// Advance unless we are at the last record for this schedule
		if (is_array($NextStage))
			return $NextStage;
		else
			return $CurrentStage;
	}

	function GetStageDesc($StageID)
	{
		if (!$StageID)
			return "Error";
		return DBGet(DB, "SELECT description FROM stages WHERE stage_id = $StageID");
	}

	function MoveCursorJS()
	{
		$Script = "<script type='text/javascript'>\n";
		$Script .= "function moveOnMax(field,nextFieldID)\n";
		$Script .= "{\n";
		$Script .= "	if(field.value.length >= field.maxLength)\n";
		$Script .= "	{\n";
		$Script .= "		document.getElementById(nextFieldID).focus();\n";
		$Script .= "	}\n";
		$Script .= "}\n";
		$Script .= "</script>\n";

		return $Script;
	}

	function ABNField($ABN = "")
	{
		$T = new Table();

		// Break up the abn for display
		if ($ABN)
			$ABNArray = explode(" ", $ABN);
		else
			$ABNArray = array();

		$T->AddData(field::GetText("FormData[abn][0]", $ABNArray[0], 2, 2, "onkeyup=\"moveOnMax(this, 'abn1')\""));
		$T->AddData(field::GetText("FormData[abn][1]", $ABNArray[1], 3, 3, "id=\"abn1\" onkeyup=\"moveOnMax(this, 'abn2')\""));
		$T->AddData(field::GetText("FormData[abn][2]", $ABNArray[2], 3, 3, "id=\"abn2\" onkeyup=\"moveOnMax(this, 'abn3')\""));
		$T->AddData(field::GetText("FormData[abn][3]", $ABNArray[3], 3, 3, "id=\"abn3\""));
		$T->EndRow();

		return $T->Get();
	}

	function PhoneNumberField($FieldName = "phone", $PhoneNumber = "")
	{
		$T = new Table();

		// Break up the phone number for display
		if ($PhoneNumber)
			$PhoneNumberArray = explode(" ", $PhoneNumber);
		else
			$PhoneNumberArray = array();

		$T->AddData(field::GetText("FormData[$FieldName][0]", $PhoneNumberArray[0], 4, 4, "onkeyup=\"moveOnMax(this, '".$FieldName."_body')\""));
		$T->AddData(field::GetText("FormData[$FieldName][1]", $PhoneNumberArray[1], 4, 4, "id=\"".$FieldName."_body\""));
		$T->AddData(field::GetText("FormData[$FieldName][2]", $PhoneNumberArray[2], 4, 4, "id=\"".$FieldName."_body\""));
		$T->EndRow();

		return $T->Get();
	}

	function GetUsersDrop($AddBlank = false)
	{
		$Q = new DBQuery();

		$Q->Run(DB, "SELECT * FROM users WHERE active = 1");

		$Users = array();

		if ($AddBlank)
			$Users[''] = '';

		while ($Q->Next())
		{
			$Users[$Q->GetField("user_id")] = $Q->GetField("name");	
		}

		$Q->EndQuery();

		return $Users;
	}

	function GetDatePopupJS($Name)
	{
		echo "<script type='text/javascript' src='js/jquery-ui-1.8.2.custom.min.js'></script>\n";
		echo "<script type='text/javascript'>\n";
		echo "$(function() {\n";
		echo "  $('#$Name').datepicker({dateFormat: 'dd/mm/yy', changeMonth: true, changeYear: true, showOtherMonths: true, selectOtherMonths: true});\n";
		echo "});\n";
		echo "</script>\n";
	}
}

?>
