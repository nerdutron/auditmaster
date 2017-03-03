<?php

class scheme
{
	function __construct()
	{
		$this->Load();
	}

	function Load()
	{
		$this->Data = DBResult(DB, "SELECT * FROM schemes");
	}

	function GetSchemes()
	{
		return $this->Data;
	}

	function GetSchemeDrop($FirstField = "", $Where = "")
	{
		$Q = new DBQuery();

		$Query = "SELECT * FROM schemes";

		if ($Where)
			$Query .= " ".$Where;

		$Q->Run(DB, $Query);

		$Schemes = array();

		if ($FirstField)
			$Schemes[$FirstField] = $FirstField;

		while ($Q->Next())
		{
			$Schemes[$Q->GetField("scheme_id")] = $Q->GetField("scheme_name");
		}

		$Q->EndQuery();

		return $Schemes;
	}

	function AddScheme($OperatorID, $SchemeID, $StartDate, $ExpiryDate = "")
	{
		$INQ = new DBInsert();

		$INQ->Save("operator_id", $OperatorID);	
		$INQ->Save("scheme_id", $SchemeID);	
		$INQ->Save("stage", $Stage);	
		$INQ->Save("start_date", $StartDate);	

		if ($ExpiryDate)
			$INQ->Save("expiry_date", $ExpiryDate);
		
		$INQ->Run(DB, "operator_schemes", "INSERT");

		$INQ->EndQuery();	
	}

	function ChangeExpiry($OperatorID, $SchemeID, $ExpiryDate)
	{
		$INQ = new DBInsert();

		$INQ->SaveWhere("operator_id", $OperatorID);	
		$INQ->SaveWhere("scheme_id", $SchemeID);	
		$INQ->Save("expiry_date", $ExpiryDate);
		
		$INQ->Run(DB, "operator_schemes", "UPDATE");

		$INQ->EndQuery();	
	}

	function GetSchemeSignup($OperatorID)
	{
		return DBResult(DB, "SELECT os.*, scheme_name FROM operator_schemes os, schemes s WHERE os.scheme_id = s.scheme_id AND operator_id = ".sql::integer($OperatorID));	
	}

	function GetSchemeOverall($OperatorID)
	{
		return DBResult(DB, "SELECT s.scheme_id as scheme_id, scheme_name, start_date, stage, expiry_date FROM schemes s LEFT JOIN operator_schemes os ON s.scheme_id = os.scheme_id AND os.operator_id = ".sql::integer($OperatorID));	
	}

	function GetStatus($OperatorID, $SchemeID)
	{
		return DBGet(DB, "SELECT * FROM operator_schemes WHERE operator_id = ".sql::integer($OperatorID)." AND scheme_id = ".sql::integer($SchemeID));
	}

	function GetExpiry($OperatorID, $SchemeID)
	{
		return DBGet(DB, "SELECT expiry_date FROM operator_schemes WHERE operator_id = ".sql::integer($OperatorID)." AND scheme_id = ".sql::integer($SchemeID));
	}

	function GetSchedule($SchemeID)
	{
		return DBGet(DB, "SELECT schedule FROM schemes WHERE scheme_id = $SchemeID");
	}

	function GetSchemeName($SchemeID)
	{
		return DBGet(DB, "SELECT scheme_name FROM schemes WHERE scheme_id = $SchemeID");
	}

	function GetUnenrolledSchemeDrop($OperatorID)
	{
		$EnrolledSchemes = scheme::GetSchemeSignup($OperatorID);


		foreach ($EnrolledSchemes as $Scheme)
		{
			$EnSchemes[] = $Scheme["scheme_id"];
		}

		if (sizeof($EnSchemes) > 0)
		{
			$Where = "WHERE scheme_id NOT IN (";
			$Where .= implode(",", $EnSchemes);
			$Where .= ")";
		}
		else
			$Where = "";

		return scheme::GetSchemeDrop("", $Where);
	}
}

?>
