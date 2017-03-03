<?php

require_once(PHPLIB_DIR."credit_card_verify.inc");

define("NR_MID_NUMBER", "1234");
define("NR_ACCESS_PASS", "abcdef");
define("NR_ENVIRONMENT", "TEST");

class nr_response
{
	public function __construct()
	{
		$this->Result = ""; // "approved" OR "declined" OR "failed"
		$this->Response = ""; // entire gateway response (with the credit card number stripped) - can be stored in entirety

		$this->TransRef = ""; // Transaction Reference
		$this->ResponseCode = ""; // Resonse Code (numeric?)
		$this->ResponseText = ""; // Response Text

		$this->Errors = array();
		$this->Duration = 0;
	}

	public function Request($Command, $URL)
	{
		$this->Started = $this->GetMicroTime();
		if (NR_ENVIRONMENT == "LIVE")
			$Data = file_get_contents($URL);
		else
			$Data = nr_test::Get($Command);
		$this->Parse($Data);
// echo $URL."\n";
	}

	private function CheckForValue($Line, $String, &$Parameter)
	{
		$Length = strlen($String);
		if (substr($Line, 0, $Length) == $String)
			$Parameter = substr($Line, $Length);
	}

	private function Parse($Data)
	{
		$Lines = explode("\n", $Data);
		$LineCount = sizeof($Lines);
		$this->Result = $Lines[0];
		$this->Errors = array();

		$Final = array();
		foreach ($Lines as $Line)
			if (substr($Line, 0, 8) != "card_no=")
				$Final[] = $Line;
		$this->Response = implode("\n", $Final);

		foreach ($Lines as $Line)
		{
			$this->CheckForValue($Line, "txn_ref=", $this->TransRef);
			$this->CheckForValue($Line, "response_code=", $this->ResponseCode);
			$this->CheckForValue($Line, "response_text=", $this->ResponseText);
		}

		$this->Duration = $this->GetMicroTime() - $this->Started;

//		$INQ = new DBInsert;
//$INQ->Debug = 0;
//$INQ->NoRun = 1;
//		$INQ->Save("result", $this->Result);
//		$INQ->Save("trans_ref", $this->TransRef);
//		$INQ->Save("recorded", date("Y-m-d H:i:s"));
//		$INQ->Save("duration", $this->Duration);
//		$INQ->Save("error", $this->Errors[0]);
//		$INQ->Save("response", $this->Response);
//		$INQ->Run(DB, "au_customers", "INSERT");
//		$INQ->EndQuery();
	}

	public function ErrorMode($Errors)
	{
		$this->Result = "failed";
		$this->Errors = $Errors;
	}

	private function GetMicroTime()
	{
		$Parts = explode(" ", microtime());
		$MT = $Parts[0] + $Parts[1];
		return ($MT);
	}

	public function IsApproved()
	{
		if ($this->Result == "approved")
			return(1);
		else
			return(0);
	}

	public function IsDeclined()
	{
		if ($this->Result == "declined")
			return(1);
		else
			return(0);
	}

	public function IsFailed()
	{
		if ($this->Result == "failed")
			return(1);
		else
			return(0);
	}

	public function GetResult()
	{
		return($this->Result);
	}

	public function GetTransRef()
	{
		return($this->TransRef);
	}

	public function ResponseCode()
	{
		return($this->ResponseCode);
	}

	public function ResponseText()
	{
		return($this->ResponseText);
	}

	public function GetResponse()
	{
		return($this->Response);
	}

	public function GetErrors()
	{
		return($this->Errors);
	}

	public function GetDuration()
	{
		return($this->Duration);
	}

	public function Show() // for Debug purposes only
	{
		print_r($this);
	}
}

class nr_gateway
{
	public function __construct()
	{
		// verify the NR_MID_NUMBER
		// verify the NR_ACCESS_PASS
		// verify the NR_ENVIRONMENT
	}

	private function MakeURL($Command, $Comment)
	{
		$URL  = "";
		$URL .= "https://4tknox.au.com/cgi-bin/themerchant.au.com/ecom/external2.pl?LOGIN=";
		$URL .= NR_MID_NUMBER;
		$URL .= "/";
		$URL .= NR_ACCESS_PASS;
		$URL .= "&COMMAND=";
		$URL .= $Command;
		$URL .= "&COMMENT=";
		$URL .= urlencode($Comment);
		return($URL);
	}

	private function AddAmount($URL, $Value)
	{
		$URL .= "&AMOUNT=".number_format($Value, 2, ".", "");
		return($URL);
	}

	private function AddCCNum($URL, $Value, $Type)
	{
		$this->Errors = array_merge($this->Errors, credit_card_verify::CheckDigits($Value));
		$this->Errors = array_merge($this->Errors, credit_card_verify::CheckType($Type, $Value));
		$URL .= "&CCNUM=".$Value;
		return($URL);
	}

	private function AddCCExp($URL, $Value)
	{
		if (strlen($Value) == 4)
			$Value = substr($Value, 0, 2)."/".substr($Value, 2, 2);
		$this->Errors = array_merge($this->Errors, credit_card_verify::CheckExpiry($Value));
		$URL .= "&CCEXP=".$Value;
		return($URL);
	}

	private function AddTransRef($URL, $Value)
	{
		$URL .= "&TXNREF=".$Value;
		return($URL);
	}

//
//	ONLY AVAILABLE TO ST GEORGE CUSTOMERS
//
//	private function AddAuthNum($URL, $Value)
//	{
//		$URL .= "&PREAUTHNUM=".$Value;
//		return($URL);
//	}
//

	private function Begin()
	{
		$this->Errors = array();
	}

	private function Process($Command, $URL)
	{
		$NR = new nr_response;
		if (sizeof($this->Errors))
			$NR->ErrorMode($this->Errors);
		else
			$NR->Request($Command, $URL);
		return($NR);
	}

	public function Purchase($Comment, $Amount, $CCNum, $CCType, $CCExp)
	{
		$this->Begin();
		$URL = $this->MakeURL("purchase", $Comment);
		$URL = $this->AddAmount($URL, $Amount);
		$URL = $this->AddCCNum($URL, $CCNum, $CCType); // AE | DC | MC | VC
		$URL = $this->AddCCExp($URL, $CCExp);
		return($this->Process("purchase", $URL));
	}

	public function Refund($Comment, $TransRef, $Amount)
	{
		$this->Begin();
		$URL = $this->MakeURL("refund", $Comment);
		$URL = $this->AddTransRef($URL, $TransRef);
		$URL = $this->AddAmount($URL, $Amount);
		return($this->Process("refund", $URL));
	}

	public function Status($Comment, $TransRef)
	{
		$this->Begin();
		$URL = $this->MakeURL("status", $Comment);
		$URL = $this->AddTransRef($URL, $TransRef);
		return($this->Process("status", $URL));
	}

//
//	ONLY AVAILABLE TO ST GEORGE CUSTOMERS
//
//	public function PreAuth($Comment, $Amount, $CCNum, $CCType, $CCExp)
//	{
//		$this->Begin();
//		$URL = $this->MakeURL("preauth", $Comment);
//		$URL = $this->AddAmount($URL, $Amount);
//		$URL = $this->AddCCNum($URL, $CCNum, $CCType);
//		$URL = $this->AddCCExp($URL, $CCExp);
//		return($this->Process("preauth", $URL));
//	}
//
//	public function Completion($Comment, $AuthNum, $Amount, $CCNum, $CCType, $CCExp)
//	{
//		$this->Begin();
//		$URL = $this->MakeURL("completion", $Comment);
//		$URL = $this->AddAuthNum($URL, $AuthNum);
//		$URL = $this->AddAmount($URL, $Amount);
//		$URL = $this->AddCCNum($URL, $CCNum, $CCType);
//		$URL = $this->AddCCExp($URL, $CCExp);
//		return($this->Process("completion", $URL));
//	}
//

}

class nr_test
{
	public function Get($Command)
	{
		switch($Command)
		{
			case "purchase":
				return(nr_test::Purchase());
				break;
			case "refund":
				return(nr_test::Refund());
				break;
			case "status":
				return(nr_test::Status());
				break;
		}
	}

	private function Purchase()
	{
		$TXT  = "";
		$TXT .= "declined\n";
		$TXT .= "000782000024\n";
		$TXT .= "Transaction No: 00332546\n";
		$TXT .= "------------------------\n";
		$TXT .= "TYRELL CORPORATION\n";
		$TXT .= "\n";
		$TXT .= "MERCH ID        99999999\n";
		$TXT .= "TERM ID           Y9TB99\n";
		$TXT .= "COUNTRY CODE AU\n";
		$TXT .= "31/07/00           14:32\n";
		$TXT .= "RRN         000782000024\n";
		$TXT .= "VISA\n";
		$TXT .= "411111-111\n";
		$TXT .= "CREDIT A/C         01/01\n";
		$TXT .= "AUTHORISATION NO:\n";
		$TXT .= "DECLINED   12\n";
		$TXT .= "\n";
		$TXT .= "PURCHASE           $1.00\n";
		$TXT .= "TOTAL   AUD        $1.00\n";
		$TXT .= "\n";
		$TXT .= "PLEASE RETAIN AS RECORD\n";
		$TXT .= "OF PURCHASE\n";
		$TXT .= "\n";
		$TXT .= "(SUBJECT TO CARDHOLDER'S\n";
		$TXT .= "ACCEPTANCE)\n";
		$TXT .= "------------------------\n";
		$TXT .= ".\n";
		$TXT .= "training_mode=0\n";
		$TXT .= "pld=0\n";
		$TXT .= "approved=0\n";
		$TXT .= "settlement_date=31/07/00\n";
		$TXT .= "transaction_no=332546\n";
		$TXT .= "status=declined\n";
		$TXT .= "version=V1.0\n";
		$TXT .= "operator_no=22546\n";
		$TXT .= "refund_mode=0\n";
		$TXT .= "merchant_index=24\n";
		$TXT .= "response_code=12\n";
		$TXT .= "receipt_array=ARRAY(0x8221b9c)\n";
		$TXT .= "cashout_amount=0\n";
		$TXT .= "account_type=CREDIT A/C\n";
		$TXT .= "rrn=000782000024\n";
		$TXT .= "response_text=INVALID TRANSACTION\n";
		$TXT .= "txn_ref=0007311458332546\n";
		$TXT .= "card_no=4111111111111111\n";
		$TXT .= "total_amount=100\n";
		$TXT .= "card_desc=VISA\n";
		$TXT .= "card_expiry=01/01\n";
		$TXT .= "card_type=6\n";
		$TXT .= "result=0\n";
		return($TXT);
	}

	private function Refund()
	{
	}

	private function Status()
	{
		$TXT  = "";
		$TXT .= "complete\n";
		$TXT .= "\n";
		$TXT .= ".\n";
		$TXT .= "card_number=411111xxxx1111\n";
		$TXT .= "settlement_date=31/07/00\n";
		$TXT .= "response_text=INVALID TRANSACTION\n";
		$TXT .= "amount=100\n";
		$TXT .= "status=complete\n";
		$TXT .= "txnref=0007311428202312\n";
		$TXT .= "bank_ref=000731000024\n";
		$TXT .= "card_desc=VISA\n";
		$TXT .= "response_code=12\n";
		$TXT .= "card_expiry=01/01\n";
		$TXT .= "MID=24\n";
		$TXT .= "card_type=6\n";
		$TXT .= "time=2000-07-31 14:28:20\n";
		$TXT .= "command=purchase\n";
		$TXT .= "result=0";
		return($TXT);
	}
}

?>
