<?php

class inbox
{
	function __construct()
	{
		$this->Open = "";
		$this->Types = array();
		$this->Types[0] = "text";
		$this->Types[1] = "multipart";
		$this->Types[2] = "message";
		$this->Types[3] = "application";
		$this->Types[4] = "audio";
		$this->Types[5] = "image";
		$this->Types[6] = "video";
		$this->Types[7] = "other";
	}

	function Connect($Server, $Port, $Login, $Password)
	{
		$this->Open = "{".$Server.":".$Port."/pop3/notls}INBOX";
		$this->Connection = imap_open($this->Open, $Login, $Password);
		$this->Inbox = imap_check($this->Connection);
		$this->MessageIndex = 0;
		$this->MessageCount = $this->Inbox->Nmsgs;
		$this->ClearMessage();
		return($this->MessageCount);
	}

	function ClearMessage()
	{
		$this->CurrentMessage = array();
		$this->CurrentMessage["Sender"] = "";
		$this->CurrentMessage["Subject"] = "";
		$this->CurrentMessage["Date"] = "";
		$this->CurrentMessage["MessageID"] = "";
		$this->CurrentMessage["Message"] = array();
		$this->CurrentMessage["Attachments"] = array();
		$this->CurrentMessage["Encoding"] = array();
	}

	function Next()
	{
		if ($this->Open)
		{
			$this->MessageIndex++;
			if($this->MessageIndex <= $this->MessageCount)
			{
				$this->ClearMessage();

				$Header = imap_fetch_overview($this->Connection, $this->MessageIndex);
				$this->CurrentMessage["Subject"] = $Header[0]->subject;
				$this->CurrentMessage["Sender"] = $Header[0]->from;
				$this->CurrentMessage["Date"] = date("Y-m-d H:i:s", strtotime($Header[0]->date));
				$this->CurrentMessage["MessageID"] = $Header[0]->message_id;

				$MessageBody = imap_fetchstructure($this->Connection, $this->MessageIndex);
// echo "<pre>";
// print_r($MessageBody);
// echo "<hr />";
// print_r(imap_fetchbody($this->Connection, $this->MessageIndex, 1));
// echo "</pre>";

// echo "[".number_format(memory_get_usage())."]";
				if (!is_array($MessageBody->parts))
				{
					$Attach["MIME"] = "HTML";
					$Attach["DATA"] = imap_fetchbody($this->Connection, $this->MessageIndex, 1);
					$this->CurrentMessage["Message"][] = $Attach;
				}
				else
				{
					foreach($MessageBody->parts as $i => $Part)
					{
						$Attach = array();
						$Name = "";
						$Boundary = "";
						foreach ($Part->parameters as $Attr)
						{
							if ($Attr->attribute == "NAME")
								$Name = $Attr->value;
							if ($Attr->attribute == "BOUNDARY")
								$Boundary = $Attr->value;
						}
						$Attach["DATA_ID"] = ($i + 1);
						$Attach["NAME"] = $Name;
						$Attach["MIME"] = $this->Types[$Part->type]."/".strtolower($Part->subtype);
						$this->CurrentMessage["Encoding"][$Attach["DATA_ID"]] = $Part->encoding;
						if ($Attach["NAME"])
							$this->CurrentMessage["Attachments"][] = $Attach;
						else
						{
							$DataID = $Attach["DATA_ID"];
							unset($Attach["DATA_ID"]);
	
							if ($Attach["MIME"] == "multipart/alternative")
							{
								foreach (explode($Boundary, $this->GetFileData($DataID)) as $Part)
								{
									if (trim($Part) != "--")
									{
										$Lines = array();
										$Body = 0;
										foreach (explode("\n", $Part) as $Line)
										{
											if ($Body)
												$Lines[] = $Line;
											elseif (substr($Line, 0, 26) == "Content-Transfer-Encoding:")
												$Body = 1;
											elseif (substr($Line, 0, 13) == "Content-Type:")
											{
												$Words = explode(":", str_replace(";", ":", $Line));
												$Attach["MIME"] = trim($Words[1]);
											}
										}
										array_pop($Lines);
										$Attach["DATA"] = trim(implode("\n", $Lines));
										$this->CurrentMessage["Message"][] = $Attach;
									}
								}
							}
							else
							{
								$Attach["DATA"] = $this->GetFileData($DataID);
								$this->CurrentMessage["Message"][] = $Attach;
							}
						}
					}
				}
				return(1);
			}
			else // no more messsage
			{
				return(0);
			}
		}
		else // no currently open inbox
			exit("\n\nINBOX ERROR: There are no currently opened mailboxes.\n\n");
	}

	function MessageCheck()
	{
		if (!$this->Open)
			exit("\n\nINBOX ERROR: There are no currently opened mailboxes.\n\n");
		if ($this->MessageIndex == 0 || $this->MessageIndex > $this->MessageCount)
			exit("\n\nINBOX ERROR: No message is currently unavailable.\n\n");
	}

	function GetMessageID()
	{
		$this->MessageCheck();
		return($this->CurrentMessage["MessageID"]);
	}

	function GetSender()
	{
		$this->MessageCheck();
		return($this->CurrentMessage["Sender"]);
	}

	function GetSubject()
	{
		$this->MessageCheck();
		return($this->CurrentMessage["Subject"]);
	}

	function GetDate()
	{
		$this->MessageCheck();
		return($this->CurrentMessage["Date"]);
	}

	function GetFileList()
	{
		$this->MessageCheck();
		return($this->CurrentMessage["Attachments"]);
	}

	function GetMessage()
	{
		$this->MessageCheck();
		return($this->CurrentMessage["Message"]);
	}

	function GetFileData($FileID)
	{
		switch ($this->CurrentMessage["Encoding"][$FileID])
		{
			case 0: // 7BIT
				return(imap_fetchbody($this->Connection, $this->MessageIndex, $FileID));
				break;
			case 1: // 8BIT
				break;
			case 2: // BINARY
				break;
			case 3: // BASE64
				return(base64_decode(imap_fetchbody($this->Connection, $this->MessageIndex, $FileID)));
				break;
			case 4: // QUOTED-PRINTABLE
				break;
			case 5: // OTHER
				break;
		}
	}

	function Delete() // mark a message for deletion when connection is closed
	{
		$this->MessageCheck();
		imap_delete($this->Connection, $this->MessageIndex);
	}

	function Close()
	{
		if ($this->Open != "")
		{
			if ($this->Inbox->Nmsgs);
				imap_expunge($this->Connection);
			imap_close($this->Connection);
			unset($this->Connection);
			unset($this->Inbox);
			$this->ClearMessage();
			$this->Open = $Login."@".$Server;
		}
	}
}

?>
