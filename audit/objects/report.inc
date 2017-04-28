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
require_once(ToIncludes("audit")."audit_report.inc");
require_once(ToIncludes("audit")."audit.inc");
require_once(ToIncludes("audit")."scheme.inc");
require_once(ToIncludes("audit")."tools.inc");

class report extends secure
{
	function __construct()
	{
		parent::__construct();
		$this->MakeL("Report", "Run", "Params", "Download");

		$this->Reports = array(
			"audit" => "Audit Report"
		);

		if (!is_array($this->Params))
		{
			// Defult to current month unassigned
			$this->Params = array("auditor_id" => "0", "start_date" => date("01/m/Y"));
		}

		$this->Processor = new audit_report($this->Report);

		if ($this->Run)
		{
			$RunParams = $this->Params;

			foreach ($RunParams as $Key => $Value)
			{
				if ($Value == "All")
					unset($RunParams[$Key]);
			}

			$this->Data = $this->Processor->Run($RunParams);
		}


		if ($this->NeedsAdmin(2))
		{
			if ($this->Download)
			{
				$this->AuditReportCSV();
				exit();
			}
		}

	}

	function Head()
	{
		parent::Head();
		echo '<script type="text/javascript" src="js/jquery.js"></script>'."\n";
		tools::GetDatePopupJS("from_date");
		tools::GetDatePopupJS("to_date");

		// Special option for power user
		if ($this->NeedsAdmin(2))
		{
			tools::GetDatePopupJS("from_completed_date");
			tools::GetDatePopupJS("to_completed_date");
			echo "<script type='text/javascript'>\n";
			echo "function ChangeAuditor(AuditID)\n";
			echo "{\n";
			echo "	var Audstring = '#audit_' + AuditID;\n";
			echo "	var Auditor = $(Audstring).val();\n";
			echo "	$.get('".$this->URL("audit|change_auditor")."', {'AuditID':AuditID, 'AuditorID':Auditor});\n";
			echo "}\n";
			echo "</script>\n";
		}
	}

	function PageContent($Message = "")
	{
		if ($Message)
		{
			$this->ShowMessage($Message);
		}

		switch ($this->Report)
		{
			case "audit":
				$this->AuditReport();
				break;
			default:
				$this->ShowMenu();
				break;
		}
	}

	function ShowMenu()
	{
		$T = new Table();

		foreach ($this->Reports as $Key => $Title)
		{
			$T->AddHead("<a href='".$this->URL("audit|report", array("Report" => $Key, "Run" => 0))."'>$Title</a>");
			$T->EndRow();
		}

		DivBox::PutBoxStart("form_block");
		$T->Put();
		DivBox::PutBoxEnd();
	}

	function AuditReportCSV()
	{
		$SuperUser = $this->NeedsAdmin(2);

		header("Content-type: text/csv");
		header("Content-Disposition: attachment; filename=\"audits.csv\"");


		echo "Due Date,";
		echo "Operator,";
		echo "Suburb,";
		echo "State,";
		echo "Auditor,";
		echo "Scheme,";
		echo "Status,";
		echo "Date Completed,";
		if ($SuperUser)
			echo "Net Audit,";
		echo "\n";

		foreach ($this->Data as $Row)
		{
			echo date(DATE_FORMAT, strtotime($Row["date_due"])).",";
			if ($Row["operator_name"])
				echo $Row["operator_name"].",";
			else
				echo $Row["legal_name"].",";

			echo $Row["suburb"].",";
			echo $Row["state"].",";
			echo $Row["auditor"].",";
			echo $Row["scheme_name"].",";
			echo $Row["audit_status"].",";
			if ($Row["date_completed"])
				echo date(DATE_FORMAT, strtotime($Row["date_completed"])).",";
			else
				echo ",";

			if ($SuperUser)
				echo $Row["net_audit"];

			echo "\n";
		}	
		return;
	}

	function AuditReport()
	{
		$this->SortBy = array(
			"date_due ASC" => "Due Date (Oldest to Newest)",
			"date_due DESC" => "Due Date (Newest to Oldest)",
			"date_completed ASC" => "Completion Date (Oldest to Newest)",
			"date_completed DESC" => "Completion Date (Newest to Oldest)",
			"auditor" => "Auditor",
			"operator_id" => "Operator",
			"scheme_name" => "Scheme"
		);

		$this->Auditors = audit::GetAuditorDrop(array("" => ""));


		if (!$this->Run)
		{
			$this->Run = 1;
			$ShowTable = false;
		}
		else
			$ShowTable = true;

		$this->ShowHeader();

		if (!$ShowTable)
			return;

		$T = new Table();
		$SuperUser = $this->NeedsAdmin(2);

		$Colspan = 9;

		$T->AddHead("Due Date");
		$T->AddHead("Operator");
		$T->AddHead("Suburb");
		$T->AddHead("State");
		$T->AddHead("Auditor");
		$T->AddHead("Scheme");
		$T->AddHead("Status");
		$T->AddHead("Date Completed");
		if ($SuperUser)
		{
			$T->AddHead("Net Audit");
			$Colspan++;
		}
		$T->AddHead("");
		$T->EndRow();

		$T->AlternateRows();
		$AuditCost;

		foreach ($this->Data as $Row)
		{
			$T->AddData(date(DATE_FORMAT, strtotime($Row["date_due"])));
			if ($Row["operator_name"])
				$T->AddData($Row["operator_name"]);
			else
				$T->AddData($Row["legal_name"]);

			$T->AddData($Row["suburb"]);
			$T->AddData($Row["state"]);

			// Special option for power user
			if ($SuperUser)
				$T->AddData(field::GetDrop("auditor".$Row["audit_id"], $Row["auditor_id"], $this->Auditors, "id='audit_".$Row["audit_id"]."' onChange='ChangeAuditor(\"".$Row["audit_id"]."\")'"));
			else
				$T->AddData($Row["auditor"]);

			$T->AddData($Row["scheme_name"]);
			$T->AddData($Row["audit_status"]);
			if ($Row["date_completed"])
				$T->AddData(date(DATE_FORMAT, strtotime($Row["date_completed"])));
			else
				$T->AddData("");

			if ($SuperUser)
			{
				$T->AddData($Row["net_audit"]);
				$AuditCost += $Row["net_audit"];
			}
			
			$T->AddData("<center><button id='edit_audit' type='button' onClick='parent.location=\"".$this->URL("audit|audit_edit", array("OperatorID" => $Row["operator_id"], "AuditID" => $Row["audit_id"], "Return" => "audit|audit_report"))."\"' class='noprint'>Edit</button></center>", 2);
			$T->EndRow();
		}

		if ($SuperUser)
		{
			$T->AddHead(sizeof($this->Data)." Audits", $Colspan - 3);
			$T->AddHead("Total");
			$T->AddHead("$".number_format($AuditCost, 2));
			$T->AddHead("");
		}
		else
			$T->AddHead(sizeof($this->Data)." Audits", $Colspan);
		$T->EndRow();
		DivBox::PutBoxStart("form_block");
		$T->Put();
		DivBox::PutBoxEnd();

	}

	function ShowHeader()
	{
		$T = new Table();

		$T->AddHead("From Due Date");
		$T->AddHead("To Due Date");
		$T->AddHead("Auditor");
		$T->AddHead("Scheme");
		$T->AddHead("Status");

		// Special option for power user
		if ($this->NeedsAdmin(2))
		{
			$T->AddHead("Stage");
			$T->AddHead("Completed Date Range", 2);
		}

		if (is_array($this->SortBy))
			$T->AddHead("Sort By");
		$T->AddHead("Spreadsheet");
		$T->EndRow();

		$T->AddData(field::GetText("Params[start_date]", $this->Params["start_date"], 12, 12, "id='from_date' onChange='submit();'"));
		$T->AddData(field::GetText("Params[end_date]", $this->Params["end_date"], 12, 12, "id='to_date' onChange='submit();'"));
		$T->AddData(field::GetDrop("Params[auditor_id]", $this->Params["auditor_id"], audit::GetAuditorDrop(array("" => "All")), "onChange='submit();'"));
		//$T->AddHead("Operator");
		$T->AddData(field::GetDrop("Params[scheme_id]", $this->Params["scheme_id"], scheme::GetSchemeDrop("All"), "onChange='submit();'"));
		//$T->AddHead("Date Completed");
		$T->AddData(field::GetDrop("Params[audit_status]", $this->Params["audit_status"], array("" => "All", "UNASSIGNED" => "Unassigned", "SCHEDULED" => "Scheduled", "PENDING" => "Pending", "COMPLETED" => "Completed"), "onChange='submit();'"));

		// Special option for power user
		if ($this->NeedsAdmin(2))
		{
			$T->AddData(field::GetDrop("Params[stage]", $this->Params["stage"], $this->Processor->GetStages(), "onChange='submit();'"));
			$T->AddData(field::GetText("Params[start_completed_date]", $this->Params["start_completed_date"], 12, 12, "id='from_completed_date' onChange='submit();'"));
			$T->AddData(field::GetText("Params[end_completed_date]", $this->Params["end_completed_date"], 12, 12, "id='to_completed_date' onChange='submit();'"));
		}

		if (is_array($this->SortBy))
		{
			$T->AddData(field::GetDrop("Params[sort_by]", $this->Params["sort_by"], $this->SortBy, "onChange='submit();'"));
		}

		$T->AddData(field::GetFormSubmit("Download", "Download"));
		$T->EndRow();

		DivBox::PutBoxStart("form_block");
		field::PutFormStart($this->URL("audit|report"));
		$T->Put();
		field::PutHidden("Report", $this->Report);
		field::PutHidden("Run", $this->Run);
		field::PutFormEnd();
		DivBox::PutBoxEnd();

	}

	function DataRange($Name)
	{

	}
}

?>
