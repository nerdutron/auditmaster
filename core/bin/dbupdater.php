<?php

require_once("definitions.inc");
require_once(ToIncludes("audit")."operator.inc");

$Q = new DBQuery();

// Operators
$Q->Run(DB, "SELECT * FROM operators");

while ($Q->Next())
{
	$Data = $Q->GetArray();

	$Insert = array();

	echo "Processing ".$Data["legal_name"]."\n";

	$Operator = new operator($Data["operator_id"]);

	$Insert = $Operator->GetData();

	$New = DBRow(UPDATED_DB, "SELECT * FROM operators WHERE operator_id = ".$Data["operator_id"]);

	$Insert["mobile_1"] = $New["mobile_1"];
	$Insert["mobile_2"] = $New["mobile_2"];
	$Insert["postal_address_1"] = $New["postal_address_1"];
	$Insert["postal_address_2"] = $New["postal_address_2"];
	$Insert["postal_suburb"] = $New["postal_suburb"];
	$Insert["postal_postcode"] = $New["postal_postcode"];
	$Insert["postal_state"] = $New["postal_state"];


	$Operator->Save($Insert);

	$Contacts = DBResult(UPDATED_DB, "SELECT * FROM contacts WHERE operator_id = ".$Data["operator_id"]);

	if (is_array($Contacts))
	{
		if (sizeof($Contacts) > 0)
		{
			DB_DoQuery(DB, "DELETE FROM contacts WHERE operator_id = ".$Data["operator_id"]);
			foreach ($Contacts as $Contact)
				$Operator->SaveContact($Contact);
		}
	}

}

$Q->EndQuery();

?>

