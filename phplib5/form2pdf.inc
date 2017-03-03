<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

//////////////////////////////
// Check Referer IP
// - Ian Bishop 4/9/02
//////////////////////////////

global $HTTP_REFERER;
require_once(PHPLIB_DIR."mail.inc");
require_once(PHPLIB_DIR."pdf/class.basepdf.inc");


if($HTTP_REFERER)
{
	$RefURL = parse_url($HTTP_REFERER);
	$RefHost = $RefURL["host"];
	$RefIP = gethostbyname($RefHost);
}
else
{
	$RefIP = "0.0.0.0";	
}

if ($RefIP != "127.0.0.1" 
 && $RefIP != "203.144.9.129" 
 && $RefIP != "203.48.131.1"
 && $RefIP != "192.168.1.100"
 && $RefIP != "192.168.2.254")
{
//	echo "Sorry, you are not permitted to access this file.\n\n";
//	exit;
}
//////////////////////////////

$Message = "";
$Password = "";
foreach ($_POST as $key=>$val)
{
	switch ($key)
	{
		case "F2P_ADDRESS":
		case "F2P_SUBJECT":
		case "F2P_URL":
		case "F2P_FILENAME":
		case "F2P_FROM":
			$$key = $val;
			break;

		case "F2P_SECURE":
			$Password = trim(file_get_contents($val));
			break;

		default:
			if(!$val)
				break;

			$Message .= "<b>".ucwords(strtolower(str_replace("_", " ", $key))).":</b>\n";
			if(is_array($val))
			{
				for ($i=0;$i<sizeof($val);$i++)
					$Message .= "    <C:Bullet>".$val[$i]."\n";
			}
			else
			{
				$Message .= $val."\n";
			}
			$Message .= "\n";
			break;
	}

}

// get uploaded files
$Files = array();
if(is_array($F2P_FILE))
{
	for($i=0; $i<sizeof($F2P_FILE); $i++)
	{
		$Files[$i]["FileName"] = $F2P_FILE[$i];
		$Files[$i]["OriginalName"] = $F2P_FILE_name[$i];
		$Files[$i]["Type"] = $F2P_FILE_type[$i];
	}
}
else if($F2P_FILE)
{
	$Files[0]["FileName"] = $F2P_FILE;
	$Files[0]["OriginalName"] = $F2P_FILE_name;
	$Files[0]["Type"] = $F2P_FILE_type;
}

// Generate Return Address if left blank
// - Ian Bishop 4/9/02
if(!$_POST["F2P_FROM"])
{
	$MX = str_replace("www.","",$RefHost);
	$F2P_FROM = "webmaster@$MX";
}

// make sure there is a filename for the PDF
if(!$F2P_FILENAME)
	$F2P_FILENAME = "form_data.pdf";

// build the PDF document
$PDF = new BASEPDF;
$PDF->ChooseHeaders("page");
$PDF->AddHeaderData($F2P_SUBJECT, 14, 1);
$PDF->AddHeaderData("Generated: ".date("j F Y H:i:s"), 8, 1);
$PDF->StartPDF("a4");
if($Password)
	$PDF->setEncryption($Password, $Password, array("print"));
$PDF->ezText($Message, 12);

// send the email
$M = new Mail;
$M->to = $F2P_ADDRESS;
$M->from = $F2P_FROM;
$M->subject = $F2P_SUBJECT;
$M->add_attachment($PDF->PDFGetData(), $F2P_FILENAME, "application/pdf");

// attach files
foreach($Files as $FileArray)
{
	extract($FileArray);

	// check file is valid
	if(!$FileName || $FileName == "none")
		continue;

	if(!is_uploaded_file($FileName))
		exit("could not very file");

	$M->add_attachment(file_get_contents($FileName), $OriginalName, $Type);
}

$M->send();

header("Location: ".$F2P_URL);
exit;

?>
