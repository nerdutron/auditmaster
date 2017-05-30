<?php
// safepay_interface.inc
//
// A Class that communicates with the Safepay client
//
// Copyright MU Systems Pty Ltd 2004
//
//
// Written: 17/08/2004		Craig Sargent, Peter Hodge
//

class SafepayInterface
{
//	var $MerchantID = "MU123";
	var $HostIP = "127.0.0.1";
	var $Port = "9050";
	var $Locale = "en";
	var $Key = "MU_SYSTEMS";
	var $CardTypes = array(
		"Amex",
		"AmexPurchaseCard",
		"Bankcard",
		"BrandX",
		"BrandY",
		"Card",
		"CreditCard",
		"Dinersclub",
		"GAPcard",
		"Genericcard",
		"JCB",
		"Loyalty",
		"MasterCard",
		"Mondex",
		"PrivateLabelCard",
		"SafeDebit",
		"SmartCard",
		"Solo",
		"Style",
		"Switch",
		"VisaDebit",
		"Visa",
		"VisaPurchaseCard",
		"MC",
		"VC",
		"BC",
		"AE",
		"DC"
		);

	function SafepayInterface($SAFEPAY_DB, $LogFile, $MerchantID)
	{
// Start logging, open sockets etc
		$this->DB = $SAFEPAY_DB;
		$this->LogFile = $LogFile;
		$this->MerchantID = $MerchantID;

		// make sure the log file works!
		if(!file_exists($this->LogFile) && !@$this->_LogMessage("Creating log file"))
			return $this->_SetError("bad_logfile");

		if(!@$this->_LogMessage("\nNew SafepayInterface initialized."))
			return $this->_SetError("bad_logfile");

		// log that a new safepay interface has been started
		$this->_LogMessage("MerchantID: ".$this->MerchantID, 0);

		if(!$this->MerchantID || $this->MerchantID == "MERCHANT_ID")
			return $this->_SetError("no_merchant");

		// open the socket connection
		if(!$this->_OpenSocket())
			return $this->ErrorID;

		return 1;
	}

//
// PURCHASE
//

	function DoPurchase($Data)
	{
		// enter in the log that we are recording a new purchase
		$this->_LogMessage("Starting new DoPurchase");

		if(!$this->_RunEchoTest())
		{
			$this->_CloseSocket();
			return $this->ErrorID;
		}

		if(!$this->_SetupPurchaseData($Data))
		{
			$this->_CloseSocket();
			return $this->ErrorID;
		}

		if(!$this->_CheckPurchaseData($Data))
		{
			$this->_CloseSocket();
			return $this->ErrorID;
		}

		// try and insert the record into the database
		$this->_LogMessage("Creating entry in safepay_request...");
		$INQ = new DBInsert;
		$INQ->Save("calling_trans_id", $this->CallingTransID);
		$INQ->Save("card_nr", addslashes($this->EncryptCardNumber($this->CardNumber)));
		$INQ->Save("card_name", $this->CardName);
		$INQ->Save("card_type", $this->CardType);
		$INQ->Save("amount", $this->Amount);
		$INQ->Save("merchant_id", $this->MerchantID);
		$INQ->Save("request_time", date("Y-m-d H:i:s"));
		$INQ->Run($this->DB, "safepay_request", "INSERT");
		$this->RequestID = $INQ->GetInsertId();

		if($this->RequestID)
			$this->_LogMessage("Entry created in safepay_request with request_nr: ".$this->RequestID);
		else
		{
			$this->_CloseSocket();
			return $this->_SetError("no_request_nr");
		}


		// Call SendPurchase()
		if(!$this->_SendPurchase())
		{
			$this->_CloseSocket();
			return $this->ErrorID;
		}

		// call GetPurchaseRsp()
		if(!$this->_GetPurchaseRsp())
		{
			$this->_CloseSocket();
			return $this->ErrorID;
		}

		// return value based on error
		$this->_LogMessage("DoPurchase was successful");
		$this->_CloseSocket();
		return 1;
	}

// internalize the variables for PurchaseData($Data)
	function _SetupPurchaseData(&$Data)
	{
		// make sure all $Data array variables exist
		foreach(array("CallingTransID", "CardType", "CardNumber", "CardName", "CardExpiry", "Amount", "Description") as $Key)
		{
			if(!$Data[$Key] && $Key != "CallingTransID")
				return $this->_SetError("no_".strtolower($Key));
			$this->$Key = $Data[$Key];
			unset($Data[$Key]);

			if($Key == "CardNumber")
				$this->_LogMessage($Key.": ".$this->EncryptCardNumber($this->$Key), 0);
			else
				$this->_LogMessage($Key.": ".$this->$Key, 0);
		}

		return 1;
	}

// make sure the array of details ($Data) is valid
	function _CheckPurchaseData($Data)
	{
		// return an error if there are extra values inside $Data
		foreach($Data as $VarName => $Value)
			return $this->_SetError("extra_value", "\$Data[\"".$VarName."\"] = \"".$Value."\"");

		// Validate Data
		if(!in_array($this->CardType, $this->CardTypes))
			return $this->_SetError("bad_cardtype", $this->CardType);

		if(!is_numeric($this->CardNumber) || !CheckCCardNumber($this->CardNumber))
			return $this->_SetError("bad_cardnumber", substr($this->CardNumber, 0, 4)."...");

		// validate name
		if(strlen($this->CardName) < 4)
			return $this->_SetError("short_cardname", $this->CardName);

		// validate expiry date
		if(!CheckCCardExpiry($this->CardExpiry))
			return $this->_SetError("bad_cardexpiry", $this->CardExpiry);

		// check the amount
		if(!is_numeric($this->Amount)
			|| $this->Amount < 0
			|| $this->Amount != floor($this->Amount))
			return $this->_SetError("bad_amount", $this->Amount);

		// make sure the description isn't too long
		if(strlen($this->Description) > 34)
			return $this->_SetError("long_description", $this->Description);

		return 1;
	}

// Send all required messages for a purchase request
// Included checking the response for each line of the message
// to ensure it was correctly received by safepay
	function _SendPurchase()
	{
		// Flip card expiry to be YYMM
		$CardX = $this->CardExpiry;
		$CardExpiry = $CardX[2].$CardX[3].$CardX[0].$CardX[1];
		$Messages = array(
			"7,CardNum,".$this->CardNumber,
			"7,CardExp,".$CardExpiry,
			"7,MerchTxnRef,".$this->RequestID,
			);

		$Order = array(
			6,
			$this->Description,
			$this->MerchantID,
			$this->Amount,
			$this->Locale,
			"",
			);
		$Messages[] = implode(",", $Order);
		$Messages[] = "5,";


		foreach($Messages as $Message)
		{
			$this->_SendMessage($Message);
			$Response = trim($this->_GetMessage());
			if($Response != "1,1")
			{
				if(substr($Message,0,1) == "6")
				{
					$this->_SendMessage("4,PaymentClient.Error");
					$Response = trim($this->_GetMessage());
					return $this->_SetError("send_moto_failed", $Response);
				}
				return $this->_SetError("bad_rsp", $Response);
			}
		}

		return 1;
	}

// Get all the required fields for a purchase request response
	function _GetPurchaseRsp()
	{
		$this->Response = "1,QSIResponseCode";

		$this->_SendMessage("4,DigitalReceipt.QSIResponseCode");
		$Response = $this->_GetMessage();
		if($Response == "0,0")
			return $this->_SetError("no_rsp_value", $FieldName);

		$ResponseArray = explode(",", $Response);
		$QSIResponseCode = trim($ResponseArray[1]);

		$this->_SaveResponseData("QSIResponseCode", $QSIResponseCode);
		if($QSIResponseCode != "0")
		{
// GDTAG - we could do this here, but I want all the extra info back, and it seems we can
// possibly get it, so I will do this check later..
//			return $this->_SetError("rsp_qsi_code_$QSIResponseCode", $QSIResponseCode);
		}

		$Fields = array(
//			"QSIResponseCode",
			"ReceiptNo",
			"MerchTxnRef",
			"OrderInfo",
			"MerchantId",
			"PurchaseAmountInteger",
			"Locale",
//			"AcqResponseCode",
			"TransactionNo",
//			"AuthoriseId",
			"BatchNo",
			"CardType",
			);

// We need an extra field if a certain type of error
		if($QSIResponseCode == "7")
		{
			$Fields[] = "ERROR";
		}
/*
$Responses = array(
"MerchTxnRef"				=> $this->RequestID."x",
"OrderInfo"					=> $this->Description,
"MerchantId"				=> $this->MerchantID,
"PurchaseAmountInteger"		=> $this->Amount,
"Locale"					=> $this->Locale,
"QSIResponseCode"			=> 7, // 0,
"TransactionNo"				=> 123456,
"BatchNo"					=> 987,
"CardType"					=> $this->CardType,
);
*/
		foreach($Fields as $FieldName)
		{
			$this->Response = "1,".$Responses[$FieldName];

			$this->_SendMessage("4,DigitalReceipt.".$FieldName);
			$Response = $this->_GetMessage();
			if($Response == "0,0")
				return $this->_SetError("no_rsp_value", $FieldName);

			$ResponseArray = explode(",", $Response);
			$$FieldName = trim($ResponseArray[1]);
			$this->_SaveResponseData($FieldName, $$FieldName);
		}

		// save the QSI response code and TransactionNo into the database
		$INQ = new DBInsert;
		$INQ->SaveWhere("request_nr", $this->RequestID);
		$INQ->Save("result", trim($QSIResponseCode));
		$INQ->Save("safepay_reference", $TransactionNo);
		$INQ->Run($this->DB, "safepay_request", "UPDATE");
		$this->_LogMessage("Storing QSIResponseCode '".$QSIResponseCode."'");
		$this->_LogMessage("Storing TransactionNo '".$TransactionNo."'", 0);


		if($QSIResponseCode != "0")
		{
			return $this->_SetError("rsp_qsi_code_$QSIResponseCode", $QSIResponseCode);
		}

		// test the response values
		if((string)$MerchTxnRef != (string)$this->RequestID)
		{
			return $this->_SetError("rsp_request_nr", $MerchTxnRef.":".$this->RequestID);
		}

		if($OrderInfo != $this->Description)
			return $this->_SetError("rsp_description", $OrderInfo);

		if($MerchantId != $this->MerchantID)
			return $this->_SetError("rsp_merchant_id", $MerchantId);

		if((string)$PurchaseAmountInteger != (string)$this->Amount)
			return $this->_SetError("rsp_amount", $PurchaseAmountInteger);

		if($Locale != $this->Locale)
			return $this->_SetError("rsp_locale", $Locale);

/*This out for now as the card types we get back are codes...peter did not take this into consideration
		if($CardType != $this->CardType)
			return $this->_SetError("rsp_cardtype", $CardType);
*/

		return 1;
	}

//
// VOID PURCHASE
//

	function DoVoidPurchase($Data)
	{
		// Validate Data
		// Setup data as member variables
		// Call SendVoidPurchase()
		// call GetVoidPurchaseRsp()
		// return value based on error
	}

// Send all required messages for a purchase request
// Included checking the response for each line of the message
// to ensure it was correctly received by safepay
	function _SendVoidPurchase()
	{
	}

// Get all the required fields for a purchase request response
	function _GetVoidPurchaseRsp()
	{
	}

//
// SOCKET CONNECTION
//

// send a message to the socket and log the message
	function _SendMessage($Message)
	{
		$Message .= "\n";
		fwrite($this->Socket, $Message, strlen($Message));

		$this->_DBLogMessage("Sent ... : ".str_replace("\n", "\\n", $Message));
	}

	function _GetMessage()
	{
		$Message = $this->Response;
		$Message = fgets($this->Socket);
		$this->_DBLogMessage("Received : ".$Message);

		// if there was no response at all, that's not right
		if(!$Message)
			return $this->_SetError("no_rsp");

		return $Message;
	}

	function _OpenSocket()
	{
		$this->Socket = fsockopen($this->HostIP, $this->Port, $ErrorNo, $ErrorString);
		if($ErrorNo)
			return $this->_SetError("socket_open", $ErrorNo." ".$ErrorString);

		return 1;
	}

	function _CloseSocket()
	{
		if(!fclose($this->Socket))
			return $this->_SetError("socket_close");

		return 1;
	}

//
// CREDIT CARD ENCRYPTION
//

	function EncryptCardNumber($Data)
	{
		$Key = $this->Key;
		// Key can be up to 255 characters

		$Pwd = $Key;
		$PwdLength = strlen($Pwd);

		for ($i = 0; $i < 255; $i++)
		{
			$Key[$i] = ord($Pwd[(($i % $PwdLength)+1)]);
			//      $Key[$i] = ord(substr($Pwd, ($i % $PwdLength)+1, 1));
			$Counter[$i] = $i;
		}

		for ($i = 0; $i < 255; $i++)
		{
			$x = ($x + $Counter[$i] + $Key[$i]) % 256;
			$TempSwap = $Counter[$i];
			$Counter[$i] = $Counter[$x];
			$Counter[$x] = $TempSwap;
		}

		$DataLen = strlen($Data);
		for ($i = 0; $i < $DataLen; $i++)
		{
			$a = ($a + 1) % 256;
			$j = ($j + $Counter[$a]) % 256;
			$Temp = $Counter[$a];
			$Counter[$a] = $Counter[$j];
			$Counter[$j] = $Temp;
			$k = $Counter[(($Counter[$a] + $Counter[$j]) % 256)];
			//      $Zcipher = ord(substr($Data, $i, 1)) ^ $k;
			$Zcipher = ord($Data[$i]) ^ $k;
			$Zcrypt .= chr($Zcipher);
		}
		return $Zcrypt;
	}

//
// LOGGING
//

// add a message to the log file
	function _LogMessage($Message = "", $ShowDate = 1)
	{
		if($Message)
			$Message = ($ShowDate ? date("Y-m-d H:i:s") : "                   ")."\t".$Message;
		$Message .= "\n";

		$F = fopen($this->LogFile, "a+");
		$Write = fwrite($F, $Message, strlen($Message));
		fclose($F);

		return $Write;
	}

// add a message to the database for this request id
// and also to the log file
	function _DBLogMessage($Message)
	{
		// save a log message in the database, and then in the log file
		if($this->RequestID)
		{
			$INQ = new DBInsert;

			// work out whether to insert to the log or update it
			if(!$this->DBLogExists)
			{
				$INQ = new DBInsert;
				$INQ->Save("request_nr", $this->RequestID);
				$INQ->Save("transmission_data", $Message);
				$INQ->Run($this->DB, "safepay_request_data", "INSERT");
				$this->DBLogExists = 1;
			}
			else
			{
				$Query = "UPDATE safepay_request_data"
					." SET transmission_data = CONCAT(transmission_data, '\n".addslashes($Message)."')"
					." WHERE request_nr = ".$this->RequestID;
				DB_DoQuery($this->DB, $Query);
			}
		}

		$this->_LogMessage($Message);
	}

//
// ERROR HANDLING
//

// lookup the number and description for an error code
// internalize the number, code, and description
// log the error
// if the error code couldn't be looked up, then set the error to unknown_error
	function _SetError($Code, $Value = "")
	{
		$this->ErrorCode = $Code;

		// lookup the error id
		$Query = "SELECT error_id, error_string FROM safepay_error_codes WHERE error_code = '".$this->ErrorCode."'";
		$Found = DBRow($this->DB, $Query);
		$this->ErrorID = $Found["error_id"];
		$this->ErrorString = $Found["error_string"];

		// if the error code could not be found, we have an unknown error
		if(!sizeof($Found))
		{
			// if we couldn't look up the "unknown_error" error, then there is database trouble
			// return -1 if we can't even lookup the real error codes
			if($Code == "unknown_error")
			{
				$this->ErrorID = -1;
				$this->ErrorCode = "error_lookup";
				$this->ErrorString = "Can't lookup error code";
			}
			// otherwise, say that the error code is unknown
			else
				return $this->_SetError("unknown_error", $Code);
		}

		$this->ErrorString = str_replace("%VALUE%", $Value, $this->ErrorString);

		// log the error (as long as the error wasn't a bad logfile!)
		if($Code != "bad_logfile")
			$this->_LogMessage("ERROR ".$this->ErrorID." ".$this->ErrorCode.": ".$this->ErrorString);

		// put the error into the database log
		if($this->RequestID)
		{
			$INQ = new DBInsert;
			$INQ->Save("error_id", $this->ErrorID);
			$INQ->Save("error_value", $Value);
			$INQ->SaveWhere("request_nr", $this->RequestID);
			$INQ->Run($this->DB, "safepay_request", "UPDATE");
		}

		// return the error number
		exit ($this->ErrorID);

		// a return status of 0 for failure
		return 0;
	}

	function _RunEchoTest()
	{
		$this->_SendMessage("1,Test");
		$Response = trim($this->_GetMessage());
		$this->_LogMessage("Echo Test Response - $Response");
		if($Response != "1,echo:Test")
		{
			return $this->_SetError("echo_fail", $Response);
		}

		return 1;
	}

	function _SaveResponseData($FieldName, $Value)
	{
		if($this->RequestID)
		{
			$INQ = new DBInsert;
			$INQ->Add("INT","request_nr", $this->RequestID);
			$INQ->Add("VARCHAR","field_name", $FieldName);
			$INQ->Add("VARCHAR","value", $Value);
			$INQ->Run($this->DB, "safepay_response_data", "INSERT");
		}
	}
	function GetErrorID() { return $this->ErrorID; }
	function GetErrorCode() { return $this->ErrorCode; }
	function GetErrorString() { return $this->ErrorString; }

	function DoRefund($Data)
	{
		// enter in the log that we are recording a new purchase
		$this->_LogMessage("Starting new DoRefund");

		if(!$this->_RunEchoTest())
		{
			$this->_CloseSocket();
			return $this->ErrorID;
		}

		if(!$this->_SetupRefundData($Data))
		{
			$this->_CloseSocket();
			return $this->ErrorID;
		}

		if(!$this->_CheckRefundData($Data))
		{
			$this->_CloseSocket();
			return $this->ErrorID;
		}

		// try and insert the record into the database
		$this->_LogMessage("Creating entry in safepay_request...");
		$INQ = new DBInsert;
		$INQ->Add("CHAR","calling_trans_id", $this->CallingTransID);
		$INQ->Add("CHAR","card_nr", "REFUND");
		$INQ->Add("CHAR","card_name", "TransRef: $this->TransactionNr");
		$INQ->Add("CHAR","card_type", "");
		$INQ->Add("INT","amount", $this->Amount);
		$INQ->Add("CHAR","merchant_id", $this->MerchantID);
		$INQ->Add("DATETIME","request_time", date("Y-m-d H:i:s"));
		$INQ->Run($this->DB, "safepay_request", "INSERT");
		$this->RequestID = $INQ->GetInsertId();

		if($this->RequestID)
			$this->_LogMessage("Entry created in safepay_request with request_nr: ".$this->RequestID);
		else
		{
			$this->_CloseSocket();
			return $this->_SetError("no_request_nr");
		}


		// Call SendPurchase()
		if(!$this->_SendRefund())
		{
			$this->_CloseSocket();
			return $this->ErrorID;
		}

		// call GetPurchaseRsp()
		if(!$this->_GetRefundRsp())
		{
			$this->_CloseSocket();
			return $this->ErrorID;
		}

		// return value based on error
		$this->_LogMessage("DoRefund was successful");
		$this->_CloseSocket();
		return 1;
	}

// internalize the variables for PurchaseData($Data)
	function _SetupRefundData(&$Data)
	{
		// make sure all $Data array variables exist
		foreach(array("CallingTransID", "TransactionNr", "Amount") as $Key)
		{
			if(!$Data[$Key] && $Key != "CallingTransID")
				return $this->_SetError("no_".strtolower($Key));
			$this->$Key = $Data[$Key];
			unset($Data[$Key]);

			$this->_LogMessage($Key.": ".$this->$Key, 0);
		}
		$this->UserName = SAFEPAY_ADMIN_USERNAME;
		$this->Password = SAFEPAY_ADMIN_PASSWORD;
		return 1;
	}

// make sure the array of details ($Data) is valid
	function _CheckRefundData($Data)
	{
		// return an error if there are extra values inside $Data
		foreach($Data as $VarName => $Value)
			return $this->_SetError("extra_value", "\$Data[\"".$VarName."\"] = \"".$Value."\"");

		// Validate Data

		// check the amount
		if(!is_numeric($this->Amount)
			|| $this->Amount < 0
			|| $this->Amount != floor($this->Amount))
			return $this->_SetError("bad_amount", $this->Amount);

		// make sure the description isn't too long
		if(!$this->TransactionNr)
			return $this->_SetError("Bad Transaction Nr", $this->TransactionNr);

		if(!$this->UserName || !$this->Password)
			return $this->_SetError("Bad Auth Details", $this->UserName." - ".$this->Password);
	
		return 1;
	}

// Send all required messages for a purchase request
// Included checking the response for each line of the message
// to ensure it was correctly received by safepay
	function _SendRefund()
	{
		$Messages = array(
			"28,MerchTxnRef,".$this->RequestID,
			);

		$Order = array(
			15,
			$this->MerchantID,
			$this->TransactionNr,
			$this->Amount,
			$this->UserName,
			$this->Password,
//			"",
			);
		$Messages[] = implode(",", $Order);
		$Messages[] = "5,";


		foreach($Messages as $Message)
		{
			$this->_SendMessage($Message);
			$Response = trim($this->_GetMessage());
			if($Response != "1,1")
			{
				if(substr($Message,0,1) == "6")
				{
					$this->_SendMessage("4,PaymentClient.Error");
					$Response = trim($this->_GetMessage());
					return $this->_SetError("send_refund_failed", $Response);
				}
				return $this->_SetError("bad_rsp", $Response);
			}
		}

		return 1;
	}

// Get all the required fields for a purchase request response
	function _GetRefundRsp()
	{
		$this->_SendMessage("4,DigitalReceipt.Result");
		$Result = $this->_GetMessage();
		$this->_SendMessage("4,DigitalReceipt.QSIResponseCode");
		$Response = $this->_GetMessage();
		if(substr($Response,0,1) == "0")
			return $this->_SetError("no_rsp_value", $FieldName);

		$QSIResponseCode = trim(substr($Response,2));

		$this->_SaveResponseData("QSIResponseCode", $QSIResponseCode);
		if($QSIResponseCode != "0")
		{
// GDTAG - we could do this here, but I want all the extra info back, and it seems we can
// possibly get it, so I will do this check later..
//			return $this->_SetError("rsp_qsi_code_$QSIResponseCode", $QSIResponseCode);
		}

		$Fields = array(
			"MerchantId",
			"TransactionNo",
			"ReceiptNo",
			"AcqResponseCode",
			"AuthoriseId",
			"BatchNo"
			);

// We need an extra field if a certain type of error
		if($QSIResponseCode == "7")
		{
			$Fields[] = "ERROR";
		}
		foreach($Fields as $FieldName)
		{
//			$this->Response = "1,".$Responses[$FieldName];

			$this->_SendMessage("4,DigitalReceipt.".$FieldName);
			$Response = $this->_GetMessage();
			if($Response == "0,0")
				return $this->_SetError("no_rsp_value", $FieldName);

			$ResponseArray = explode(",", $Response);
			$$FieldName = trim($ResponseArray[1]);
			$this->_SaveResponseData($FieldName, $$FieldName);
		}

		// save the QSI response code and TransactionNo into the database
		$INQ = new DBInsert;
		$INQ->SaveWhere("request_nr", $this->RequestID);
		$INQ->Save("result", trim($QSIResponseCode));
		$INQ->Save("safepay_reference", $TransactionNo);
		$INQ->Run($this->DB, "safepay_request", "UPDATE");
		$this->_LogMessage("Storing QSIResponseCode '".$QSIResponseCode."'");
		$this->_LogMessage("Storing TransactionNo '".$TransactionNo."'", 0);
		$this->_LogMessage("Storing Result'".$Result."'", 0);


		if($QSIResponseCode != "0")
		{
			return $this->_SetError("rsp_qsi_code_$QSIResponseCode", $QSIResponseCode);
		}


		if($MerchantId != $this->MerchantID)
			return $this->_SetError("rsp_merchant_id", $MerchantId);

		return 1;
	}

	function FindRefundDetails($Data, $Count=0)
	{
		$Qry = "SELECT * FROM safepay_request sr JOIN safepay_request_data srd ON sr.request_nr = srd.request_nr ";
		$Qry .= "WHERE card_name = '".addslashes($Data["CardName"])."' ";
		$Qry .= " AND amount = ".$Data["Amount"];
		$Qry .= " AND transmission_data LIKE 'Sent ... : 7,CardNum,".$Data["CardNumber"]."%' ";
		$Qry .= " AND merchant_id = '$this->MerchantID' ";
		$Qry .= "AND safepay_reference IS NOT NULL ";
		$Qry .= " ORDER BY sr.request_nr DESC limit 1";

		$Q = new DBQuery;
		$Q->Run($this->DB, $Qry);

		if(!$Q->Next())
		{
			return 0;
		}
		$NewData = array();
		$NewData["TransactionNr"] = $Q->GetField("safepay_reference");
		$NewData["Amount"] = $Data["Amount"];

//echo "Found TNr: ".$NewData["TransactionNr"];
		return $NewData;
	}
}


function CheckCCardNumber($CardNr)
{
	$CardNr = "$CardNr";

	$Len = strlen($CardNr);

	//echo "CardNr: $CardNr - Length $Len<BR>\n";
	if($Len < 14)
		return 0;

	$Card = strrev($CardNr);

	//echo "Card REV: $Card<BR>\n";
	$FF = 1;
	for($i=0;$i<$Len;$i++)
	{
		$Val = $Card[$i] * $FF;
		//echo "VAL: $Val - ";
		if($Val > 9)
		{
			$ValEnd = 0;
			$ValStr = "$Val";
			$ValLen = strlen($ValStr);
			//echo "VALLEN: $ValLen - ";
			for($j=0;$j<$ValLen;$j++)
			{
				$ValEnd += $ValStr[$j];
				//echo "ValEnd - $ValEnd - ValPt: ".$ValStr[$j]." - ";
			}
			$Val = $ValEnd;
		}

		$Total += $Val;

		$FF++;
		if($FF > 2)
			$FF = 1;

		//echo "CN $i:$Card[$i] - Val: $Val - TOT:$Total - FF:$FF<BR>\n";
	}

	while($Total >= 10)
		$Total -= 10;
	//echo "CD: $CardNr -  $Total<BR>\n";
	if($Total == 0)
		return 1;
	return 0;
}

function CheckCCardExpiry($Expiry)
{
	if(strlen($Expiry) != 4)
		return 0;

	$Month = substr($Expiry, 0, 2);
	$Year = substr($Expiry, 2, 2);
	if(!is_numeric($Year) || !is_numeric($Month))
		return 0;

	$Month *= 1;
	$Year *= 1;

	// month must be between 1 and 12
	if($Month < 1 || $Month > 12)
		return 0;

/*
	// if year is this year, the month must be greater than this month
	if($Year == date("y") && $Month < date("m"))
		return 0;

	// if year is less than this year, it's not valid
	if($Year < date("y"))
		return 0;
*/

	return 1;
}

?>
