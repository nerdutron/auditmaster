<?php

class audit_report
{
	function __construct($Report)
	{
		$this->Report = $Report;
	}

	function Run($Params)
	{
		switch ($this->Report)
		{
			case "audit":
				return $this->RunAuditReport($Params);
				break;
		}
	}

	function RunAuditReport($Params)
	{
	
		$Query = "SELECT a.*, u.name as auditor, trading_name as operator_name, legal_name, suburb, state, scheme_name FROM audits a LEFT OUTER JOIN users u ON a.auditor_id = u.user_id, operators o, schemes s WHERE a.operator_id = o.operator_id AND a.scheme_id = s.scheme_id AND o.active = 1";
		//$Query = "SELECT a.*, u.name as auditor, trading_name as operator_name, scheme_name FROM audits a, operators o, users u, schemes s WHERE a.operator_id = o.operator_id AND aAND .scheme_id = s.scheme_id";

		if (!$Params)
			$Params = array();

		if ($Params["start_date"])
		{
			if (strstr($Params["start_date"], "/"))
				$Params["start_date"] = date("Y-m-d", strtotime(str_replace("/", "-", $Params["start_date"])));
		}

		if ($Params["end_date"])
		{
			if (strstr($Params["end_date"], "/"))
				$Params["end_date"] = date("Y-m-d", strtotime(str_replace("/", "-", $Params["end_date"])));
		}

		if ($Params["start_date"] && $Params["end_date"])
		{
			$Query .= " AND date_due BETWEEN '".$Params["start_date"]."' AND '".$Params["end_date"]."'";
		}
		else if ($Params["start_date"])
		{
			$Query .= " AND date_due > '".$Params["start_date"]."'";
		}
		else if ($Params["end_date"])
		{
			$Query .= " AND date_due < '".$Params["end_date"]."'";
		}

		if ($Params["start_completed_date"])
		{
			if (strstr($Params["start_completed_date"], "/"))
				$Params["start_completed_date"] = date("Y-m-d", strtotime(str_replace("/", "-", $Params["start_completed_date"])));
		}

		if ($Params["end_completed_date"])
		{
			if (strstr($Params["end_completed_date"], "/"))
				$Params["end_completed_date"] = date("Y-m-d", strtotime(str_replace("/", "-", $Params["end_completed_date"])));
		}

		if ($Params["start_completed_date"] && $Params["end_completed_date"])
		{
			$Query .= " AND date_completed BETWEEN '".$Params["start_completed_date"]."' AND '".$Params["end_completed_date"]."'";
		}
		else if ($Params["start_completed_date"])
		{
			$Query .= " AND date_completed > '".$Params["start_completed_date"]."'";
		}
		else if ($Params["end_completed_date"])
		{
			$Query .= " AND date_completed < '".$Params["end_completed_date"]."'";
		}

		if ($Params["stage"])
		{
			$Stages = DBResult(DB, "SELECT stage_id FROM stages WHERE description = '".$Params["stage"]."'");

			$Query .= " AND stage_id IN (";

			foreach ($Stages as $Index =>  $ID)
			{
				if ($Index > 0)
					$Query .= ",";
				$Query .= $ID["stage_id"];
			}

			$Query .= ")";

			unset($Params["stage"]);
		}

		foreach ($Params as $Field => $Val)
		{
			if ($Val)
			{
				switch ($Field)
				{
					case "start_date":
					case "end_date":
					case "start_completed_date":
					case "end_completed_date":
					case "sort_by":
						break;
					case "audit_status":
						$Query .= " AND a.$Field = '$Val'";
						break;
					default:
						$Query .= " AND a.$Field = $Val";
						break;
				}
			}
		}

		if ($Params["sort_by"])
		{
			switch ($Params["sort_by"])
			{
				case "auditor":
					$Query .= " ORDER BY ".$Params["sort_by"];
					break;
				default:
					$Query .= " ORDER BY a.".$Params["sort_by"];
			}
		}

		//$Query .= " LIMIT 10";
		//echo $Query;

		return DBResult(DB, $Query);
	}

	function GetStages()
	{
		$Stages = DBResult(DB, "SELECT description FROM stages");

		$StageArray = array("" => "All");

		foreach ($Stages as $Row)
			$StageArray[$Row["description"]] = $Row["description"];

		return $StageArray;
	}
}

?>
