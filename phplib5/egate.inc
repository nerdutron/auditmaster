<?php
// egate.inc
//
// A Class that communicates with the EGate client
//
// Copyright MU Systems Pty Ltd 2004
//
//
// Written: 17/08/2004		Craig Sargent, Peter Hodge
//

class EGate
{
//	var $MerchantID = "TESTANZONLEGAL";
	var $HostIP = "127.0.0.1";
	var $Port = "9051";
	var $Locale = "en";

	function EGate($EGATE_DB, $LogFile)
	{
// Start logging, open sockets etc
		$this->DB = $EGATE_DB;
		$this->LogFile = $LogFile;
		$this->MerchantID = MERCHANT_ID;

		// make sure the log file works!
		if(!file_exists($this->LogFile) && !@$this->_LogMessage("Creating log file"))
			return $this->_SetError("bad_logfile");

		if(!@$this->_LogMessage("\n\n".str_repeat("=", 40), 0))
			return $this->_SetError("bad_logfile");

		// log that a new egate interface has been started
		$this->_LogMessage("New EGate class initialized");

		return 1;
	}

//
// PURCHASE
//

	function SendDigitalOrder($Reference, $Amount, $Description, $ReturnURL)
	{
		// enter in the log that we are recording a new purchase
		$this->_LogMessage("Starting SendDigitalOrder()");
		$this->_LogMessage("  Reference: ".$Reference, 0);
		$this->_LogMessage("     Amount: ".$Amount, 0);
		$this->_LogMessage("Description: ".$Description, 0);
		$this->_LogMessage("  ReturnURL: ".$ReturnURL, 0);

		// open the socket connection
		if(!$this->_OpenSocket())
			return 0;

		if(strlen($Description) > 30)
			$Description = substr($Description,0,30);
		// try and insert the record into the database
		$this->_LogMessage("Creating entry in egate_request...");
		$INQ = new DBInsert;
		$INQ->Save("amount", $Amount);
		$INQ->Save("merchant_id", $this->MerchantID);
		$INQ->Save("request_time", date("Y-m-d H:i:s"));
		$INQ->Save("egate_reference", $Reference);
		$INQ->Save("description", $Description);
		$INQ->Run($this->DB, "egate_request", "INSERT");
		$this->RequestID = $INQ->GetInsertId();

		if($this->RequestID)
			$this->_LogMessage("Entry created in egate_request with request_nr: ".$this->RequestID);
		else
		{
			$this->_CloseSocket();
			$this->_SetError("no_request_nr");
			return 0;
		}

		// setup the merchant transaction reference number
		$this->_SendMessage("7,MerchTxnRef,{$this->RequestID}");
		$Response = $this->_GetMessage();
		if($Response != "1,1")
		{
			$this->_CloseSocket();
			$this->_SetError("bad_rsp", $Response);
			return 0;
		}

// If we have variables already in the return url
		if(!strpos($ReturnURL,"?"))
			$ReturnURL .= "?REQ_ID=$this->RequestID";
		else
			$ReturnURL .= "&REQ_ID=$this->RequestID";

		// send the digital order
		$this->_SendMessage("2,$Description,$this->MerchantID,$Amount,$this->Locale,$ReturnURL");
		$Response = trim($this->_GetMessage());
		if(substr($Response, 0, 1) != "1")
		{
			// there was an error in sending the digital order
			$this->_GetMessageError($Response);
			return 0;
		}

		// we now have a ReturnURL (digital order) to redirect to
		$DigitalOrder = substr($Response, 2);
		$this->_LogMessage("Digital Order: ".$DigitalOrder);
		$INQ = new DBInsert;
		$INQ->Save("digital_order", $DigitalOrder);
		$INQ->SaveWhere("request_nr", $this->RequestID);
		$INQ->Run($this->DB, "egate_request", "UPDATE");

		if(!$this->_CloseSocket())
			return 0;

		// return value based on error
		$this->_LogMessage("SendDigitalOrder() was successful");
		return $DigitalOrder;
	}

	// return an array containing the different
	// parts of the digital receipt
	function DecryptDigitalReceipt($DigitalReceipt, $RequestID="")
	{
		if($RequestID)
			$this->RequestID = $RequestID;

		$this->_LogMessage("Starting DecryptDigitalReceipt()");
		$this->_LogMessage("Digital Receipt: ".$DigitalReceipt, 0);
		$this->_LogMessage("Request ID : ".$RequestID, 0);

		// make sure the digital receipt isn't empty
		if(!trim($DigitalReceipt))
			return $this->_SetError("no_dr", $DigitalReceipt);

		if(!$this->_OpenSocket())
			return 0;

		// send the digital receipt to be decrypted by the payment client
		$this->_SendMessage("3,$DigitalReceipt");
		$Response = $this->_GetMessage();
		if($Response != "1,1")
			// there was an error in decrypting the digital order
			return $this->_GetMessageError($Response);

		$this->_SendMessage("5");
		$Response = $this->_GetMessage();
		if($Response != "1,1")
			// there was an error in decrypting the digital order
			return $this->_GetMessageError($Response);

		// get the parts of the digital receipt from the payment client
		$this->_SendMessage("4,DigitalReceipt.QSIResponseCode");
		$Response = $this->_GetMessage();
		if(substr($Response, 0, 1) != "1")
		{
			$this->_CloseSocket();

			// could not get the QSI response code
			return $this->_SetError("bad_rsp", $Response);
		}

		$QSIResponseCode = substr($Response, 2);

		// any QSI response code other than 0 is a failure
		if($QSIResponseCode)
		{
			// make sure the response code is not a payment client error
			if($QSIResponseCode == 7)
			{
				$this->_SendMessage("4,DigitalReceipt.ERROR");
				$Response = $this->_GetMessage();

				// close socket as function is about to fail
				$this->_CloseSocket();

				return $this->_SetError("rsp_qsi_code_$QSIResponseCode", $Response);
			}

			// close socket as function is about to fail
			$this->_CloseSocket();

			return $this->_SetError("rsp_qsi_code_$QSIResponseCode", $QSIResponseCode);
		}

		$Receipt = array("QSIResponseCode" => $QSIResponseCode);

		// get other fields and add them to the receipt
		$Fields = array(
			"MerchTxnRef" => "MerchTxnRef",
			"MerchantId" => "MerchantID",
			"OrderInfo" => "Description",
			"ReceiptNo" => "ReceiptNumber",
			"TransactionNo" => "TransNumber",
			"PurchaseAmountInteger" => "PurchaseAmountInteger",
			"AcqResponseCode" => "ResponseCode",
			"AuthorizeId" => "AuthID",
			"BatchNo" => "BatchNumber",
			);
		
		foreach($Fields as $FieldName => $Key)
		{
			$this->_SendMessage("4,DigitalReceipt.$FieldName");
			$Response = $this->_GetMessage();
			$Receipt[$Key] = $Response[0] === "1" ? substr($Response, 2) : "Unknown";
		}

		// save the transaction number
		$INQ = new DBInsert;
		$INQ->Save("transaction_number", $Receipt["TransNumber"]);
		$INQ->SaveWhere("request_nr", $this->RequestID);
		$INQ->Run($this->DB, "egate_request", "UPDATE");

		if(!$this->_CloseSocket())
			return 0;

		// save an error code of 1 for success
		$INQ = new DBInsert;
		$INQ->Save("error_id", 1);
		$INQ->SaveWhere("request_nr", $this->RequestID);
		$INQ->Run($this->DB, "egate_request", "UPDATE");

		$this->_LogMessage("SendDigitalOrder() was successful");
		return $Receipt;
	}

//
// SOCKET CONNECTION
//

// send a message to the socket and log the message
	// optionally, you can send all the parts of the message as individual arguments
	function _SendMessage($Message)
	{
		$Message .= "\n";
		fwrite($this->Socket, $Message, strlen($Message));

		$this->_DBLogMessage("Sent ... : ".str_replace("\n", "\\n", $Message));
	}

	function _GetMessage()
	{
		$Message = $this->Response;
		$Message = trim(fgets($this->Socket));
		$this->_DBLogMessage("Received : ".$Message);

		// if there was no response at all, that's a problem
//		if(!$Message)
//			return $this->_SetError("no_rsp");

		return (string)$Message;
	}

	function _GetMessageError($Response)
	{
		$this->_SendMessage("4,PayentClient.Error");
		$ErrorResponse = $this->_GetMessage();

		$this->_CloseSocket();

		// show the PaymentClient.error if possible, otherwise a
		// generic "send DO failed" error will do
		if(substr($ErrorResponse, 0, 1) == "1")
			return $this->_SetError("pc_error", substr($ErrorResponse, 2));

		return $this->_SetError("bad_rsp", $Response);
	}

	function _OpenSocket()
	{
		$this->_LogMessage("Opening Socket Connection");

//		$this->Socket = fopen("/tmp/egate_socket", "a+");

		$this->Socket = @fsockopen($this->HostIP, $this->Port, $ErrorNo, $ErrorString);
		if($ErrorNo)
			return $this->_SetError("socket_open", $ErrorNo." ".$ErrorString);

		// do an "echo test" on the socket
		$this->_SendMessage("1,test");
		$Response = $this->_GetMessage();
		if($Response != "1,echo:test")
			return $this->_SetError("socket_test", $Response);

		return 1;
	}

	function _CloseSocket()
	{
		$this->_LogMessage("Closing socket");
		if(!fclose($this->Socket))
			return $this->_SetError("socket_close");

		return 1;
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
			$Q = new DBQuery;
			$INQ = new DBInsert;

			$Qry = "SELECT transmission_data FROM egate_request_data WHERE request_nr=$this->RequestID";

			$Q->Run($this->DB,$Qry);
			if($Q->Next())
			{
				$TData = $Q->GetField("transmission_data")."\n".$Message;
				$INQ->AddWhere("INT","request_nr", $this->RequestID);
				$RequestType = "UPDATE";
			}
			else
			{
				$TData = $Message;
				$RequestType = "INSERT";
				$INQ->Add("INT","request_nr", $this->RequestID);
			}

			$INQ->Add("VARCHAR","transmission_data", $TData);
			$INQ->Run($this->DB, "egate_request_data", $RequestType);

			$INQ->EndQuery();
			$Q->EndQuery();
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
		$Query = "SELECT error_id, error_string, error_friendly ";
		$Query .= "FROM egate_error_codes WHERE error_code = '".$this->ErrorCode."'";
		$Found = DBRow($this->DB, $Query);
		$this->ErrorID = $Data["error_id"];
		$this->ErrorString = $Data["error_string"];
		$this->ErrorFriendly = $Data["error_friendly"];

		// if the error code could not be found, we have an unknown error
		if(sizeof($Found))
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
			$INQ->Run($this->DB, "egate_request", "UPDATE");
		}

		// a return status of 0 for failure
		return 0;
	}

	function GetErrorID() { return $this->ErrorID; }
	function GetErrorCode() { return $this->ErrorCode; }
	function GetErrorString() { return $this->ErrorString; }
}


//
// EGateTest
//
// this class allows a user to simulate being the PaymentClient
// by replacing the socket connection calls with output/input prompts
// this only works from a command-line script
//
class EGateTest extends EGate
{
	function EGateTest($EGATE_DB, $LogFile, $MerchantID)
	{
		$this->EGate($EGATE_DB, $LogFile, $MerchantID);
	}

	function _OpenSocket()
	{
		// do an "echo test" on the socket
		$this->_LogMessage("Opening Socket Connection");

		$this->_SendMessage("1,test");
		$Response = $this->_GetMessage();
		if($Response != "1,echo:test")
			return $this->_SetError("socket_test", $Response);

		return 1;
	}

	function _CloseSocket()
	{
		$this->_LogMessage("Closing socket connection");

		return 1;
	}

	function _SendMessage($Message)
	{
		// allow multiple arguments for another easy way to build messages
		$Args = func_get_args();
		$Message = implode(",", $Args);

		echo "PaymentClient RECEIVED: ".$Message."\\n\n";

		$this->_DBLogMessage("Sent ... : ".str_replace("\n", "\\n", $Message));
	}

	function _GetMessage()
	{
		$Command = "read -p \"PaymentClient RESPONSE: \"";
		$Command .= "\necho \$REPLY";

		$Message = trim((string)shell_exec($Command));
		$this->_DBLogMessage("Received : ".$Message);

		if(!$Message)
			return $this->_SetError("no_rsp");

		return $Message;
	}
}


/*

TABLES REQUIRED


CREATE TABLE egate_error_codes (
  error_id int(11) NOT NULL default '0',
  error_code varchar(16) NOT NULL default '',
  error_string varchar(255) NOT NULL default '',
  error_friendly varchar(255) NOT NULL default '',
  PRIMARY KEY  (error_id)
);

INSERT INTO egate_error_codes VALUES (1,'success','','');
INSERT INTO egate_error_codes VALUES (2,'unknown_error','unknown error code \'%VALUE%\'','');
INSERT INTO egate_error_codes VALUES (5,'socket_open','Socket connection error: %VALUE%','');
INSERT INTO egate_error_codes VALUES (10,'no_request_nr','Failed to create an entry in egate_request','');
INSERT INTO egate_error_codes VALUES (101,'bad_rsp','Received an unexpected response from socket connection \'%VALUE%\'','');
INSERT INTO egate_error_codes VALUES (6,'socket_close','Failed closing socket','');
INSERT INTO egate_error_codes VALUES (4,'no_merchant','No merchant ID provided','');
INSERT INTO egate_error_codes VALUES (100,'no_rsp','Received no response from socket','');
INSERT INTO egate_error_codes VALUES (3,'bad_logfile','Unable to write to log file','Internal error: could not write to log file');
INSERT INTO egate_error_codes VALUES (7,'socket_test','Failed socket echo test: response was \'%VALUE%\'','');
INSERT INTO egate_error_codes VALUES (20,'pc_error','Payment Client Error: %VALUE%','');
INSERT INTO egate_error_codes VALUES (40,'qsi_error','QSI Response Code is an error: %VALUE%','');
INSERT INTO egate_error_codes VALUES (41,'bad_qsi','Unsuccessful QSI Response Code: %VALUE%','');
INSERT INTO egate_error_codes VALUES (30,'no_dr','Digital Receipt was empty: \'%VALUE%\'','');

CREATE TABLE egate_request (
  request_nr int(11) NOT NULL auto_increment,
  amount int(11) default NULL,
  merchant_id varchar(64) default NULL,
  request_time datetime default NULL,
  digital_order mediumtext default NULL,
  mod_time timestamp(14) NOT NULL,
  egate_reference varchar(32) default NULL,
  description varchar(34) default NULL,
  transaction_number int(11),
  error_id int(11) NOT NULL default '0',
  error_value varchar(64) NOT NULL default '',
  PRIMARY KEY  (request_nr),
  KEY idx_sr_mid (merchant_id,request_time)
);

CREATE TABLE egate_request_data (
  request_nr int(11) NOT NULL default '0',
  transmission_data text,
  PRIMARY KEY  (request_nr)
);

*/

?>
