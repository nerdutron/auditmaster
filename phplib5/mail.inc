<?php
/*
 *  Class mime_mail
 *  Original implementation by Sascha Schumann <sascha@schumann.cx>
 *  Modified by Tobias Ratschiller <tobias@dnet.it>:
 *      - General code clean-up
 *      - separate body- and from-property
 *      - killed some mostly un-necessary stuff
 */
// Modification History
// --------------------
// Murray Shields	28/4/2004
//
// Rod Cole	31/10/2003
// Added in Content-Disposition section to functoin build_message
//
// Craig Sargent	13/6/2002
// Added in support for passing parameters
 
class Mail 
{
	var $parts;
	var $to;
	var $from;
	var $headers;
	var $parameters;
	var $subject;
	var $body;

//
// void mime_mail()
// class constructor
//
	function __construct()
	{
		$this->parts = array();
		$this->to = "";
		$this->from = "";
		$this->subject = "";
		$this->content = "";
		$this->body = "";
		$this->mime = "";
		$this->headers = "";
		$this->parameters = "";
	}

//
// void add_attachment(string message, [string name], [string ctype])
// Add an attachment to the mail object
//
	function add_attachment($message, $name = "", $ctype = "application/octet-stream")
	{
		$this->parts[] = array (
			"ctype" => $ctype,
			"message" => $message,
			"name" => $name
			);
	}

//
// void build_message(array part=
// Build message parts of an multipart mail
//
	function build_message($Part)
	{
		$message = chunk_split(base64_encode($Part["message"]), 72);

		$ReturnStr = "Content-Type: ".$Part["ctype"];
		if($Part["name"])
			$ReturnStr .= "; name=\"".$Part["name"]."\"";
		$ReturnStr .= "\nContent-Transfer-Encoding: base64";

		if($Part["name"])
			$ReturnStr .= "\nContent-Disposition: inline; filename=\"".$Part["name"]."\"";

		$ReturnStr .= "\n\n$message\n";
		return $ReturnStr;
	}

//
// void build_multipart()
// Build a multipart mail
//
	function build_multipart() 
	{
		$boundary = "b".md5(uniqid(time()));
		$this->mime .= "Content-Type: multipart/mixed; boundary=\"$boundary\"\n\n";
		$this->content .= "--$boundary";

		for($i = sizeof($this->parts)-1; $i >= 0; $i--) 
			$this->content .= "\n".$this->build_message($this->parts[$i])."--$boundary";
		$this->content .= "--\n";
	}

	function extract_header($Type)
	{
		$Found = "";
		if (strstr($this->mime, $Type))
		{
			$New = array();
			foreach (explode("\n", $this->mime) as $Line)
			{
				if (strstr($Line, $Type))
					$Found = $Line;
				else
					$New[] = $Line;
			}
		}
		$this->mime = implode("\n", $New);
		return($Found);
	}

//
// void send()
// Send the mail (last class-function to be called)
//

	function send() 
	{
		if (defined("MAIL_TO_DIR"))
		{
			if (is_dir(MAIL_TO_DIR))
			{
				$FileID = array();
				$D = opendir(MAIL_TO_DIR);
				while (($File = readdir($D)) !== false)
				{
					if (substr($File, 0, 5) == "MAIL_")
					{
						$Parts = explode("_", $File);
						$FileID[] = $Parts[1] * 1;
					}
				}
				closedir($D);
				sort($FileID);
				if (sizeof($FileID))
					$ID = str_pad((array_pop($FileID) + 1), 6, "0", STR_PAD_LEFT);
				else
					$ID = "000000";

				$Path = MAIL_TO_DIR."MAIL_".$ID."_";
				file_put_contents($Path."message_".str_replace(array("/", " "), array("-", "_"), $this->subject).".htm", nl2br($this->body));
				foreach ($this->parts as $part)
					file_put_contents($Path.$part["name"], $part["message"]);
				return;
			}
		}

		if (!empty($this->from))
			$this->mime .= "From: ".$this->from."\n";
		if (!empty($this->headers))
			$this->mime .= $this->headers;

		if (defined("MAIL_TO_FORCE"))
		{
			$CC  = $this->extract_header("Cc:");
			$BCC = $this->extract_header("Bcc:");
			$this->body = trim("Intended Recipients: ".$this->to." ".$CC." ".$BCC)."\n\n".$this->body;

			$ToAddresses = array();
			foreach (explode(" ", trim($this->to)) as $E)
				if (strstr($E, "@"))
					$ToAddresses[] = $E;
			$this->to = "";
			foreach (explode(" ", trim($CC)) as $E)
				if (strstr($E, "@"))
					$ToAddresses[] = $E;
			foreach (explode(" ", trim($BCC)) as $E)
				if (strstr($E, "@"))
					$ToAddresses[] = $E;
			if (defined("MAIL_TO_DOMAIN") && MAIL_TO_DOMAIN)
			{
				$Domains = explode("|", MAIL_TO_DOMAIN);
				foreach ($ToAddresses as $Address)
				{
					foreach ($Domains as $Domain)
					if (stristr($Address, $Domain))
					{
						if ($this->to)
							$this->to .= " ";
						$this->to .= $Address;
					}
				}
			}

			if ($this->to == "")
				$this->to = MAIL_TO_FORCE;
		}

		$FileCount = sizeof($this->parts); 
		if (!empty($this->body))
			$this->add_attachment(nl2br($this->body), "", "text/html; charset=\"UTF-8\"");
		//	$this->add_attachment(nl2br($this->body), "", "text/html; charset=\"ISO-8859-1\"");

		$this->mime .= "MIME-Version: 1.0\n";
		$this->build_multipart();

		if (!strstr($this->from, "creditworks.com.au"))
		{
			$Found = "";
			foreach(debug_backtrace() as $Trace)
			{
				if (!$Found)
				{
					if (!strstr($Trace["file"], "phplib"))
					{
						$Found .= date("H:i:s");
						$Found .= " File: ";
						$Found .= $Trace["file"];
						$Found .= " Line: ";
						$Found .= $Trace["line"];
						$Found .= " Function: ";
						$Found .= $Trace["function"];
						$Found .= "()";
						$Found .= " TO: ";
						$Found .= $this->from;
						$Found .= "\n";
					}
				}
			}
			if ($Found)
			{
				$File = "/tmp/mail_from_".get_current_user()."_".date("Y-m-d").".log";
				$fp = fopen($File, "a");
				fwrite($fp, $Found);
				fclose($fp);
			}
		}

		$Success = mail($this->to, $this->subject, $this->content, $this->mime, $this->parameters);
		if (!$Success)
		{
			if (defined("MAIL_TO_FAIL"))
			{
				$Message = error_get_last();
				$Error  = "";
				if (strstr($Message["file"], "mail"))
				{
					$Error .= "ERROR:\n\n";
					$Error .= $Message["message"];
				}
				$Error .= "\n\n";
				$Error .= "TO:\n\n";
				$Error .= "[".$this->to."]";
				$Error .= "\n\nSUBJECT:\n\n";
				$Error .= "[".$this->subject."]";
				$Error .= "\n\nATTACHED:\n\n";
				$Error .= sizeof($FileCount);
				$Error .= "\n\nMIME:\n\n";
				$Error .= "[".$this->mime."]";
				$Error .= "\n\nPARAMETERS:\n\n";
				$Error .= "[".$this->parameters."]";
				$Error .= "\n\nBODY:\n\n";
				$Error .= "[".$this->body."]";
				mail(MAIL_TO_FAIL, "mail() Failure: ".ENV." on ".$_SERVER["HOSTNAME"], $Error);
			}
		}
	}

	function all_replies($email)
	{
//		$this->parameters = "-f".$this->address_base($email);
		$this->parameters = "-f".$email;
//		$email = $this->address_full($email);
		$this->from = $email;
		$this->headers .= "Reply-To: ".$email."\n";
	}

	function address_base($email)
	{
		$Parts = explode(" ", trim(str_replace(array("'", "\"", "<", ">"), "", $email)));
		$Address = "";
		while($Address == "" && sizeof($Parts))
			$Address = array_pop($Parts);
		return($Address);
	}

	function address_full($email)
	{
		$Parts = explode(" ", trim(str_replace(array("'", "\"", "<", ">"), "", $email)));
		$Address = "";
		while($Address == "" && sizeof($Parts))
			$Address = array_pop($Parts);
		$Name = trim(implode(" ", $Parts));
		if ($Name)
			$Final = "\"".$Name."\" <".$Address.">";
		else
			$Final = $Address;
	}
}

//
// Example usage
//
 
// $attachment = fread(fopen("test.jpg", "r"), filesize("test.jpg")); 

// $mail = new Mail;
// $mail->from = "bill@ben.com";
// $mail->headers = "Reply-To: bill@ben.com";
//// $mail->parameters = "-fbill@ben.com";
// $mail->to = "bar@foo.com";
// $mail->subject = "Testing...";
// $mail->body = "This is just a test.";
// $mail->add_attachment("$attachment", "test.jpg", "image/jpeg");
// $mail->send();
 
?>
