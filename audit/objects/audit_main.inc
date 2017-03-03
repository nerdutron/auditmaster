<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: about.inc 8 2006-12-10 05:06:30Z scott                           $ //
//                                                                          //
// 	Object Model															// 
//  Copyright (C) Authors listed in AUTHORS.txt                             //
//                                                                          //
//  This program is free software; you can redistribute it and/or modify    //
//  it under the terms of the GNU General Public License as published by    //
//  the Free Software Foundation; either version 2 of the License, or       //
//  (at your option) any later version.                                     //
//                                                                          //
//  This program is distributed in the hope that it will be useful,         //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of          //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           //
//  GNU General Public License for more details.                            //
//                                                                          //
//  You should have received a copy of the GNU General Public License       //
//  along with this program; if not, write to the Free Software             //
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

require_once(ToObjects("core")."secure.inc");

require_once(ToIncludes("audit")."operator.inc");
require_once(ToIncludes("audit")."audit.inc");
require_once(ToIncludes("audit")."scheme.inc");
require_once(ToIncludes("audit")."tools.inc");
require_once(ToIncludes("audit")."audit_business.inc");

class audit_main extends secure
{
	function __construct()
	{
		parent::__construct();
		$this->Title = "Audit Main";
		$this->MakeL("OperatorID", "FormData");

		$this->Operator = new operator($this->OperatorID);
		$this->SchemesObj = new scheme();

		$this->Schemes = $this->SchemesObj->GetSchemeSignup($this->OperatorID);

		$this->AuditBusiness = new audit_business($this->User, $this->OperatorID);

		if ($this->OperatorID)
		{
			if (!is_array($this->FormData))
			{
				$this->FormData = $this->Operator->GetData();
			}
		}

		//$this->States = tools::GetStateDrop();
	}

	function Head()
	{
		parent::Head();
		echo '<script type="text/javascript" src="js/jquery.js"></script>'."\n";
		echo '<script type="text/javascript">'."\n";
		echo "$(document).ready(function(){\n";
//		echo '	$("#scheme_signup").click(function(){'."\n";
//		echo "		$('#dialog-form').dialog('open');\n";
//		echo "	});\n";
/*
		echo "	var values = new Array();\n";
		echo "	$(\"input[name='Scheme[]']:checked\").each(function(){values.push($(this).val());});\n";
		echo "	$('#audit_display').load('".$this->URL("audit|add_audits", array("OperatorID" => $this->OperatorID))."', {'Schemes[]' : values} );\n";
		echo "	$('#scheme_display').load('".$this->URL("audit|show_schemes", array("OperatorID" => $this->OperatorID))."');\n";
		echo "	});\n";
*/
		echo '	$("#save_note").click(function(){'."\n";
		echo "	$('#notes_display').load('".$this->URL("audit|save_notes", array("OperatorID" => $this->OperatorID))."', {'Note' : document.getElementById('NoteArea').value} );\n";
		echo "  	document.getElementById('NoteArea').value = '';\n";
		echo "	});\n";
		echo "	$('#dialog-form').dialog({\n";
		echo "		autoOpen: false,\n";
		echo "		height: 300,\n";
		echo "		width: 350,\n";
		echo "		modal: true,\n";
		echo "		buttons: {\n";
		echo "			'Enrol in Scheme': function() {\n";
		echo "				var opsel = $('#AddScheme').val();\n";
		echo "				$.get('".$this->URL("audit|add_scheme", array("OperatorID" => $this->OperatorID))."',\n";
		echo " 					{'AddScheme' : $('#AddScheme').val(), 'AddSchemeDate' : $('#AddSchemeDate').val()},\n";
		echo "					function(data) {\n";
		echo "						parent.location = '".$this->URL("audit|audit_main", array("OperatorID" => $this->OperatorID))."';\n";
		echo "					}\n";
		echo " 				);\n";
//		echo "				$('#audit_display').load('".$this->URL("audit|add_audits", array("OperatorID" => $this->OperatorID))."', {'AddScheme' : $('#AddScheme').val(), 'AddSchemeDate' : $('#AddSchemeDate').val()} );\n";
		echo "				$(this).dialog('close');\n";
//		echo "				$('#AddScheme option[value=opsel]').remove();\n";
		echo "			},\n";
		echo "		Cancel: function() {\n";
		echo "			$(this).dialog('close');\n";
		echo "			}\n";
		echo "		}\n";
		echo "	});\n";
		echo "	$('#dialog-expiry-form').dialog({\n";
		echo "		autoOpen: false,\n";
		echo "		height: 200,\n";
		echo "		width: 300,\n";
		echo "		modal: true,\n";
		echo "		buttons: {\n";
		echo "			'Update': function() {\n";
		echo "				var scheme_id = $('#ExpirySchemeID').val();\n";
		echo "				var scheme_expiry_date = $('#ChangeExpiryDate').val();\n";
		echo "				$.get('".$this->URL("audit|scheme_expiry", array("OperatorID" => $this->OperatorID))."',\n";
		echo " 					{'ExpirySchemeID' : scheme_id, 'ExpirySchemeDate' : scheme_expiry_date},\n";
		echo "					function(data) {\n";
		echo "						$('#expiry_date_' + $('#ExpirySchemeID').val()).text(scheme_expiry_date);\n";
		echo "					}\n";
		echo " 				);\n";
		echo "				$(this).dialog('close');\n";
		echo "			},\n";
		echo "		Cancel: function() {\n";
		echo "			$(this).dialog('close');\n";
		echo "			}\n";
		echo "		}\n";
		echo "	});\n";
		echo "});\n";
		echo "</script>\n";
		echo tools::GetAutocompleteOperatorSearchJS($this->URL("audit|audit_main"));
		echo tools::GetAutocompleteContactSearchJS($this->URL("audit|audit_main"));

		echo '<script type="text/javascript">'."\n";
		echo 'function LoadAudits() {'."\n";
		echo "	var values = new Array();\n";
		echo "	$(\"input[name='Scheme[]']:checked\").each(function(){values.push($(this).val());});\n";
		echo "	$('#audit_display').load('".$this->URL("audit|add_audits", array("OperatorID" => $this->OperatorID))."', {'Schemes[]' : values}, function() { \n";
		echo "	  $('#scheme_display').load('".$this->URL("audit|show_schemes", array("OperatorID" => $this->OperatorID))."'); } );\n";
		echo "	}\n";
		echo "</script>\n";
		echo "<script type='text/javascript'>\n";
		echo "function DeleteDialog(AuditID, Title)\n";
		echo "{\n";
		echo "	$(\"#dialog-confirm\").dialog(\n";
		echo "	{\n";
		echo "		resizable: false,\n";
		echo "		title: 'Do you wish to delete ' + Title,\n";
		echo " 		height:140,\n";
		echo "		modal: true,\n";
		echo " 		buttons: {\n";
		echo "		'Delete': function() {\n";
		echo "		$('#audit_display').load('".$this->URL("audit|delete_audit", array("OperatorID" => $this->OperatorID))."', {'AuditID' : AuditID} );\n";
		echo "			 $(this).dialog('close');\n";
		echo "		 },\n";
		echo "		Cancel: function() {\n";
		echo "			$(this).dialog('close');\n";
		echo "		}\n";
		echo " 	}\n";
		echo "});\n";
		echo "};\n";
		echo "</script>\n";
		tools::GetDatePopupJS("AddSchemeDate");
		tools::GetDatePopupJS("ChangeExpiryDate");
		echo '<script type="text/javascript">'."\n";
		echo 'function SchemeSignup() {'."\n";
		echo "	$('#dialog-form').dialog('open');\n";
		echo "}\n";
		echo 'function ShowExpiryEdit(scheme_id, scheme_name) {'."\n";
		echo "	$('#ExpirySchemeID').val(scheme_id);\n";
		echo "	$('#scheme_title').text(scheme_name);\n";
		echo "	$('#ChangeExpiryDate').val($('#expiry_date_' + scheme_id).text());\n";
		echo "	$('#dialog-expiry-form').dialog('open');\n";
		echo "}\n";
		echo "</script>\n";
		echo '<script type="text/javascript">'."\n";
		echo 'function ChangeOp(Direction) {'."\n";
		echo "	parent.location = '".$this->URL("audit|next_operator", array("OperatorID" => $this->OperatorID))."' + '&Direction=' + Direction;";
		echo "}\n";
		echo "</script>\n";

/*
		echo '<script type="text/javascript">'."\n";
		echo "$(document).ready(function(){\n";
		echo '	$("#save_note").click(function(){'."\n";
		echo "	$('#notes_display').load('".$this->URL("audit|save_notes", array("OperatorID" => $this->OperatorID))."', {'Note' : document.getElementById('NoteArea').value} );\n";
		echo "	});\n";
		echo "});\n";
		echo "</script>\n";
*/
	}

	function PageContent($Message = "")
	{
		$this->AuditBusiness = new audit_business($this->User, $this->OperatorID);
		if ($Message)
		{
			$this->ShowMessage($Message);
		}

		$this->DisplayOperator();

		if ($this->OperatorID)
		{
			$this->DisplaySchemes(true);
			$this->DisplayAudits(true);
			$this->DisplayNotesEntry(true);
		}
	}

	function DisplayOperator()
	{
		$this->Operator = new operator($this->OperatorID);

		$Details = $this->Operator->GetData();

		$T = new Table("full_width");

		$T->AddHead("<h3>Operator</h3>", 5);
		$T->AddHead("<button onClick='ChangeOp(\"Prev\")'>Previous</button>"."<button onClick='ChangeOp(\"Next\")'>Next</button>");
		$T->EndRow();
		$T->AddHead("Name:");
		$T->AddData($Details["legal_name"]);
		$T->AddHead("ABN:");
		$T->AddData($Details["abn"]);
		$T->AddHead("Address:");
		$T->AddData($Details["address_1"]." ".$Details["address_2"]."<br/>".$Details["suburb"]." ".$Details["state"]." ".$Details["postcode"], 3);
		$T->EndRow();
		$T->AddHead("Trading Name:");
		$T->AddData($Details["trading_name"]);
		$T->AddHead("Owner:");
		$T->AddData($Details["owners_name"]);
		$T->AddHead("Postal Address:");
		$T->AddData($Details["postal_address_1"]." ".$Details["postal_address_2"]."<br/>".$Details["postal_suburb"]." ".$Details["postal_state"]." ".$Details["postal_postcode"], 3);
		$T->EndRow();

		//$T->AddHead("<a href='".$this->URL("audit|operator_maintenance", array("OperatorID" => $this->OperatorID, "Return" => "audit|audit_main"))."'>Edit</a>", 2);
		$T->AddHead("<center><button id='op_maint' type='button' onClick='parent.location=\"".$this->URL("audit|operator_maintenance", array("OperatorID" => $this->OperatorID, "Return" => "audit|audit_main"))."\"'>Edit</button></center>", 3);
		$T->AddHead("<center><button id='contact_maint' type='button' onClick='parent.location=\"".$this->URL("audit|contact_maintenance", array("OperatorID" => $this->OperatorID, "Return" => "audit|audit_main"))."\"'>Add Contact</button></center>", 3);
		$T->EndRow();

		$T2 = new Table("full_width");

		$T2->AlternateRows("row0", "row1");

		$T2->AddHead("Contact Name");
		$T2->AddHead("Mobile");
		$T2->AddHead("Phone");
		$T2->AddHead("Fax");
		$T2->AddHead("Email");
		$T2->AddHead("Position");
		$T2->AddHead("");
		$T2->EndRow();

		$Contacts = $this->Operator->GetContacts();

		foreach ($Contacts as $Contact)
		{
			$Name = $Contact["firstname"]." ".$Contact["surname"];
			if ($Contact["nickname"])
				$Name .= " (".$Contact["nickname"].")";
			$T2->AddData($Name);
			$T2->AddData($Contact["mobile"]);
			$T2->AddData($Contact["phone"]);
			$T2->AddData($Contact["fax"]);
			$T2->AddData(tools::EmailLink($Contact["email"], $Contact["email"]));
			$T2->AddData($Contact["position"]);
			$T2->AddData("<center><button id='edit_contact' type='button' onClick='parent.location=\"".$this->URL("audit|contact_maintenance", array("OperatorID" => $this->OperatorID, "ContactID" => $Contact["contact_id"], "Return" => "audit|audit_main"))."\"'>Edit</button></center>", 2);
			$T2->EndRow();
		}

		$T->AddData($T2->Get(), 6);

		DivBox::PutBoxStart("op_display");
		$T->Put();
		DivBox::PutBoxEnd();
	}

	function DisplaySchemes($Drawing = false)
	{
		$Colspan = 5;

		$T = new Table();

		$T->AddHead("<h3>Schemes<h3>", $Colspan - 2);
		//$T->AddHead("<center><button id='scheme_signup' type='button'>Add New</button></center>", 2);
		$T->AddHead("<center><button id='scheme_signup' type='button' onClick='SchemeSignup()'>Add New</button></center>", $Colspan);
		$T->EndRow();

		$T->AddHead("");
		$T->AddHead("Scheme");
		$T->AddHead("Start");
		$T->AddHead("Next");
		$T->AddHead("Expiry");
		$T->AddHead("");
		$T->EndRow();

		$T->AlternateRows("row0", "row1");

		$i = 1;
		foreach ($this->Schemes as $Scheme)
		{
			//$T->AddData(field::GetCheck("Scheme[".$Scheme["scheme_id"]."].", $Scheme["scheme_id"], true));
			$T->AddData(field::GetCheck("Scheme[]", 0, $Scheme["scheme_id"]));
			$T->AddData($Scheme["scheme_name"]);

			if ($Scheme["start_date"])
			{
				$T->AddData(date(DATE_FORMAT, strtotime($Scheme["start_date"])));
				$DummyExpiry = $Scheme["expiry_date"];
				$T->AddData(tools::GetStageDesc($this->AuditBusiness->GetNextStage($Scheme["scheme_id"], $Dummy, $DummyExpiry)));
			}
			else
			{
				$T->AddData("");
				$T->AddData(tools::GetStageDesc($this->AuditBusiness->GetFirstStage($Scheme["scheme_id"])));
			}

			$ExpiryField = "<div id='expiry_".$Scheme["scheme_id"]."'><span id='expiry_date_".$Scheme["scheme_id"]."'>";

			if ($Scheme["expiry_date"])
				$ExpiryField .= date(DATE_FORMAT, strtotime($Scheme["expiry_date"]));

			$ExpiryField .= "</span>";

			$ExpiryField .= "<button id='edit_expiry_".$Scheme["scheme_id"]."' type='button' ";
			$ExpiryField .= "onClick='ShowExpiryEdit(".$Scheme["scheme_id"].", \"".$Scheme["scheme_name"]."\")'>Change Expiry</button>";
			$ExpiryField .= "</div>";
/*
			$ExpiryField .= "<div id='edit_expiry_".$Scheme["scheme_id"]."' style='display: none;'>";

			$ExpiryField .= field::GetText("scheme_update[".$Scheme["scheme_id"]."][date]", date("d/m/Y", strtotime($Scheme["expiry_date"])), 12, 12);
			$ExpiryField .= field::GetFormSubmit("Update");

			$ExpiryField .= "</div>";
*/

			$T->AddData($ExpiryField);

			$T->EndRow();

//			if ($i % 2 == 0)
//				$T->EndRow();

			$i++;
		}

		$T->EndRow();

		$T->AddHead("<center><button id='load_audits' type='button' onClick='LoadAudits()'>Load</button></center>", $Colspan);

		if ($Drawing)
			DivBox::PutBoxStart("scheme_display", "", "scheme_display");
		field::PutFormStart("schemes");
		$T->Put();
		field::PutFormEnd();
		if ($Drawing)
			DivBox::PutBoxEnd();

		echo "<div id='dialog-form' style='display: none' title='Scheme Signup'>\n";
		echo "<form>\n";
		$T = new Table();
		$T->AddHead("Scheme:");
		$T->AddData(field::GetDrop("AddScheme", "", scheme::GetUnenrolledSchemeDrop($this->OperatorID), "id='AddScheme'"));
		$T->EndRow();
		$T->AddHead("Signup Date:");
		$T->AddData(field::GetText("AddSchemeDate", date("d/m/Y"), 12, 12, "id='AddSchemeDate'"));
		$T->EndRow();
		$T->Put();
		echo "</form>\n";
		echo "</div>\n";

		echo "<div id='dialog-expiry-form' style='display: none' title='Scheme Expiry'>\n";
		echo "<form>\n";
		$T = new Table();
		$T->AddHead("Scheme:");
		$T->AddData("<span id='scheme_title'></span>");
		$T->EndRow();
		$T->AddHead("Expiry Date:");
		$T->AddData(field::GetText("ChangeExpiryDate", date("d/m/Y"), 12, 12, "id='ChangeExpiryDate'").
			field::GetHidden("ExpirySchemeID", "", "id='ExpirySchemeID'"));
		$T->EndRow();
		$T->Put();
		echo "</form>\n";
		echo "</div>\n";
	}

	function DisplayAudits($Drawing = false)
	{
		$T = new Table("full_width");

		$T->AddHead("<h3>Audits</h3>", 11);
		$T->EndRow();

		$T->AddHead("Due Date");
		$T->AddHead("Auditor Name");
		$T->AddHead("Scheme");
		$T->AddHead("Stage");
		$T->AddHead("Net Audit<br/>(ex gst)");
		$T->AddHead("Total");
		$T->AddHead("Status");
		$T->AddHead("Date Completed");
		//$T->AddHead("Date Closed");
		$T->AddHead("");
		$T->AddHead("");
		$T->EndRow();

		$T->AlternateRows("row0", "row1");
	
		$Audits = new audit($this->OperatorID);

		foreach ($Audits->GetAuditsForOperator($this->OperatorID) as $Audit)
		{
			if ($Audit["date_due"] && $Audit["date_due"] != "0000-00-00")
				$T->AddData(date(DATE_FORMAT, strtotime($Audit["date_due"])));
			else
				$T->AddData("");
			$T->AddData($Audits->GetAuditorName($Audit["auditor_id"]));
			$T->AddData($Audit["scheme_name"]);
			$T->AddData($Audit["description"]);
			$T->AddData($Audit["net_audit"]);
			$T->AddData($Audit["total_cost"]);
			$T->AddData($Audit["audit_status"]);
			if ($Audit["date_completed"] && $Audit["date_completed"] != "0000-00-00")
				$T->AddData(date("d/m/Y", strtotime($Audit["date_completed"])));
			else
				$T->AddData("");
/*
			if ($Audit["date_closed"] && $Audit["date_closed"] != "0000-00-00")
				$T->AddData(date("d/m/Y", strtotime($Audit["date_closed"])));
			else
				$T->AddData("");
*/

			$T->AddData("<center><button id='edit_audit' type='button' onClick='parent.location=\"".$this->URL("audit|audit_edit", array("OperatorID" => $this->OperatorID, "AuditID" => $Audit["audit_id"], "Return" => "audit|audit_main"))."\"'>Edit</button></center>");
			//$T->AddData("<center><button type='button' onClick='DeleteDialog(\"".$Audit["audit_id"]."\")'>Delete</button></center><div id='dialog-confirm'><div>");
			$T->AddData("<center><button type='button' onClick='DeleteDialog(\"".$Audit["audit_id"]."\", \"".$Audit["scheme_name"]." ".$Audit["description"]." audit on ".date(DATE_FORMAT, strtotime($Audit["date_due"]))."\")'>Delete</button></center>");

			$T->EndRow();
		}

		if ($Drawing)
			DivBox::PutBoxStart("audit_display", "", "audit_display");
		$T->Put();
		echo "<div id='dialog-confirm'></div>\n";
		if ($Drawing)
			DivBox::PutBoxEnd();
	}

	function DisplayNotesEntry($Drawing = false)
	{
		$T = new Table();
		$T->AddHead("<h3>Notes</h3>", 2);
		$T->EndRow();

		$T->AddData(field::GetArea("Note", "", 40, 4, "id='NoteArea'"), 2);
		$T->EndRow();

		$T->AddHead("<center><button id='save_note' type='button'>Save</button></center>", 2);
		$T->EndRow();

		DivBox::PutBoxStart("notes_entry", "", "notes_entry");
		$T->Put();
		$this->DisplayNotes($Drawing);
		DivBox::PutBoxEnd();
	}

	function DisplayNotes($Drawing = false)
	{
		$T = new Table();

		$Notes = $this->Operator->GetNotes(5);
	
		$T->AddHead("Date");
		$T->AddHead("Note");
		$T->EndRow();

		$T->AlternateRows("row0", "row1");

		foreach ($Notes as $Note)
		{
			$T->AddData(date("d/m/Y", strtotime($Note["note_date"])));
			$T->AddData($Note["note"]);
			$T->EndRow();
		}

		if ($Drawing)
			DivBox::PutBoxStart("notes_display", "", "notes_display");
		$T->Put();
		if ($Drawing)
			DivBox::PutBoxEnd();
	}

	function DisplayNotes2()
	{
		$T = new Table();
		$T->AddHead("<h3>Notes</h3>", 2);
		$T->EndRow();

		$T1 = new Table();

		$T1->AddData(field::GetArea("Note", "", 60, 4));
		$T1->EndRow();

		$T1->AddHead("<center><button id='save_note' type='button'>Save</button></center>");
		$T1->EndRow();

		$T2 = new Table();

		$Notes = $this->Operator->GetNotes(5);
	
		$T2->AddHead("Date");
		$T2->AddHead("Note");
		$T2->EndRow();

		foreach ($Notes as $Note)
		{
			$T2->AddData(date("d/m/Y", strtotime($Note["note_date"])));
			$T2->AddData($Note["note"]);
			$T2->EndRow();
		}

		$T->AddData($T1->Get());
		$T->AddData($T2->Get());
		$T->EndRow();

		DivBox::PutBoxStart("notes_display", "", "notes_display");
		$T->Put();
		DivBox::PutBoxEnd();
	}

	function MenuExtra()
	{
		echo "<div class='search_block'>\n";
		echo tools::GetAutocompleteContactSearch();
		echo "</div>\n";
		echo "<div class='search_block'>\n";
		echo tools::GetAutocompleteOperatorSearch();
		echo "</div>\n";
	}
}

?>
