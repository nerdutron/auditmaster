<?php
// usage_manage.inc
//
// Handle Web Site usage via accounts
//
// Copyright MU Systems Pty Ltd 2004
//
// Written: 10/09/04	Craig Sargent

/*
create table usage_record
(
usage_id int not null AUTO_INCREMENT,
account_id int not null,
user_id int not null,
product_id int not null,
details char(64) not null,
request_time datetime not null,
ex_gst decimal (10,2) not null,
gst decimal (10,2) not null,
cost decimal (10,2),
user_ref_1 char(32),
user_ref_2 char(32),
primary key (usage_id),
index idx_u_ac (account_id,request_time),
index idx_u_pc (product_id,request_time)
);

# This table offsets the usage table. Anything in here will affect users balance
create table payments
(
payment_id int not null AUTO_INCREMENT,
account_id int not null,
payment_time datetime not null,
#payment_type is INVOICE, CREDIT_CARD, REFUND, CHEQUE, CASH
#INVOICE is a balancing entry which should match an entry in the invoice table
payment_type char(12) not null,
details char(64) not null,
amount decimal(10,2) not null,
#If it relates to a refund
usage_id int,
primary key (payment_id),
index idx_pay_acc(account_id)
);

#This table does not affect a users balance
create table invoices
(
invoice_id int not null AUTO_INCREMENT,
account_id int not null,
payment_id int not null,
amount decimal(10,2) not null,
invoice_date datetime,
invoice_status char(12),
primary key(invoice_id),
index idx_inv_acc(account_id)
);

#This table does not affect a users balance
create table invoice_payments
(
invoice_payment_id int not null AUTO_INCREMENT,
invoice_id int not null,
amount decimal(10,2) not null,
payment_date datetime not null,
primary key(invoice_payment_id),
index idx_ip_inv(invoice_id, payment_amount)
);

create table accounts
(
account_id int not null AUTO_INCREMENT,
account_name char(64) not null,
street_nr char(4),
street_name char(24),
street_type char(12),
suburb char(24),
state char(3),
postcode char(8),
address_name char(32),
account_type char(12) not null,
account_limit decimal(10,2) default '0',
schedule_id int not null,
active_status char(1) not null default 'A',
# Account group type is PARENT, JOINT_VEN, HEAD_OFF, BROKER, NORMAL
account_group_type char(12) default 'NORMAL',
account_parent int,
create_date date not null,
primary key (account_id),
index idx_ac_at(account_type)
);

create table users
(
 user_id int  not null AUTO_INCREMENT,
 account_id int not null,
 login_name char(24) not null,
 passwd char(24) not null,
 street_nr char(4),
 street_name char(24),
 street_type char(12),
 suburb char(24),
 state char(3),
 postcode char(8),
 active_status char(1) not null default 'A',
 create_date date not null,
 primary key (user_id),
 index idx_u_ln (login_name, passwd),
 index idx_u_ac (account_id)
)

create table account_access
(
 account_id int not null,
 service_code char(12) not null,
 primary key (account_id, service_code)
);

create table fee_schedules
(
 product_id int not null,
 schedule_id int not null,
 ex_gst_price decimal(10,2) not null,
 gst_exception_class char(8) default 'N',
 primary key (schedule_id, product_id)
);

create table services
(
 service_code char(8) not null,
 service_desc char(24) not null,
 open_access char(1) default 'Y',
 primary key (service_code)
);

#Not sure how this is gonna work...
create table product_groups
(
 group_id int not null auto_increment,
 product_id int not null,
 group_code char(12) not null,
 group_desc char(32) not null,
 service_code char(8) not null,
 primary key (group_id),
 index idx_pg_sc (service_code,group_id)
 index idx_pg_sc (group_code,product_id)
);

create table products
(
 product_id int not null auto_increment,
#	product_group_id int not null,
 product_desc char(32) not null,
 service_code char(8) not null,
 cost decimal(10,2) default '0',
 primary key (product_id)
);
*/

class UsageManage
{
	var $UserDetails; // Information on the user

	function UsageManage($LoginName)
	{
		$this->GST=.1;
		$this->UserDetails = new UserDetails($LoginName);
		$this->_SetPricing();
	}

	function ServiceAccess($ServiceCode)
	{
		if($this->UserDetails->Error)
			return 0;

		if(!count($this->Services))
			$this->_GetServiceAccess();

		if($this->Services[$ServiceCode]["Access"] == "Y")
			return 1;

		return 0;
	}

	function GetPricing($ProductID)
	{
		if($this->UserDetails->Error)
			return array();

		if(!is_array($this->Products))
			$this->_SetPricing();

		$GSTStatus = $this->Products[$ProductID]["GSTStatus"];
		$Price = $this->Products[$ProductID]["Price"];
		$Cost = $this->Products[$ProductID]["Cost"];

		if($GSTStatus == "EXEMPT")
			$GST = 0.00;
		else
			$GST = $this->GST * $Price;

		$Data["Price"] = $Price;
		$Data["GST"] = $GST;
		$Data["Cost"] = $Cost;

		return $Data;
	}

	// Note: Need to add in concept of referer/parent/head office/broker
	function RecordUsage($ProductID, $Details, $UserRef1, $UserRef2)
	{
		if(!$this->UserDetails->AccountID)
		{
			$this->Error=1;
			$this->ErrorString = "No Account ID";
			return 0;
		}

		if(!$ProductID)
		{
			$this->Error=1;
			$this->ErrorString = "No Product ID";
			return 0;
		}

		if(!is_array($this->Products[$ProductID]))
		{
			$this->Error=1;
			$this->ErrorString = "Invalid Product ID: $ProductID";
			return 0;
		}
/*
		if($this->Products[$ProductID]["ProductStatus"] != "A")
		{
			$this->Error=1;
			$this->ErrorString = "Product ID: $ProductID not active";
			return 0;
		}
*/

		if(!$this->ServiceAccess($this->Products[$ProductID]["ServiceCode"]))
		{
			$this->Error=1;
			$this->ErrorString = "No Access to Service";
			return 0;
		}

		$Pricing = $this->GetPricing($ProductID);

		$Price = $Pricing["Price"];
		$GST = $Pricing["GST"];
		$CostToUs = $Pricing["Cost"];

		$INQ = new DBInsert;

		$INQ->Add("AUTO","usage_id","0");
		$INQ->Add("INT","product_id",$ProductID);
		$INQ->Add("INT","account_id",$this->UserDetails->AccountID);
		$INQ->Add("INT","user_id",$this->UserDetails->UserID);
		$INQ->Add("VARCHAR","details",$Details);
		$INQ->Add("VARCHAR","user_ref_1",$UserRef1);
		$INQ->Add("VARCHAR","user_ref_2",$UserRef2);
		$INQ->Add("FUNCTION","request_time","now()");
		$INQ->Add("DECIMAL","ex_gst",$Price);
		$INQ->Add("DECIMAL","gst",$GST);
		$INQ->Add("DECIMAL","cost",$CostToUs);


		$INQ->Run(DB,"usage_record","INSERT");

		$this->UsageID = $INQ->GetInsertId();

		if(is_array($ExtraData))
			$this->StoreExtraData($this->UsageID, $ExtraData);
		return $this->UsageID;
	}

	function StoreExtraData($UsageID, $ExtraData)
	{
		if(!is_array($ExtraData) || !$UsageID)
			return;

		foreach($ExtraData as $Key => $Value)
		{
			$INQ = new DBInsert;
			$INQ->Add("INT","usage_id",$UsageID);
			$INQ->Add("VARCHAR","field_name",$Key);
			$INQ->Add("VARCHAR","value",$Value);
			$INQ->Run(DB,"usage_extra_data","INSERT");
			$INQ->EndQuery();
		}
	}

	function CheckFunds($ProductID)
	{
		if($this->UserDetails->Error)
			return 0;

		$Balance = $this->GetBalance();

		$GSTStatus = $this->Products[$ProductID]["GSTStatus"];
		$Price = $this->Products[$ProductID]["Price"];

		if($GSTStatus == "EXEMPT")
			$GST = 0.00;
		else
			$GST = $this->GST * $Price;


		if($Balance >= ($Price + $GST))
			return 1;

		return 0;
	}

	function GetBalance()
	{
		if($this->UserDetails->Error)
			return 0;

		$Qry = "SELECT sum(ex_gst + gst) as spent FROM usage_record ";
		$Qry .= "WHERE account_id = ".$this->UserDetails->AccountID;

		$Q = new DBQuery;

		$Q->RunOne(DB, $Qry);

		$Spent = $Q->GetField("spent");

		$Q->EndQuery();

		$Qry = "SELECT sum(amount) as paid FROM payments ";
		$Qry .= "WHERE account_id = ".$this->UserDetails->AccountID; 

		$Q = new DBQuery;

		$Q->RunOne(DB, $Qry);

		$Payments = $Q->GetField("paid");

		$Q->EndQuery();

		$Balance = $Payments - $Spent;

		$Type = $this->UserDetails->AccountType;

		// If an invoice account, add the negative balance to the limit to see how much funds are left
		if($Type == "INVOICE")
			$Balance = $this->UserDetails->AccountLimit + $Balance;

		return $Balance;
	}

	function SetUsageInvoice($UsageID, $InvoiceID)
	{
		if(!$UsageID || !$InvoiceID)
			return;

		$INQ = new DBInsert;

		$INQ->AddWhere("INT","usage_id",$UsageID);
		$INQ->Add("INT","invoice_id",$InvoiceID);

		$INQ->Run(DB,"usage_record","UPDATE");

		$INQ->EndQuery();
	}

	function GetInvoiceUsageInfo($DateTime)
	{
		$Value = $this->GetInvoiceValue($DateTime);

		$Info = array();
		$UsageIDArr = array();

		$Qry = "SELECT usage_id FROM usage_record ";
		$Qry .= "WHERE account_id = ".$this->UserDetails->AccountID." ";
		$Qry .= "AND request_time <= '$DateTime' ";
		$Qry .= "AND invoice_id = 0";

		$Q = new DBQuery;

		$Q->Run(DB, $Qry);
		
		while($Q->Next())
		{
			$UsageIDArr[] = $Q->GetField("usage_id");
		}

		$Info["Value"] = $Value;
		$Info["UsageIDList"] = $UsageIDArr;

		$Q->EndQuery();
		return $Info;
	}

	function GetInvoiceValue($DateTime)
	{
		if($this->UserDetails->Error)
			return;

		$Type = $this->UserDetails->AccountType;
		if($Type != "INVOICE")
			return 0;

		$Qry = "SELECT sum(ex_gst + gst) as spent FROM usage_record ";
		$Qry .= "WHERE account_id = ".$this->UserDetails->AccountID." ";
		$Qry .= "AND request_time <= '$DateTime'";

		$Q = new DBQuery;

		$Q->RunOne(DB, $Qry);

		$Spent = $Q->GetField("spent");

		$Q->EndQuery();

		$Qry = "SELECT sum(amount) as paid FROM payments ";
		$Qry .= "WHERE account_id = ".$this->UserDetails->AccountID; 

// Want to look at ALL payments, not just before a date...so exclude the date from this query
//		$Qry .= "AND payment_time <= '$DateTime'";

		$Q = new DBQuery;

		$Q->RunOne(DB, $Qry);

		$Payments = $Q->GetField("paid");

		$Q->EndQuery();

		$InvoiceValue= $Payments - $Spent;

		return $InvoiceValue;
	}

	function _GetServiceAccess()
	{
		if($this->UserDetails->Error)
			return;

		$Qry = "SELECT * FROM services";

		$Q = new DBQuery;
		$Q->Run(DB, $Qry);

		while($Q->Next())
		{
			$this->Services[$Q->GetField("service_code")]["Desc"] = $Q->GetField("service_desc");
			$this->Services[$Q->GetField("service_code")]["Access"] = $Q->GetField("open_access");
		}

		$Q->EndQuery();

		$Qry = "SELECT * FROM account_access ";
		$Qry .= "WHERE account_id=".$this->UserDetails->AccountID;

		$Q = new DBQuery;
		$Q->Run(DB, $Qry);

		while($Q->Next())
		{
			$this->Services[$Q->GetField("service_code")]["Access"] = "Y";
		}

		$Q->EndQuery();
	}

	function _SetPricing()
	{
		$this->Products = array();
		if($this->UserDetails->Error)
			return;

		$Qry = "SELECT * FROM fee_schedules ";
		$Qry .= "WHERE schedule_id = ".$this->UserDetails->ScheduleID;

		$Q = new DBQuery;

		$Q->Run(DB, $Qry);

		while($Q->Next())
		{
			$this->Products[$Q->GetField("product_id")]["Price"] = $Q->GetField("ex_gst_price");
			$this->Products[$Q->GetField("product_id")]["GSTStatus"] = $Q->GetField("gst_exemption_class");
		}
		$Q->EndQuery();

		$Qry = "SELECT * FROM products ";

		$Q = new DBQuery;

		$Q->Run(DB, $Qry);

		while($Q->Next())
		{
			$this->Products[$Q->GetField("product_id")]["ProductStatus"] = $Q->GetField("product_status");
			$this->Products[$Q->GetField("product_id")]["Cost"] = $Q->GetField("cost");
			$this->Products[$Q->GetField("product_id")]["ServiceCode"] = $Q->GetField("service_code");
		}
		$Q->EndQuery();
	}
}

class UserDetails
{
	var $AccountID;
	var $AccountName;
	var $UserID;
	var $LoginName;
	var $AccountStatus;
	var $UserStatus;

	function UserDetails($LoginName)
	{
		$Qry = "SELECT * FROM users WHERE login_name='$LoginName'";
		$Q = new DBQuery;

		$Q->Run(DB, $Qry);
		if(!$Q->Next())
		{
			$this->Error=1;
			$this->ErrorString = "Invalid Login Name: $LoginName";
			return 0;
		}

		$this->UserStatus = $Q->GetField("active_status");
		if($this->UserStatus != "A")
		{
			$this->Error=1;
			$this->ErrorString = "User not active";
			return 0;
		}
		$this->UserID = $Q->GetField("user_id");
		$this->AccountID = $Q->GetField("account_id");
		$this->LoginName= $Q->GetField("login_name");
		$this->UserName= $Q->GetField("user_name");
		$this->UserEmail = $Q->GetField("user_email");
		$this->UserAddress["StreetNr"] = $Q->GetField("street_nr");
		$this->UserAddress["StreetName"] = $Q->GetField("street_name");
		$this->UserAddress["StreetType"] = $Q->GetField("street_type");
		$this->UserAddress["Suburb"] = $Q->GetField("suburb");
		$this->UserAddress["State"] = $Q->GetField("State");
		$this->UserAddress["Postcode"] = $Q->GetField("postcode");

		$this->SetAccountDetails($this->AccountID);
	}

	function SetAccountDetails($AccountID)
	{
		$Qry = "SELECT * FROM accounts WHERE account_id='$AccountID'";

		$Q = new DBQuery;

		$Q->Run(DB, $Qry);
		if(!$Q->Next())
		{
			$this->Error=1;
			$this->ErrorString = "Invalid Account ID";
			return 0;
		}

		$this->AccountStatus = $Q->GetField("active_status");
		if($this->AccountStatus != "A")
		{
			$this->Error=1;
			$this->ErrorString = "Account not active";
			return 0;
		}

		$this->ScheduleID = $Q->GetField("schedule_id");
		$this->AccountName = $Q->GetField("account_name");
		$this->AccountType = $Q->GetField("account_type");
		$this->AccountLimit = $Q->GetField("account_limit");
		$this->AccountAddress["StreetNr"] = $Q->GetField("street_nr");
		$this->AccountAddress["StreetName"] = $Q->GetField("street_name");
		$this->AccountAddress["StreetType"] = $Q->GetField("street_type");
		$this->AccountAddress["Suburb"] = $Q->GetField("suburb");
		$this->AccountAddress["State"] = $Q->GetField("State");
		$this->AccountAddress["Postcode"] = $Q->GetField("postcode");
		$this->AccountAddress["AddressName"] = $Q->GetField("address_name");
	}
}
?>
