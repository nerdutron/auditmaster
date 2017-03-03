<?php

require_once("../globals.inc");
require_once(PHPLIB_DIR."safepay_interface.inc");
require_once(PHPLIB_DIR."auth.inc");
require_once(PHPLIB_DIR."credit_cards.inc");
define("SAFEPAY_DB", "mysql://".$MY_USER.":".$MY_PASSWORD."@localhost/SAFEPAY");
//exit(1);
// no error reporting from PHP except fatal errors!
//error_reporting(E_ERROR);

$DataFile = $argv[1];
$OutputFile = $argv[2];
$LogFile = $argv[3];
$MerchantID = $argv[4];
$User = $argv[5];
$Pass = $argv[6];

define(MERCHANT_ID, $MerchantID);
define(SAFEPAY_ADMIN_USERNAME, $User);
define(SAFEPAY_ADMIN_PASSWORD, $Pass);

if(!is_file($DataFile))
{
	echo "File $DataFile Does Not Exist\r\n";
	exit(0);
}
$File = file($DataFile);

if(!count($File))
{
	echo "File Empty\r\n";
	exit(0);
}

$OutFile = fopen($OutputFile, "w");
if(!$OutFile)
{
	echo "OutputFile $OutputFile not able to be opened\n";
	exit(0);
}

foreach($File as $Line)
{
	$Line = trim($Line);
	if(!strlen($Line))
		continue;

	$LineBits = explode(",",$Line);
	$InStatus = $LineBits[7];

	$CardType = $LineBits[2];
	switch($CardType)
	{
		case "V":
			$CardType = "VC";
			break;
		case "M":
			$CardType = "MC";
			break;
		case "A":
			$CardType = "AE";
			break;
		case "D":
			$CardType = "DC";
			break;
	}
	
	$CardType = "MC";
	$CardNumber = $LineBits[1];
	$CardName = trim($LineBits[0]);
	$CardExpiry = $LineBits[3];
	$Amount = trim($LineBits[4]);
	$CallingTransID = trim($LineBits[5]);

// we only refund successful transactions
	if($InStatus != "SUCCESS")
	{
		fputs($OutFile, "$CardName,$CardNumber,$CardType,$CardExpiry,$Amount,REFUND,$CallingTransID,NOT DONE-In Status:$InStatus\r\n");
		continue;
	}

	$RetVal = DoCreditCardRefund($CardType, $CardNumber, $CardName, $CardExpiry, $Amount, "REFUND", $CallingTransID);

//fputs($OutFile, "Got retval of $RetVal\r\n");
/* GDTAG - this code commented out. It increments expiry year to try and find the right year
 * And apparently the banks don't like that much
	if($RetVal == 5 || $RetVal == 3 || $RetVal == 2 || $RetVal == 47)
	{
		// This was a card expired error
		for($i=1;$i<6;$i++)
		{
			$NewCardExpiry = $CardExpiry[2]*10 + $i+$CardExpiry[3];
			if(strlen($NewCardExpiry)==1)
				$NewCardExpiry = "0".$NewCardExpiry;
			$NewCardExpiry = $CardExpiry[0].$CardExpiry[1].$NewCardExpiry;

//fputs($OutFile, "Trying $CardNumber - $NewCardExpiry\r\n");
			$RetVal = DoCreditCardPurchase($CardType, $CardNumber, $CardName, $NewCardExpiry, $Amount, $Description, $CallingTransID);
			if(!$RetVal)
			{
				break;
			}
// If we find a new card expiry that works, we keep it to display later by reseting this variable after the break statement
			$NewCardExpiry = "";
		}
	}
*/
	if(!$RetVal || $RetVal == 0)
	{
		$Status = "SUCCESS";
	}
	elseif($RetVal == 81)
	{
//Log file failed to be opened
		fputs($OutFile, "SYSTEM ERROR - Please contact the Marketing system administrator before retrying batch\r\n");
		fclose($OutFile);
		exit(0);
	}
	elseif(in_array($RetVal,array(42,43,44,45,40,46,47,48,49,41)))
	{
		$Status = "BAD_DETAILS ($RetVal)";
		sleep(1);
	}
	elseif(in_array($RetVal, array(83,101,84,100,7)))
	{
		$Status = "CONNECT_FAILURE ($RetVal)";
		sleep(1);
	}
	else
	{
		$Status = "FAILED ($RetVal)";
		sleep(1);
	}

fputs($OutFile, "$CardName,$CardNumber,$CardType,$CardExpiry,$Amount,REFUND,$CallingTransID,$Status\r\n");
fflush($OutFile);
}

fclose($OutFile);
exit(1);
?>