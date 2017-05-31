<?php

class operator
{
	function __construct($OperatorID = "")
	{
		$this->OperatorID = sql::integer($OperatorID);

		if ($this->OperatorID)
			$this->Load();
	}

	function Load()
	{
		$this->Data = DBRow(DB, "SELECT * FROM operators WHERE operator_id = ".$this->OperatorID);
	}

	function GetData()
	{
		return $this->Data;
	}

	function Save($Data, $CreateUserID = "", $CreateOpID = "")
	{
		$INQ = new DBInsert();

		if ($this->OperatorID)
		{
			$INQ->SaveWhere("operator_id", $this->OperatorID);
			$Action = "UPDATE";
		}
		else
		{
			$Action = "INSERT";
			$INQ->Save("created_by", $CreateUserID);
			$INQ->Add("FUNCTION", "setup_date", "now()");

			if ($CreateOpID)
				$INQ->Save("operator_id", $CreateOpID);
		}

		$INQ->Save("legal_name", $Data["legal_name"]);
		$INQ->Save("trading_name", $Data["trading_name"]);
		$INQ->Save("abn", $Data["abn"]);
		$INQ->Save("customer_number", $Data["customer_number"]);
		$INQ->Save("owners_name", $Data["owners_name"]);
		$INQ->Save("email", $Data["email"]);
		$INQ->Save("website", $Data["website"]);
		$INQ->Save("phone", $Data["phone"]);
		$INQ->Save("fax", $Data["fax"]);
		if (trim($Data["mobile_1"]))
			$INQ->Save("mobile_1", $Data["mobile_1"]);
		if (trim($Data["mobile_2"]))
			$INQ->Save("mobile_2", $Data["mobile_2"]);
		$INQ->Save("last_audit_date", $Data["last_audit_date"]);
		$INQ->Save("address_1", $Data["address_1"]);
		$INQ->Save("address_2", $Data["address_2"]);
		$INQ->Save("suburb", $Data["suburb"]);
		$INQ->Save("postcode", $Data["postcode"]);
		$INQ->Save("state", $Data["state"]);
		$INQ->Save("postal_address_1", $Data["postal_address_1"]);
		$INQ->Save("postal_address_2", $Data["postal_address_2"]);
		$INQ->Save("postal_suburb", $Data["postal_suburb"]);
		$INQ->Save("postal_postcode", $Data["postal_postcode"]);
		$INQ->Save("postal_state", $Data["state"]);
		$INQ->Save("operator_status_id", $Data["operator_status_id"]);
		$INQ->Save("setup_by", $Data["setup_by"]);
		$INQ->Save("active", $Data["active"]);

		$INQ->Run(DB, "operators", $Action);

		if (!$this->OperatorID)
			$this->OperatorID = $INQ->GetInsertID();

		$INQ->EndQuery();

		return $this->OperatorID;
	}

	function SaveContact($Data, $ContactID = 0)
	{
		$INQ = new DBInsert();

		if ($ContactID)
		{
			$INQ->SaveWhere("contact_id", $ContactID);
			$Action = "UPDATE";
		}
		else
		{
			$Action = "INSERT";
		}

		$INQ->Save("operator_id", $this->OperatorID);
		$INQ->Save("surname", $Data["surname"]);
		$INQ->Save("firstname", $Data["firstname"]);
		$INQ->Save("nickname", $Data["nickname"]);
		$INQ->Save("phone", $Data["phone"]);
		$INQ->Save("fax", $Data["fax"]);
		$INQ->Save("mobile", $Data["mobile"]);
		$INQ->Save("email", $Data["email"]);
		$INQ->Save("position", $Data["position"]);

		$INQ->Run(DB, "contacts", $Action);

		if (!$ContactID)
			$ContactID = $INQ->GetInsertID();

		$INQ->EndQuery();

		return $ContactID;
	}

	function GetContacts()
	{
		return DBResult(DB, "SELECT * FROM contacts WHERE operator_id = ".$this->OperatorID);
	}

	function GetContact($ContactID)
	{
		return DBRow(DB, "SELECT * FROM contacts WHERE operator_id = ".$this->OperatorID." AND contact_id = ".sql::integer($ContactID));
	}

	function SaveNote($Note, $UserID)
	{
		$INQ = new DBInsert();

		$INQ->Save("operator_id", $this->OperatorID);
		$INQ->Save("note", $Note);
		//$INQ->Add("FUNCTION", "note_date", "now()");
		$INQ->Save("note_date", date("Y-m-d H:i:s"));
		$INQ->Save("user_id", $UserID);

		$INQ->Run(DB, "notes", "INSERT");
	}

	function GetNotes($Limit = 0)
	{
		$Query = "SELECT * FROM notes WHERE operator_id = ".$this->OperatorID." AND audit_id IS NULL ORDER BY note_date DESC";

		if ($Limit > 0)
			$Query .= " LIMIT $Limit";

		return DBResult(DB, $Query);
	}

	function GetOperatorName($OperatorID)
	{
		return DBGet(DB, "SELECT legal_name FROM operators WHERE operator_id = ".sql::integer($OperatorID));
	}

	function OperatorSearch($Term)
	{
		$Q = new DBQuery();

		$Q->Run(DB, "SELECT * FROM operators WHERE legal_name LIKE '%".sql::text($Term)."%' OR trading_name LIKE '%".sql::text($Term)."%'");

		$List = array();

		while ($Q->Next())
		{
			$List[] = array("id" => (int)$Q->GetField("operator_id"),
							"label" => $Q->GetField("trading_name")."(".$Q->GetField("legal_name").")",
							"key" => $Q->GetField("legal_name"));
		}

		$Q->EndQuery();

		return $List;
	}

	function ContactSearch($Term)
	{
		$Q = new DBQuery();

		$Q->Run(DB, "SELECT * FROM operators o, contacts c WHERE o.operator_id = c.operator_id AND CONCAT(firstname, ' ', surname) LIKE '%".sql::text($Term)."%'");

		$List = array();

		while ($Q->Next())
		{
			$List[] = array("id" => (int)$Q->GetField("operator_id"),
							"label" => $Q->GetField("firstname")." ".$Q->GetField("surname")." (".$Q->GetField("legal_name").")",
							"key" => $Q->GetField("legal_name"));
		}

		$Q->EndQuery();

		return $List;
	}

	function OperatorFullSearch($Params)
	{
		$Q = new DBQuery();

		$Query = "SELECT * FROM operators";

		$Seperator = "WHERE";

		foreach ($Params as $Field => $Value)
		{
			if (strlen(trim($Value)))
			{
				switch ($Field)
				{
					case "name":
						$Query .= " ".$Seperator." (legal_name LIKE '%".sql::text($Value)."%' OR trading_name LIKE '%".sql::text($Value)."%')";
						break;
					case "suburb":
					case "owners_name":
					case "email":
					case "contact_name":
						$Query .= " ".$Seperator." ".$Field." LIKE '%$Value%'";
						break;
					case "created_by":
					case "operator_status_id":
					case "active":
						$Query .= " ".$Seperator." ".$Field." = $Value";
						break;
					case "setup_after":
						$Query .= " ".$Seperator." setup_date >= '".date("Y-m-d", strtotime(str_replace("/", "-", $Value)))."'";
						break;
					case "setup_before":
						$Query .= " ".$Seperator." setup_date <= '".date("Y-m-d", strtotime(str_replace("/", "-", $Value)))."'";
						break;
					default:
						$Query .= " ".$Seperator." ".$Field." = '$Value'";
						break;
				}

				$Seperator = "AND";
			}
		}

		$Query .= " ORDER BY legal_name";

		//echo $Query;

		$Q->Run(DB, $Query);

		$Result = array();

		while ($Q->Next())
		{
			$Row = $Q->GetArray();
			$Row["contacts"] = DBResult(DB, "SELECT * FROM contacts WHERE operator_id = ".$Row["operator_id"]);
			$Result[] = $Row;
		}

		$Q->EndQuery();

		return $Result;
	}

	function GetNextID($CurrentID)
	{
		return DBGet(DB, "SELECT operator_id FROM operators WHERE operator_id > $CurrentID ORDER BY operator_id LIMIT 1");
	}

	function GetPrevID($CurrentID)
	{
		return DBGet(DB, "SELECT operator_id FROM operators WHERE operator_id < $CurrentID ORDER BY operator_id DESC LIMIT 1");
	}
}

?>
