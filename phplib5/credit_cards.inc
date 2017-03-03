<?php
require_once(PHPLIB_DIR."../globals.inc");
//
//
// Functions and other useful stuff for Credit Card Handling
//
// Copyright MU Systems Pty Ltd 2004
//
// Written: 17/8/04		Craig Sargent

//
// Call the php script to do the safepay transaction
// and interpret the result.
// Log details
function DoCreditCardPurchase($CardType, $CardNr, $Name, $Expiry, $Amount, $Details, $CallingTransID="")
{
	if (defined("SAFEPAY_FORCE_RESPONSE"))
	{
		if (trim(SAFEPAY_FORCE_RESPONSE))
			$Responses = explode("|", SAFEPAY_FORCE_RESPONSE);
		else
			$Responses = array();
		if (sizeof($Responses))
		{
			shuffle($Responses);
			$Result = array_pop($Responses);
			if($Result == 1)
				return;
			return ($Result);
		}
	}

	// convert the amount to cents if it is numeric
	if(is_numeric($Amount))
		$Amount *= 100;

	$Command = "/usr/local/bin/php ".PHPLIB_DIR."safepay.php";
	$Parameters = array(SAFEPAY_LOG.".".MERCHANT_ID, MERCHANT_ID, $CardType, $CardNr, $Name, $Expiry, $Amount, $Details, $CallingTransID);
	foreach($Parameters as $Arg)
		$Command .= " \"".str_replace("\"","",$Arg)."\"";

	$Result = shell_exec($Command);

	if($Result == 1)
		return;
	return ($Result);
}

// GetFriendlyError - function to return a friendly version of the supplied error code
function GetFriendlyError($ErrorCode)
{
	$MessageQuery = "SELECT error_friendly FROM safepay_error_codes WHERE error_id = ".$ErrorCode;
	$Friendly = DBGet(SAFEPAY_DB, $MessageQuery);

	if (!$Friendly)
		$Friendly = "Internal error";

	return $Friendly;
}

// GetErrorAction - function to return the suggested action for a particular error
function GetErrorAction($ErrorCode)
{
	$Qry = "SELECT error_action FROM safepay_error_codes WHERE error_id = ".$ErrorCode;
	$Action = DBGet(SAFEPAY_DB, $Qry);

	return ($Action);
}

function DoCreditCardRefund($CardType, $CardNr, $Name, $Expiry, $Amount, $Details)
{
	if(is_numeric($Amount))
		$Amount *= 100;

	$Command = "/usr/local/bin/php ".PHPLIB_DIR."safepay_refund.php";
	$Parameters = array(SAFEPAY_LOG.".".MERCHANT_ID, MERCHANT_ID, SAFEPAY_ADMIN_USERNAME, SAFEPAY_ADMIN_PASSWORD,"0",$CardType, $CardNr, $Name, $Expiry, $Amount, $Details, $CallingTransID);
	foreach($Parameters as $Arg)
		$Command .= " \"".$Arg."\"";

	$Result = shell_exec($Command);

	if($Result == 1)
		return;

	if(!$Result)
		return -1;

	return ($Result);
}

?>
