#!/usr/local/bin/php5
<?php
//exit(1);
// no error reporting from PHP except fatal errors!
//error_reporting(E_ERROR);

// make sure the script doesn't run forever
set_time_limit(10);

// ignore first value in argv
array_shift($argv);

$LogFile = array_shift($argv);

// next argument is the merchant id
$MerchantID = array_shift($argv);

$UserName = array_shift($argv);
$Password = array_shift($argv);
DEFINE(SAFEPAY_ADMIN_USERNAME,$UserName);
DEFINE(SAFEPAY_ADMIN_PASSWORD,$Password);

// third argument is whether we already have a TransactionNr to use (0 or transactionNr)
$TransactionNr = array_shift($argv);

// Expect different order and type of parameters if we have a trans nr
if($TransactionNr)
{
	$Data["TransactionNr"] = $TransactionNr;
	$DataParameters = array("UserName","Password","Amount");
}
else
{
	$DataParameters = array(
			"CardType",
			"CardNumber",
			"CardName",
			"CardExpiry",
			"Amount",
			"Description",
			);
}
// first argument is the log file
foreach($DataParameters as $VarName)
	$Data[$VarName] = array_shift($argv);

define("PHPLIB_DIR", dirname($_SERVER["SCRIPT_NAME"])."/");
require_once(PHPLIB_DIR."safepay_interface.inc");
require_once(PHPLIB_DIR."auth.inc");
define("SAFEPAY_DB", "mysql://".$MY_USER.":".$MY_PASSWORD."@localhost/SAFEPAY");

$SI = new SafepayInterface(SAFEPAY_DB, $LogFile, $MerchantID);
if($SI->GetErrorID())
	exit ((string)$SI->GetErrorID());

// If we don't have the transaction nr we have to attempt to fnd it
if(!$TransactionNr)
	$Data = $SI->FindRefundDetails($Data);

if(!$Data)
	exit("-1");

// add a new credit card transaction
$SI->DoRefund($Data);
if($SI->GetErrorID())
	exit ((string)$SI->GetErrorID());

exit ("1");

?>
