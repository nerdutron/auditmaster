<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

//
// mailq.inc
//
// Outgoing Email Queuing software
//
// Author: Murray Shields
// 
// Copyright 2003 MU Systems Pty Ltd
//
//

require_once(PHPLIB_DIR."zip.inc");
require_once(PHPLIB_DIR."mail.inc");

if (!defined("MAIL_Q_DB"))
	define("MAIL_Q_DB", "mysql://".$MY_USER.":".$MY_PASSWORD."@localhost/WEB");

class MailQ
{
	function __construct()
	{
		$this->Reset();
	}

	function AddEmail($Subject, $Body, $Sender, $Project = "", $Expires = "")
	{
		$this->Reset();
		$this->Details["Project"] = $Project;
		if ($Expires)
			$this->Details["Expires"] = date("Y-m-d", strtotime($Expires));
		else
			$this->Details["Expires"] = date("Y-m-d", strtotime("1 month"));
		$this->Details["Subject"] = $Subject;
		$this->Details["Body"] = $Body;
		$this->Details["Sender"] = $Sender;
		$this->Details["Delivery"] = "0000-00-00 00:00:00";
	}

	function AddRecipients($Recipients, $Type = "")
	{
		$Type = strtoupper($Type);
		if (!in_array($Type, $this->Types))
			$Type = "";

		if (!is_array($Recipients))
		{
			$Addresses[0] = $Recipients;
			if ($Type == "")
				$Type = "TO";
		}
		else
		{
			$Addresses = $Recipients;
			if ($Type == "")
				$Type = "BCC";
		}

		$Size = sizeof($Addresses);
		for ($i=0;$i<$Size;$i++)
		{
			if ($Addresses[$i] != "")
			{
				if (defined("MAIL_Q_RECIPIENT"))
					$this->IntendedRecipients .= $Type.": ".$Addresses[$i]."\n";
				else
					$this->Recipients[$Type][] = $Addresses[$i];
			}
		}
	}

	function AddDocument($DocumentData, $MimeType, $FileName, $Compression = 0)
	{
		$i = sizeof($this->Documents);

		$this->Documents[$i]["Data"] = $DocumentData;
		$this->Documents[$i]["Type"] = $MimeType;
		$this->Documents[$i]["FileName"] = $FileName;
		$this->Documents[$i]["Compression"] = $Compression;
	}

	function DeliveryAfter($TimeStamp)
	{
		$this->Details["Delivery"] = $TimeStamp;
	}

	function DelayDeliveryBy($Delay)
	{
		$this->DeliveryAfter(date("Y-m-d H:i:s", strtotime($Delay)));
	}

	function Reset()
	{
		$this->Details = array();
		$this->Recipients = array();
		$this->Documents = array();
		$this->Types = array ("TO", "CC", "BCC");
		$this->IntendedRecipients = "\n=========================\nIntended Email Recipients\n-------------------------\n";
	}

	function Abort($Error)
	{
		$Query = "delete from mail_q_recipients where request_id = ".$this->RequestID;
		DB_DoQuery(MAIL_Q_DB, $Query);
		$Query = "delete from mail_q_documents  where request_id = ".$this->RequestID;
		DB_DoQuery(MAIL_Q_DB, $Query);
		$Query = "delete from mail_q_requests   where request_id = ".$this->RequestID;
		DB_DoQuery(MAIL_Q_DB, $Query);

		if (defined("MAIL_Q_ERRORS"))
		{
			$Message  = $Error;
			$Message .= "\n\n";
			$Message .= "========================================";
			$Message .= "\n\n";
			$Message .= "Script Details:";
			$Message .= "\n\n";
			if ($_SERVER["REQUEST_URI"])
				$Message .= $_SERVER["REQUEST_URI"];
			else
				$Message .= implode(" ", $_SERVER["argv"]);
			$Message .= "\n\n";
			$Message .= "========================================";
			$Message .= "\n\n";
			$Message .= "Email Details:";
			$Message .= "\n\n";
			$Message .=  GetArrayContents($this->Details);
			$Message .= "\n\n";
			$Message .= "Intended Recipients:";
			$Message .= "\n\n";
			$Message .=  GetArrayContents($this->Recipients);
			$Message .= "\n\n";
			$Message .= "Attached Documents:";
			$Message .= "\n\n";
			$Message .=  GetArrayContents($this->Documents);
			$Message .= "\n\n";
			mail(MAIL_Q_ERRORS, "MailQ Send Error", $Message);
		}
		return($Error);
	}

	function Send()
	{
		$this->RequestID = 0;

		if (defined("MAIL_Q_TEST"))
			if (MAIL_Q_TEST)
			{
				$this->Reset();
				return("");
			}

		if (defined("MAIL_Q_RECIPIENT"))
		{
			$this->Recipients["TO"][] = MAIL_Q_RECIPIENT;
			$this->Details["Body"] .= $this->IntendedRecipients;
		}

		if (trim($this->Details["Subject"]) == "")
			return ($this->Abort("This email does not have a subject line."));

		$INQ = new DBInsert;
		$INQ->Save("project", $this->Details["Project"]);
		$INQ->Save("keep_until", $this->Details["Expires"]);
		$INQ->Save("delivery_after", $this->Details["Delivery"]);
		$INQ->Save("sender_email", $this->Details["Sender"]);
		$INQ->Save("email_subject", $this->Details["Subject"]);
		$INQ->Save("email_body", $this->Details["Body"]);
		$INQ->Save("delivery_status", "loading");
		$INQ->Run(MAIL_Q_DB, "mail_q_requests", "INSERT");
		$this->RequestID = $INQ->GetInsertId();
		$INQ->EndQuery();

		$c = 0;
		$Count1 = sizeof($this->Types);
		for ($t=0;$t<$Count1;$t++)
		{
			if (empty($this->Recipients[$this->Types[$t]]))
					continue;

			$Count2 = sizeof($this->Recipients[$this->Types[$t]]);
			for ($i=0;$i<$Count2;$i++)
			{
				$INQ = new DBInsert;
				$INQ->Save("request_id", $this->RequestID);
				$INQ->Save("email_id", $c);
				$INQ->Save("recipient_email", $this->Recipients[$this->Types[$t]][$i]);
				$INQ->Save("address_type", $this->Types[$t]);
				$INQ->Run(MAIL_Q_DB, "mail_q_recipients", "INSERT");
				$INQ->EndQuery();
				$c++;
			}
		}

		if ($c == 0)
			return ($this->Abort("This email does not have any recipients."));

		$DocCount = sizeof($this->Documents);
		for ($i=0;$i<$DocCount;$i++)
		{
			$INQ = new DBInsert;
			$INQ->Save("request_id", $this->RequestID);
			$INQ->Save("doc_data", base64_encode(gzcompress($this->Documents[$i]["Data"])));
			$INQ->Save("mime_type", $this->Documents[$i]["Type"]);
			$INQ->Save("attachment_filename", $this->Documents[$i]["FileName"]);
			if ($this->Documents[$i]["Compression"])
				$INQ->Save("compression", "Y");
			else
				$INQ->Save("compression", "N");
			$INQ->Run(MAIL_Q_DB, "mail_q_documents", "INSERT");
			$INQ->EndQuery();

			if (strlen($this->Documents[$i]["FileName"]) > 210)
				return ($this->Abort("A document filename exceeds the maximum allowed length (210 characters)."));
		}

		$INQ = new DBInsert;
		$INQ->SaveWhere("request_id", $this->RequestID);
		$INQ->Save("delivery_status", "waiting");
		$INQ->Run(MAIL_Q_DB, "mail_q_requests", "UPDATE");
		$INQ->EndQuery();

		$this->Reset();
	}

	function Process()
	{
		$Query = "SELECT value FROM mail_q_config WHERE config = 'UPLOAD_RATE_BYTES' ";
		$UploadRate = DBGet(MAIL_Q_DB, $Query);
		if ($UploadRate)
			$UploadRate = ($UploadRate / 10);

		$Config = array();
		$Config["restart_maximum"] = 5;
		$Config["load_sensitivity"] = 3;
		$Config["every"] = 5;
		$Config["delay"] = 1;
		$Config["timeout"] = 1800;
		$Config["description"] = "MailQ on ".ENV;
		$Config["email"] = "support@shields.id.au";
//		$Config["pager"] = "0418736298@mu.com.au";

		$SC = new Script(MAIL_Q_DB, "MailQ_".ENV, $Config);
		$SC->Open();

		$Incomplete = 1;
		do
		{
			$Query  = "";
			$Query .= "select * from mail_q_requests ";
			$Query .= "where delivery_after < '".date("Y-m-d H:i:s")."' ";
			$Query .= "and delivery_status = 'waiting' ";
			$Query .= "limit 1";
			$Q = new DBQuery;
			$Q->RunOne(MAIL_Q_DB, $Query);
			if ($Q->NumRows())
				$AttachmentsSize = MailQ::DeliverFromQ($Q->GetField("request_id"));
			else
			{
				$AttachmentsSize = 0;
				$Incomplete = 0;
			}
			$Q->EndQuery();

			if ($UploadRate * $AttachmentsSize)
			{
				$Seconds = ceil($AttachmentsSize / $UploadRate);
				$Seconds += 2;
				sleep($Seconds);
			}

			$SC->Increment();

		} while($Incomplete);

		$Query  = "";
		$Query .= "select * from mail_q_requests ";
		$Query .= "where keep_until < '".date("Y-m-d H:i:s")."' ";
		$Query .= "and delivery_status = 'delivered' ";
		$Q = new DBQuery;
		$Q->Run(MAIL_Q_DB, $Query);
		while ($Q->Next())
			MailQ::Delete(array($Q->GetField("request_id")));
		$Q->EndQuery();

		$SC->Close();
	}

	function DeliverFromQ($RequestID)
	{
		$INQ = new DBInsert;

		$Query = "select * from mail_q_requests where request_id = ".$RequestID;
		$R = new DBQuery;
		$R->RunOne(MAIL_Q_DB, $Query);
		if ($R->GetField("delivery_status") == "waiting")
		{
			$AttachmentsSize = 0;

			$INQ->SaveWhere("request_id", $RequestID);
			$INQ->Save("delivery_status", "sending");
			$INQ->Run(MAIL_Q_DB, "mail_q_requests", "UPDATE");
			$INQ->EndQuery();

			$M = new Mail;
			$M->all_replies($R->GetField("sender_email"));
			$M->subject = $R->GetField("email_subject");
			$M->body = $R->GetField("email_body");

			$TO = "";
			$CC = "";
			$BCC = "";
			$Query = "select * from mail_q_recipients where request_id = ".$RequestID;
			$Q = new DBQuery;
			$Q->Run(MAIL_Q_DB, $Query);
			while ($Q->Next())
			{
				switch($Q->GetField("address_type"))
				{
					case "TO":
						$TO .= $Q->GetField("recipient_email")." ";
						break;
					case "CC":
						$CC .= $Q->GetField("recipient_email")." ";
						break;
					case "BCC":
						$BCC .= $Q->GetField("recipient_email")." ";
						break;
				}
			}

			if ($TO)
				$M->to = $TO;
			if (!empty($CC))
				$M->headers .= "Cc: ".$CC."\n";
			if (!empty($BCC))
				$M->headers .= "Bcc: ".$BCC."\n";

			$Z = new ZIP;
			$Query = "select * from mail_q_documents where request_id = ".$RequestID;
			$Q = new DBQuery;
			$Q->Run(MAIL_Q_DB, $Query);
			while ($Q->Next())
			{
				$Data = gzuncompress(base64_decode($Q->GetField("doc_data")));
				$MIME = $Q->GetField("mime_type");
				if ($Q->GetField("compression") == "Y")
				{
					$Z->AddFile($Data, $Q->GetField("attachment_filename"));
					$Zip = 1;
				}
				else
				{
					$AttachmentsSize = $AttachmentsSize + strlen($Data);
					$M->add_attachment($Data, $Q->GetField("attachment_filename"), $MIME);
				}
			}
			if ($Zip)
			{
				$Data = $Z->Get();
				$AttachmentsSize = $AttachmentsSize + strlen($Data);
				$M->add_attachment($Data, "attachments.zip", "application/zip");
			}
			$Z->Reset();

			$M->send();

			$INQ->SaveWhere("request_id", $RequestID);
			$INQ->Save("delivery_status", "delivered");
			$INQ->Save("delivered_at", date("Y-m-d H:i:s"));
			$INQ->Save("delivered_size", $AttachmentsSize);
			$INQ->Run(MAIL_Q_DB, "mail_q_requests", "UPDATE");
			$INQ->EndQuery();
		}

		return ($AttachmentsSize);
	}

	function Status()
	{
		echo "\n\n";

		$Query  = "";
		$Query .= "select count(*) as count ";
		$Query .= "from mail_q_requests where delivery_status <> 'delivered' ";
		$Query .= "and delivery_after [MATCH1] '".date("Y-m-d H:i:s")."' and delivery_status = '[MATCH2]'";

		$Count = DBGet(MAIL_Q_DB, str_replace(array("[MATCH1]", "[MATCH2]"), array("<", "waiting"), $Query));
		echo "\nWaiting: ".number_format($Count);
		$Count = DBGet(MAIL_Q_DB, str_replace(array("[MATCH1]", "[MATCH2]"), array("<", "loading"), $Query));
		echo "\nLoading: ".number_format($Count);
		$Count = DBGet(MAIL_Q_DB, str_replace(array("[MATCH1]", "[MATCH2]"), array("<", "sending"), $Query));
		echo "\nSending: ".number_format($Count);
		$Count = DBGet(MAIL_Q_DB, str_replace(array("[MATCH1]", "[MATCH2]"), array(">", "waiting"), $Query));
		echo "\nEmails: ".number_format($Count);

		$Config = array();
		$Config["restart_maximum"] = 5;
		$Config["timeout"] = 1800;
		$SC = new Script(MAIL_Q_DB, "MailQ", $Config);
		$SC->Open();
		$SC->Close();

		echo "\n\n";
	}

	function Listing()
	{
		$Query = "select * from mail_q_requests where delivery_status != 'delivered' order by delivery_status, delivery_after, request_id";
		$Q = new DBQuery;
		$Q->Run(MAIL_Q_DB, $Query);

		$Emails = array();
		while($Q->Next())
		{
			$Data = array();
			$Data["request"] = $Q->GetField("request_id");
			$Data["status"] = $Q->GetField("delivery_status");
			$Data["delay"] = $Q->GetField("delivery_after");
			$Data["sender"] = $Q->GetField("sender_email");
			$Data["subject"] = $Q->GetField("email_subject");

			$Query = "select count(*) from mail_q_recipients where request_id = ".$Data["request"];
			$Count = DBGet(MAIL_Q_DB, $Query);
			if ($Count > 1)
				$Data["recipients"] = $Count." recipients";
			else
			{
				$Query = "select recipient_email from mail_q_recipients where request_id = ".$Data["request"];
				$Address = DBGet(MAIL_Q_DB, $Query);
				$Data["recipients"] = $Address;
			}

			$Query = "select doc_data from mail_q_documents where request_id = ".$Data["request"];
			$Count = 0;
			$Size = 0;
			$D = new DBQuery;
			$D->Run(MAIL_Q_DB, $Query);
			while ($D->Next())
			{
				$Count++;
				$Size = $Size + strlen(gzuncompress(base64_decode($D->GetField("doc_data"))));
			}
			$Documents = array();
			$Data["documents"] = number_format($Size)." bytes in ".$Count." files.";
			$Emails[] = $Data;
		}
		$Q->EndQuery();

		foreach ($Emails as $Email)
		{
			echo "\n\n";
			echo "ID: ".$Email["request"];
			echo "     ";
			echo "State: ".$Email["status"];
			echo "     ";
			echo "After: ".$Email["delay"];
			echo "\n";
			echo "From: ".$Email["sender"];
			echo "     ";
			echo "To: ".$Email["recipients"];
			echo "\n";
			echo "Subject: ".$Email["subject"];
			echo "     ";
			echo "Attachments: ".$Email["documents"];
		}
		echo "\n\n";
	}

	function Delete($Requests)
	{
		foreach ($Requests as $Request)
		{
			$Query = "DELETE FROM mail_q_recipients WHERE request_id = ".$Request;
			DB_DoQuery(MAIL_Q_DB, $Query);
			$Query = "DELETE FROM mail_q_documents  WHERE request_id = ".$Request;
			DB_DoQuery(MAIL_Q_DB, $Query);
			$Query = "DELETE FROM mail_q_requests   WHERE request_id = ".$Request;
			DB_DoQuery(MAIL_Q_DB, $Query);
		}
	}

	function ResetQ()
	{
        DB_DoQuery(MAIL_Q_DB, "delete from script_log");
	}
}

?>
