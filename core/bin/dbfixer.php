<?php

require_once("definitions.inc");
require_once(ToIncludes("audit")."operator.inc");

$Q = new DBQuery();

// Operators
$Q->Run(OLD_DB, "SELECT * FROM Operators");

while ($Q->Next())
{
	$Data = $Q->GetArray();

	$Insert = array();

	echo "Processing ".$Data["Enterprise_Name"]."\n";

	$Operator = new operator($Data["Operator_ID"]);

	$Insert = $Operator->GetData();

	$Insert["mobile_1"] = $Data["Mobile_1"];
	$Insert["mobile_2"] = $Data["Mobile_2"];
	$Insert["postal_address_1"] = $Data["P_Street"];
	$Insert["postal_address_2"] = $Data["address_2"];
	if ($Data["P_Suburb"])
	{
		$Suburb = DBRow(OLD_DB, "SELECT * FROM Suburbs WHERE Suburb_ID = ".$Data["P_Suburb"]);
		$Insert["postal_suburb"] = $Suburb["Suburb"];
		$Insert["postal_postcode"] = $Suburb["Postcode"];
		$Insert["postal_state"] = $Suburb["State"];
	}


	$Operator->Save($Insert, $Data["Setup_By"], $Data["Operator_ID"]);

	if ($Data["Mobile_2"])
	{

	}
	$Name = explode(" ", $Data["Contact_Name"]);

	$Contact = array();

	$Contact["surname"] = $Name[sizeof($Name) - 1];
	$Contact["firstname"] = "";

	for ($i = 0; $i < sizeof($Name) - 1; $i++)
		$Contact["firstname"] .= $Name[$i];

	$Contact["email"] = $Data["Email"];
	$Contact["phone"] = $Data["Phone"];
	$Contact["fax"] = $Data["Fax"];
	$Contact["mobile"] = $Data["Mobile_1"];

	$Operator->SaveContact($Contact);

	if (trim($Data["Mobile_2"]))
	{
		$Name = explode(" ", $Data["Owner_Name"]);

		$Contact = array();

		$Contact["surname"] = $Name[sizeof($Name) - 1];
		$Contact["firstname"] = "";

		for ($i = 0; $i < sizeof($Name) - 1; $i++)
			$Contact["firstname"] .= $Name[$i];

		$Contact["email"] = $Data["Email"];
		$Contact["phone"] = $Data["Phone"];
		$Contact["fax"] = $Data["Fax"];
		$Contact["mobile"] = $Data["Mobile_2"];
	}

	if (trim($Data["Notes"]))
	{
		$INQ = new DBInsert();
		$INQ->Save("operator_id", $Data["Operator_ID"]);
		$INQ->Save("note", $Data["Notes"]);
		$INQ->Save("note_date", "2010-01-01");

		$INQ->Run(DB, "notes", "INSERT");

		$INQ->EndQuery();
	}
}

$Q->EndQuery();

// Suburbs
$Q->Run(OLD_DB, "SELECT * FROM Suburbs");

$INQ = new DBInsert();

while ($Q->Next())
{
	$Data = $Q->GetArray();

	$INQ->Save("suburb", $Data["Suburb"]);
	$INQ->Save("state", $Data["State"]);
	$INQ->Save("postcode", $Data["Postcode"]);

	$INQ->Run(DB, "suburbs", "INSERT");

	$INQ->EndQuery();
}

/*
// Operator Scheme
$Q->Run(OLD_DB, "SELECT * FROM OpSchemes");

$INQ = new DBInsert();

while ($Q->Next())
{
	$Data = $Q->GetArray();

	$INQ->Save("operator_id", $Data["Operator_ID"]);
	$INQ->Save("scheme_id", $Data["Scheme_ID"]);
	$INQ->Save("stage", $Data["Stage_ID"]);
	$INQ->Save("start_date", $Data["Start_Date"]);

	$INQ->Run(DB, "operator_schemes", "INSERT");

	$INQ->EndQuery();
}

*/

// Audit
$Q->Run(OLD_DB, "SELECT * FROM Audits where Scheme_ID is not null");

$INQ = new DBInsert();

while ($Q->Next())
{
	$Data = $Q->GetArray();

	// If there is no operator, we can't do anything with the record, so just skip
	if (!$Data["Operator_ID"])
		continue;

	$INQ->Save("operator_id", $Data["Operator_ID"]);
	if ($Data["Auditor_ID"] == 1)
		$Status = "UNASSIGNED";
	else
		$INQ->Save("auditor_id", $Data["Auditor_ID"]);
	if (!$Data["Stage_ID"])
		$INQ->Save("stage_id", 0);
	else
		$INQ->Save("stage_id", $Data["Stage_ID"]);
	$INQ->Save("scheme_id", $Data["Scheme_ID"]);
	$INQ->Save("cost", $Data["Cost"]);
	$INQ->Save("paid", $Data["Paid"]);
	if ($Data["Date_Due"])
		$INQ->Save("date_due", date("Y-m-d", strtotime($Data["Date_Due"])));
	if ($Data["Date_Completed"])
		$INQ->Save("date_completed", date("Y-m-d", strtotime($Data["Date_Completed"])));
	if ($Data["Date_Closed"])
		$INQ->Save("date_closed", date("Y-m-d", strtotime($Data["Date_Closed"])));

	$INQ->Run(DB, "audits", "INSERT");

	$AuditID = $INQ->GetInsertID();

	$INQ->EndQuery();

	$INQ->Save("operator_id", $Data["Operator_ID"]);
	$INQ->Save("note", $Data["Notes"]);
	if ($Data["Date_Due"])
		$INQ->Save("note_date", date("Y-m-d", strtotime($Data["Date_Due"])));
	$INQ->Save("audit_id", $AuditID);

	$INQ->Run(DB, "notes", "INSERT");

	$INQ->EndQuery();

}


// For Operator Scheme create entries from scratch, as there are too many duplicates in missing entries in the original data

$Q = new DBQuery();

$Q->Run(DB, "SELECT operator_id, scheme_id, min(date_due) as start_date FROM audits group by operator_id, scheme_id");

while ($Q->Next())
{
	$INQ->Save("operator_id", $Q->GetField("operator_id"));
	$INQ->Save("scheme_id", $Q->GetField("scheme_id"));
	$INQ->Save("start_date", $Q->GetField("start_date"));
	$INQ->Save("stage", DBGet(DB, "SELECT max(stage_id) FROM audits WHERE operator_id = ".$Q->GetField("operator_id")." AND scheme_id = ".$Q->GetField("scheme_id")));

	$INQ->Save("expiry_date", date("Y-m-d", strtotime("+2 years", strtotime(DBGet(DB, "SELECT max(date_due) FROM audits WHERE operator_id = ".$Q->GetField("operator_id")." AND scheme_id = ".$Q->GetField("scheme_id"))))));

	$INQ->Run(DB, "operator_schemes", "INSERT");

	$INQ->EndQuery();
}

?>

