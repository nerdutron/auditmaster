#!/usr/local/bin/php5
<?php
//exit(1);
// no error reporting from PHP except fatal errors!
//error_reporting(E_ERROR);

// make sure the script doesn't run forever
set_time_limit(10);

$DataParameters = array(
	"CardType",
	"CardNumber",
	"CardName",
	"CardExpiry",
	"Amount",
	"Description",
	);

// ignore first value in argv
array_shift($argv);

// first argument is the log file
$LogFile = array_shift($argv);

// next argument is the merchant id
$MerchantID = array_shift($argv);

foreach($DataParameters as $VarName)
	$Data[$VarName] = array_shift($argv);

define("PHPLIB_DIR", dirname($_SERVER["SCRIPT_NAME"])."/");
require_once(PHPLIB_DIR."safepay_interface.inc");
require_once(PHPLIB_DIR."auth.inc");
define("SAFEPAY_DB", "mysql://".$MY_USER.":".$MY_PASSWORD."@localhost/SAFEPAY");

$SI = new SafepayInterface(SAFEPAY_DB, $LogFile, $MerchantID);
if($SI->GetErrorID())
	exit ((string)$SI->GetErrorID());


// add a new credit card transaction
$SI->DoPurchase($Data);
if($SI->GetErrorID())
	exit ((string)$SI->GetErrorID());

exit ("1");

?>
