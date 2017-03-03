<?php

class audit
{
	function __construct($OperatorID, $AuditID = 0)
	{
		$this->OperatorID = $OperatorID;
		$this->AuditID = $AuditID;

		if ($this->AuditID)
			$this->Load();
	}

	function Load()
	{
		$this->Data = DBRow(DB, "SELECT * FROM audits WHERE audit_id = ".$this->AuditID);
	}

	function GetDetails()
	{
		return $this->Data;
	}

	function Save($Data)
	{
		$INQ = new DBInsert();

		if ($this->AuditID)
		{
			$INQ->SaveWhere("audit_id", $this->AuditID);
			$Action = "UPDATE";
		}
		else
		{
			$Action = "INSERT";
			if (!$Data["audit_status"])
				$Data["audit_status"] = "PENDING";
		}

		if ($Data["operator_id"])
			$INQ->Save("operator_id", $Data["operator_id"]);
		$INQ->Save("auditor_id", $Data["auditor_id"]);

		if ($Data["stage_id"])
			$INQ->Save("stage_id", $Data["stage_id"]);
		if ($Data["scheme_id"])
			$INQ->Save("scheme_id", $Data["scheme_id"]);
//		$INQ->Save("cost", $Data["cost"]);
//		$INQ->Save("paid", $Data["paid"]);
//		$INQ->Save("net_cost", $Data["net_cost"]);
		$INQ->Save("net_audit", $Data["net_audit"]);
		$INQ->Save("net_travel", $Data["net_travel"]);
		$INQ->Save("total_cost", $Data["total_cost"]);
		$INQ->Save("date_due", $Data["date_due"]);

		$INQ->Save("date_completed", $Data["date_completed"]);
		$INQ->Save("date_closed", $Data["date_closed"]);
		$INQ->Save("audit_status", $Data["audit_status"]);
		$INQ->Save("non_conformance_report", $Data["non_conformance_report"]);

		$INQ->Run(DB, "audits", $Action);

		if (!$this->AuditID)
			$this->AuditID = $INQ->GetInsertID();

		$INQ->EndQuery();

		return $this->AuditID;
	}

	function AddNote($Note, $UserID)
	{
		$INQ = new DBInsert();

		$INQ->Save("operator_id", $this->OperatorID);
		$INQ->Save("audit_id", $this->AuditID);
		$INQ->Save("note", $Note);
		$INQ->Add("FUNCTION", "note_date", "now()");
		$INQ->Save("user_id", $UserID);

		$INQ->Run(DB, "notes", "INSERT");
	
		$INQ->EndQuery();	
	}

	function GetAuditsForOperator($OperatorID)
	{
		return DBResult(DB, "SELECT * FROM audits a LEFT JOIN users u on a.auditor_id = u.user_id, schemes s, stages st WHERE a.scheme_id = s.scheme_id AND a.stage_id = st.stage_id AND operator_id = ".sql::integer($OperatorID)." ORDER BY date_due desc");
		//return DBResult(DB, "SELECT * FROM audits a LEFT JOIN users u on aauditor_id = u.user_id, schemes s, stages st WHERE .auditor_id = u.user_id OR a.auditor_id = 0) AND a.scheme_id = s.scheme_id AND a.stage_id = st.stage_id AND operator_id = ".sql::integer($OperatorID));
	}

	function GetAuditorName($AuditorID)
	{
		if ($AuditorID == 0)
			return "Unassigned";

		return DBGet(DB, "SELECT name FROM users WHERE user_id = $AuditorID");
	}

	function AddAudit($SchemeID, $OperatorID, $DueDate, $Auditor, $StageID, $Status)
	{
		$this->AuditID = 0;

		$Data = array();

		$Data["operator_id"] = $OperatorID;
		$Data["scheme_id"] = $SchemeID;
		$Data["stage_id"] = $StageID;
		$Data["auditor_id"] = $Auditor;
		$Data["date_due"] = $DueDate;
		$Data["audit_status"] = $Status;

		$this->Save($Data);

	}

	function GetAuditorDrop($Auditors = array())
	{
		$Q = new DBQuery();

		$Q->Run(DB, "SELECT * FROM users WHERE auditor = 'Y' ORDER BY name");

		while ($Q->Next())
		{
			$Auditors[$Q->GetField("user_id")] = $Q->GetField("name");
		}

		$Q->EndQuery();

		return $Auditors;
	}

	function GetStageDrop($SchemeID)
	{
		$Q = new DBQuery();

		$Schedule = DBGet(DB, "SELECT schedule FROM schemes WHERE scheme_id = $SchemeID");

		$Query = "SELECT * FROM stages";

		if ($Schedule)
			$Query .= " WHERE schedule = '$Schedule'";

		$Q->Run(DB, $Query);

		$Stages = array();

		while ($Q->Next())
		{
			$Stages[$Q->GetField("stage_id")] = $Q->GetField("description");
		}

		$Q->EndQuery();

		return $Stages;

	}

	function GetNotes($Limit = 0)
	{
		//$Query = "SELECT * FROM notes WHERE operator_id = ".$this->OperatorID." AND audit_id = ".$this->AuditID." ORDER BY note_date DESC";
		$Query = "SELECT * FROM notes WHERE audit_id = ".$this->AuditID." ORDER BY note_date DESC";

		if ($Limit > 0)
			$Query .= " LIMIT $Limit";

		return DBResult(DB, $Query);
	}

	function SaveNote($Note, $UserID)
	{
		$INQ = new DBInsert();

		$INQ->Save("operator_id", $this->OperatorID);
		$INQ->Save("audit_id", $this->AuditID);
		$INQ->Save("note", $Note);
		$INQ->Add("FUNCTION", "note_date", "now()");
		$INQ->Save("user_id", $UserID);

		$INQ->Run(DB, "notes", "INSERT");
	}

	function GetFiles($Limit)
	{
		return DBResult(DB, "SELECT * FROM files f, file_links fl WHERE f.file_id = fl.file_id AND audit_id = ".$this->AuditID." ORDER BY loaded DESC");
	}

	function SaveFile($Data)
	{
		$INQ = new DBInsert();

		$INQ->Save("file_name", $Data["file_name"]);
		$INQ->Save("file_mime", $Data["file_mime"]);
		$INQ->Save("file_size", sizeof($Data["file_data"]));
		$INQ->Add("FUNCTION", "loaded", "now()");
		$INQ->Save("description", $Data["description"]);
		$INQ->Save("file_data", $Data["file_data"]);
		$INQ->Save("operator_id", $Data["operator_id"]);

		$INQ->Run(DB, "files", "INSERT");

		$FileID = $INQ->GetInsertID();

		$INQ->EndQuery();

		return $FileID;
	}

	function GetLastAudit($SchemeID)
	{
		$SchemeID = sql::integer($SchemeID);

		$Date = DBGet(DB, "SELECT max(date_due) FROM audits WHERE operator_id = ".$this->OperatorID." AND scheme_id = $SchemeID");

		if ($Date)
			$Result = DBRow(DB, "SELECT * FROM audits WHERE operator_id = ".$this->OperatorID." AND scheme_id = $SchemeID AND date_due = '$Date'");

		return $Result;
	}

	function Delete()
	{
		DB_DoQuery(DB, "DELETE FROM audits WHERE audit_id = ".$this->AuditID);
	}

	function ChangeAuditor($AuditorID)
	{
		$INQ = new DBInsert();

		$INQ->SaveWhere("audit_id", $this->AuditID);
		$INQ->Save("auditor_id", $AuditorID);

		if ($AuditorID)
			$INQ->Save("audit_status", "SCHEDULED");

		$INQ->Run(DB, "audits", "UPDATE");

		$INQ->EndQuery();
	}
}

?>
