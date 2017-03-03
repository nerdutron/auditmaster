<?php

class audit_business
{
	function __construct($User, $OperatorID = 0)
	{
		$this->User = $User;
		$this->OperatorID = $OperatorID;

		if ($this->OperatorID)
			$this->Load();
	}

	function Load()
	{
		$this->Operator = new operator($this->OperatorID);
		$this->Scheme = new scheme();
		$this->Schemes = $this->Scheme->GetSchemeSignup($this->OperatorID);
		$this->Audit = new audit($this->OperatorID);
		$this->Audits = $this->Audit->GetAuditsForOperator($this->OperatorID);
	}

	function SchemeSignup($SchemeID, $StartDate = "")
	{
		// Is this operator already registered?
		if (!$this->Registered($SchemeID))
		{
			if (!$StartDate)
				$StartDate = date("Y-m-d");

			// Use the expiry from the entry row of the scheme
			$Stage = $this->GetFirstStage($SchemeID, $ExpiryMonths);
			$ExpiryDate = date("Y-m-d", strtotime("+$ExpiryMonths months", strtotime($StartDate)));

			$this->Scheme->AddScheme($this->OperatorID, $SchemeID, $StartDate, $ExpiryDate);
		}
		else
		{
			$Stage = $this->GetNextStage($SchemeID, $StartDate, $ExpiryDate);
		}

		if ($this->User->IsAuditor())
			$UserID = $this->User->UserID();
		else
			$UserID = 0;

		$this->Audit->AddAudit($SchemeID, $this->OperatorID, $StartDate, $UserID, $Stage, audit_business::GetStatus(array("auditor_id" => $UserID, "date_due" => $StartDate)));
		if ($ExpiryDate)
		{
			$this->Scheme->ChangeExpiry($this->OperatorID, $SchemeID, $ExpiryDate);
		//	echo "New Expiry on $SchemeID = $ExpiryDate<br/>\n";
		}

		return $ExpiryDate;
	}

	function Registered($SchemeID)
	{
		$Status = $this->Scheme->GetStatus($this->OperatorID, $SchemeID);

		if (!$Status || $Status == "EXPIRED")
			return false;
		else
			return true;		
	}

	function GetNextStage($SchemeID, &$DueDate, &$ExpiryDate)
	{
		$CurrentExpiry = $this->Scheme->GetExpiry($this->OperatorID, $SchemeID);

		$CurrentAudit = $this->Audit->GetLastAudit($SchemeID);

		//$Schedule = $this->Scheme->GetSchedule($SchemeID);

		$NextStage = tools::GetNextStage($CurrentAudit["stage_id"]);

		$ExpiryDate = date("Y-m-d", strtotime("+".$NextStage["expiry_months"]." months", strtotime($CurrentExpiry)));

		if ($NextStage["calculate_from_expiry"] == "Y")
			$DueDate = date("Y-m-d", strtotime("-2 months", strtotime($ExpiryDate)));
		else
			$DueDate = date("Y-m-d", strtotime("+".$NextStage["months"]." months", strtotime($CurrentAudit["date_due"])));

		return $NextStage["stage_id"];
	}

	function GetFirstStage($SchemeID, &$ExpiryMonths)
	{
		$Schedule = $this->Scheme->GetSchedule($SchemeID);

		$NextStage = tools::GetFirstStage($Schedule);

		$ExpiryMonths = $NextStage["expiry_months"];

		return $NextStage["stage_id"];
	}

	function GetStatus($Audit)
	{
		if (!$Audit["auditor_id"])
			$Status = "UNASSIGNED";

		else if ($Audit["date_completed"])
			$Status = "COMPLETED";

		else if (strtotime($Audit["date_due"]) > time())
			$Status = "SCHEDULED";

		else
			$Status = "PENDING";

		return $Status;
	}
}

?>
