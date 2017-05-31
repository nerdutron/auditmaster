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

class audit_edit extends secure
{
	function __construct()
	{
		parent::__construct();
		$this->Title = "Audit Edit";
		$this->MakeL("OperatorID", "AuditID");

		$this->Audit = new audit($this->OperatorID, $this->AuditID);

		//$this->States = tools::GetStateDrop();
	}

	function Head()
	{
		parent::Head();
		echo '<script type="text/javascript" src="js/jquery.js"></script>'."\n";
		tools::GetDatePopupJS("date_due");
		tools::GetDatePopupJS("date_completed");
		echo '<link href="css/uploadify.css" rel="stylesheet" type="text/css" />'."\n";
		echo '<script type="text/javascript" src="js/swfobject.js"></script>'."\n";
		echo '<script type="text/javascript" src="js/jquery.uploadify.v2.1.0.min.js"></script>';
		echo "<script type='text/javascript'>\n";
		echo "$(document).ready(function() {\n";
		echo "    $('#uploadify').uploadify({\n";
		echo "        'uploader'       : 'js/uploadify.swf',\n";
		//echo "        'script'         : '".$this->URL("save_file")."',\n";
		echo "        'script'         : 'uploadify.php',\n";
		echo "        'scriptData'     : {'OperatorID':'".$this->OperatorID."','AuditID':'".$this->AuditID."'},\n";
		echo "        'cancelImg'      : 'cancel.png',\n";
		echo "        'folder'         : 'uploads',\n";
		echo "        'queueID'        : 'fileQueue',\n";
		echo "        'auto'           : true,\n";
		echo "        'multi'          : true,\n";
		echo "		  'onAllComplete'  : function(event, data) {\n";
		echo "					$.ajax({url: '".$this->URL("audit|file_table", array("OperatorID" => $this->OperatorID, "AuditID" => $this->AuditID))."',\n";
		echo "						success: function(result) {\n";
		echo "							$('.files_table').html(result);\n";
		echo "						}});\n";
      		echo "				}\n";
		echo "    });\n";
		echo '	$("#save_note").click(function(){'."\n";
		echo "	  $.ajax({url: '".$this->URL("audit|save_audit_notes", array("OperatorID" => $this->OperatorID, "AuditID" => $this->AuditID))."',\n";
		echo "		data: {'Note' : document.getElementById('NoteArea').value},\n";
		echo "		success: function(result) {\n";
		echo "			$('.notes_display').html(result);\n";
		echo "		}});\n";
		echo "	});\n";
		echo "});\n";
		echo "</script>\n";
		echo "<script type='text/javascript'>\n";
		echo "function DeleteDialog(FileID, FileName)\n";
		echo "{\n";
		echo "  $(\"#dialog-confirm\").dialog(\n";
		echo "  {\n";
		echo "      resizable: false,\n";
		echo "      title: 'Do you wish to delete the file ' + FileName,\n";
		echo "      height:60,\n";
		echo "      modal: true,\n";
		echo "      buttons: {\n";
		echo "      'Delete': function() {\n";
		echo "      $.get('".$this->URL("audit|delete_file")."', {'FileID' : FileID} );\n";
		echo "		var dialog = $(this);\n";
		echo "		$.ajax({url: '".$this->URL("audit|file_table", array("OperatorID" => $this->OperatorID, "AuditID" => $this->AuditID))."',\n";
		echo "			success: function(result) {\n";
		echo "				$('.files_table').html(result);\n";
		echo "           		dialog.dialog('close');\n";
		echo "			}});\n";
		echo "       },\n";
		echo "      Cancel: function() {\n";
		echo "          $(this).dialog('close');\n";
		echo "      }\n";
		echo "  }\n";
		echo "});\n";
        echo "};\n";
		echo "function AddTotal()\n";
		echo "{\n";
		echo "	var Result = Math.round((parseInt($('#net_audit').val()) + parseInt($('#net_travel').val())) * 1.1);\n";
		echo "	if (!isNaN(Result))\n";
		echo "		$('#total_cost').val(Result);\n";
		echo "	\n";
		echo "}\n";
        echo "</script>\n";

	}

	function PageContent($Message = "")
	{
		if ($Message)
		{
			$this->ShowMessage($Message);
		}

		$FormData = $this->Audit->GetDetails();

		$T = new Table();

		$T->AddHead("<h3>Audit</h3>", 2);
		$T->EndRow();
		$T->AddHead("Date Due:");
		if ($FormData["date_due"])
			$Date = date(DATE_FORMAT, strtotime($FormData["date_due"]));
		else
			$Date = "";

		$T->AddData(field::GetText("FormData[date_due]", $Date, 12, 12, "id='date_due'"));
		$T->EndRow();

		$T->AddHead("Operator:");
		$T->AddData("<a href='".$this->URL("audit|audit_main", array("OperatorID" => $this->OperatorID))."'>".operator::GetOperatorName($this->OperatorID)."</a>");
		$T->EndRow();

		$T->AddHead("Auditor:");
		if ($this->NeedsAdmin(2) || $this->Audit->Editable())
			$T->AddData(field::GetDrop("FormData[auditor_id]", $FormData["auditor_id"], audit::GetAuditorDrop(array("" => ""))));
		else
		{
			$AuditorArray = audit::GetAuditorDrop();
			$T->AddData($AuditorArray[$FormData["auditor_id"]]);
		}
		$T->EndRow();
		$T->AddHead("Stage:");
		$T->AddData(field::GetDrop("FormData[stage_id]", $FormData["stage_id"], audit::GetStageDrop($FormData["scheme_id"])));
		//$T->AddData(tools::GetStageDesc($FormData["stage_id"]));
		$T->EndRow();
		$T->AddHead("Scheme:");
		//$T->AddData(field::GetDrop("FormData[scheme_id]", $FormData["scheme_id"], scheme::GetSchemeDrop()));
		$T->AddData(scheme::GetSchemeName($FormData["scheme_id"]));
		$T->EndRow();

/*
		$T->AddHead("Audit Cost:");
		$T->AddData("$".field::GetText("FormData[cost]", $FormData["cost"], 8, 8));
		$T->EndRow();
		$T->AddHead("Amount Paid:");
		$T->AddData("$".field::GetText("FormData[paid]", $FormData["paid"], 8, 8));
		$T->EndRow();

		$T->AddHead("Net Cost:");
		$T->AddData("$".field::GetText("FormData[net_cost]", $FormData["net_cost"], 8, 8));
		$T->EndRow();
*/
		$T->AddHead("Net Audit (ex gst):");
		$T->AddData("$".field::GetText("FormData[net_audit]", $FormData["net_audit"], 8, 8, "id='net_audit' onChange='AddTotal()'"));
		$T->EndRow();
		$T->AddHead("Net Travel:");
		$T->AddData("$".field::GetText("FormData[net_travel]", $FormData["net_travel"], 8, 8, "id='net_travel' onChange='AddTotal()'"));
		$T->EndRow();

		$T->AddHead("Total (inc gst):");
		$T->AddData("$".field::GetText("FormData[total_cost]", $FormData["total_cost"], 8, 8, "id='total_cost'"));
		$T->EndRow();

		$T->AddHead("Date Completed:");
		if ($FormData["date_completed"] && $FormData["date_completed"] != "0000-00-00")
			$Date = date(DATE_FORMAT, strtotime($FormData["date_completed"]));
		else
			$Date = "";
		$T->AddData(field::GetText("FormData[date_completed]", $Date, 12, 12, "id='date_completed'"));
		$T->EndRow();

		$T->AddHead("NCR:");
		$T->AddData(field::GetCheck("FormData[non_conformance_report]", $FormData["non_conformance_report"], "Y"));
		$T->EndRow();

		//$T->AddHead("<a href='".$this->URL("<center>audit|operator_maintenance", array("OperatorID" => $this->OperatorID, "Return" => "audit|audit_main"))."'>Save</a></center>", 2);
		$T->AddHead("<center>".field::GetFormSubmit("Exit", "FormData[Step]")."</center>");
		$T->AddHead("<center>".field::GetFormSubmit("Save")."</center>");
		$T->EndRow();

		DivBox::PutBoxStart("audit_display", "", "audit_display");
		field::PutFormStart($this->URL("audit|audit_edit_save"));
		field::PutHidden("OperatorID", $this->OperatorID);
		field::PutHidden("AuditID", $this->AuditID);
		$T->Put();
		field::PutFormEnd();
		DivBox::PutBoxEnd();

		$this->Files();
		$this->DisplayNotesEntry();

		// Push footer below date popup level as it is visible behind it
		for ($i = 0; $i < 40; $i++)
			echo "<br/>";
	}

/*
	function Notes()
	{
		$T = new Table();

		$T->AddHead("<h3>Notes</h3>", 2);
		$T->EndRow();


		$T->AddData(field::GetArea("Note", "", 60, 4), 2);
		$T->EndRow();

		$T->AddHead("<center><button id='save_note' type='button'><b>Save</b></button></center>", 2);
		$T->EndRow();

		$T->AlternateRows("row0", "row1");

		$Notes = $this->Audit->GetNotes(5);
	
		$T->AddHead("Date");
		$T->AddHead("Note");
		$T->EndRow();

		foreach ($Notes as $Note)
		{
			$T->AddData(date("d/m/Y", strtotime($Note["note_date"])));
			$T->AddData($Note["note"]);
			$T->EndRow();
		}

		DivBox::PutBoxStart("notes_display", "", "notes_display");
		$T->Put();
		DivBox::PutBoxEnd();
	}
*/
	function Files()
	{
		$T = new Table("", "", "width='100%'");

		$T->AddHead("<h3>Files</h3>");
		$T->EndRow();

		$T->AddData("<div id='fileQueue'></div><input type='file' name='uploadify' id='uploadify' /><p><a href=\"javascript:jQuery('#uploadify').uploadifyClearQueue()\">Cancel All Uploads</a></p>");

		$T->EndRow();

/*
		$T->AddData(field::GetFile("FileData"), 3);
		$T->EndRow();

		$T->AddHead("<center><button id='save_file' type='button'><b>Save</b></button></center>", 2);
		$T->EndRow();
*/


		DivBox::PutBoxStart("files_display", "", "files_display");
		$T->Put();
		echo $this->FileTable();
		DivBox::PutBoxEnd();

	}


	function FileTable()
	{
		$T = new Table();
		$T->AlternateRows("row0", "row1");

		$Files = $this->Audit->GetFiles(5);
	
		if (is_array($Files))
		{
			$T->AddHead("Date");
			$T->AddHead("Description");
			$T->AddHead("");
			$T->AddHead("");
			$T->EndRow();

			foreach ($Files as $File)
			{
				$T->AddData(date("d/m/Y", strtotime($File["loaded"])));
				$T->AddData($File["file_name"]);
				$T->AddData("<center><button id='get_file' type='button' onClick='window.location.href = \"".$this->URL("audit|get_file", array("FileID" => $File["file_id"]))."\";'>Get File</button></center>");
				$T->AddData("<center><button id='delete_file' type='button' onClick='DeleteDialog(".$File["file_id"].",\"".$File["file_name"]."\")'>Delete</button></center>");
				$T->EndRow();
			}
		}

		$Output = DivBox::GetBoxStart("files_table", "", "files_table");
		$Output .= $T->Get();
		$Output .= DivBox::GetBoxEnd();
		echo "<div id='dialog-confirm'></div>\n";

		return $Output;
	}

	function DisplayNotesEntry()
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
		$this->DisplayNotes();
		DivBox::PutBoxEnd();
	}

	function DisplayNotes()
	{
		$T = new Table();

		$Notes = $this->Audit->GetNotes(5);
	
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

		DivBox::PutBoxStart("notes_display", "", "notes_display");
		$T->Put();
		DivBox::PutBoxEnd();
	}
}

?>
