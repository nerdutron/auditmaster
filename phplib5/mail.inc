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
// Attachments can now be embedded INSIDE emails
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
		$this->body = "";
		$this->headers = "";
		$this->parameters = "";
		$this->embedded = 0;
	}

//
// void add_attachment(string message, [string name], [string ctype])
// Add an attachment to the mail object
//
	function add_attachment($message, $name = "", $ctype = "application/octet-stream", $embedded = 0)
	{
		if ($embedded)
		{
			$pictureID = uniqid(time());
			$this->embedded++;
		}
		else
			$pictureID = "";

		$this->parts[] = array (
			"ctype" => $ctype,
			"message" => $message,
			"pictureID" => $pictureID,
			"name" => $name
			);

		if ($pictureID)
			return ("<img src=\"cid:".$pictureID."\">");
		else
			return ("");
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

		if($Part["pictureID"])
			$ReturnStr .= "\nContent-ID: <".$Part["pictureID"].">";
		elseif($Part["name"])
			$ReturnStr .= "\nContent-Disposition: inline; filename=\"".$Part["name"]."\"";

		$ReturnStr .= "\n\n$message\n";
		return $ReturnStr;
	}

//
//      void build_multipart()
//      Build a multipart mail
//
	function build_multipart() 
	{
		$boundary = "b".md5(uniqid(time()));
		if ($this->embedded)
			$multipart = "Content-Type: multipart/related; boundary=\"$boundary\"\n\n";
		else
			$multipart = "Content-Type: multipart/mixed; boundary=\"$boundary\"\n\n";
		$multipart .= "This is a MIME encoded message.\n\n--$boundary";

		for($i = sizeof($this->parts)-1; $i >= 0; $i--) 
			$multipart .= "\n".$this->build_message($this->parts[$i])."--$boundary";
		$multipart .= "--\n";
		return $multipart;
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
					$ID = str_pad((array_pop($FileID) + 1), 3, "0", STR_PAD_LEFT);
				else
					$ID = "000";

				$Path = MAIL_TO_DIR."MAIL_".$ID."_";
				file_put_contents($Path."message_".str_replace(array("/", " "), array("-", "_"), $this->subject).".htm", nl2br($this->body));
				foreach ($this->parts as $part)
					file_put_contents($Path.$part["name"], $part["message"]);
				return;
			}
		}

		$mime = "";
		if (!empty($this->from))
			$mime .= "From: ".$this->from."\n";
		if (!empty($this->headers))
			$mime .= $this->headers."\n";
		    
		if (!empty($this->body))
			$this->add_attachment(nl2br($this->body), "", "text/html; charset=\"ISO-8859-1\"");   

		$mime .= "MIME-Version: 1.0\n".$this->build_multipart();

		mail($this->to, $this->subject, "", $mime, $this->parameters);
	}

	function all_replies($email)
	{
		$this->parameters = "-f".$this->address_base($email);
		$email = $this->address_full($email);
		$this->from = $email;
		$this->headers = "Reply-To: ".$email;
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
// $mail->parameters = "-fbill@ben.com";
// $mail->to = "bar@foo.com";
// $mail->subject = "Testing...";
// $mail->body = "This is just a test.";
// $mail->add_attachment("$attachment", "test.jpg", "image/jpeg");
// $mail->send();
 
?>
